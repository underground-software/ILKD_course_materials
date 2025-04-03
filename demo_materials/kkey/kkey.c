#include <linux/cdev.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/byteorder/generic.h>

// contains ioctl cmds provided to userspace
#include "kkey.h"

#undef 	pr_fmt // to remove redefinition warning 
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

// most significant bit always 0 since midi notes are all in 0-127 range
#define MIDI_NUM_NOTES 128

#define MIDI_MAX_VELOCITY 127

const static struct __packed midi_header {
	u8  header_label[4];
	u32 header_size;
	u16 format_type;
	u16 num_tracks;
	u16 ticks_per_quarter_note;
} midi_header = {
	.header_label = "MThd",
	.header_size = __cpu_to_be32(6), // midi format is big endian
	.format_type = __cpu_to_be16(0), // 0 denotes a single track file
	.num_tracks =  __cpu_to_be16(1), // must be 1 for format_type == 0
	// we will use HZ so 1 jiffie = 1 midi tick at 4/4 time and 60 BPM
	.ticks_per_quarter_note = __cpu_to_be16(HZ),
};

struct midi_track_header {
	u8 track_header_label[4];
	u32 track_length;
};

struct midi_note_event {
	u8 channel:4;
	u8 cmd:4;
	u8 note;
	u8 velocity;
};

const static struct midi_meta_event {
	u8 cmd;
	u8 meta_cmd;
	u8 data;
} event_end_of_track = {
	.cmd = 0xFF, 		// meta event
	.meta_cmd = 0x2F, 	// end of track meta event subtype
	.data = 0,		// always 0
};

#define KKEY_MAX_EVENTS (1UL << 14) // 16384

#define KKEY_NAME "kkey"

struct kkey_event {
	u64		timestamp;	// jiffies
	unsigned 	note;
	bool		off_on;
};

static struct kkey {
	dev_t devnum;
	struct class * class;
	struct cdev cdev;
	struct device * devices[MIDI_NUM_NOTES];
	struct kkey_event events[KKEY_MAX_EVENTS];
	size_t size;
} kkey;

static bool dirty = true;

static struct kkey_attrs {
	u32 slowdown;
	u8  velocity;
} kkey_attrs = {
	// increase this value by x to slow down playback by a factor of x
	.slowdown = 10,
	// arbitary constant value near midpoint for simplicity
	.velocity = 63,
};

static ssize_t attr_velocity_show(const struct class *class, const struct class_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%hhu\n", kkey_attrs.velocity);
}

static ssize_t attr_velocity_store(const struct class *class, const struct class_attribute *attr, const char *buf, size_t count)
{
	char kbuf[16];
	if (count >= sizeof(kbuf))
		return -EINVAL;

	memcpy(kbuf, buf, count);
	kbuf[count] = '\0';

	u8 velocity;
	if (kstrtou8(kbuf, 10, &velocity) || velocity > MIDI_MAX_VELOCITY)
		return -EINVAL;

	kkey_attrs.velocity = velocity;

	dirty = true;
	return count;
}

static ssize_t attr_slowdown_show(const struct class *class, const struct class_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%u\n", kkey_attrs.slowdown);
}

static ssize_t attr_slowdown_store(const struct class *class, const struct class_attribute *attr, const char *buf, size_t count)
{

	char kbuf[16];
	if (count >= sizeof(kbuf))
		return -EINVAL;

	memcpy(kbuf, buf, count);
	kbuf[count] = '\0';

	u32 slowdown;
	if (kstrtou32(kbuf, 10, &slowdown) || slowdown == 0)
		return -EINVAL;

	kkey_attrs.slowdown = slowdown;

	dirty = true;
	return count;
}

static struct class_attribute kkey_attr_velocity = {
	.attr.name = "kkey_velocity",
	.attr.mode = 0666,
	.show = attr_velocity_show,
	.store = attr_velocity_store,
};

static struct class_attribute kkey_attr_slowdown = {
	.attr.name = "kkey_slowdown",
	.attr.mode = 0666,
	.show = attr_slowdown_show,
	.store = attr_slowdown_store,
};


// big enough to fit any u64 delta value (7 * 10 >= 64)
#define KKEY_MAX_DELTA_BYTES 10

// A delta value is an arbitrarily long series of 7-bit chunks with MSB = 1 for all but the last
static size_t write_delta(u64 delta, u8 * dest)
{
	u8 buf[KKEY_MAX_DELTA_BYTES] = { 0 }, *iter = buf, flag = 0;

	// less significant bits will be repesented last in the series of bytes due to big endian format
	do {
		*iter++ = (delta & 0x7F) | flag;
		flag = 0x80;
		delta >>= 7;
	} while (delta);

	// ret contains number of bytes for this delta value
	size_t ret = iter - buf;
	for (int i = 0; i < ret; ++i) {
		// last value in iter is first value in dest, and so on
		*dest++ = *--iter;
	}

	return ret;
}

static int kkey_open(struct inode * inode, struct file * filep)
{
	unsigned note = iminor(inode);

	filep->private_data = (void *)(uintptr_t)note;

	return 0;
}

static int kkey_close(struct inode * inode, struct file * filep)
{
	return 0;
}

static u8 midi_buf[sizeof(struct midi_header) + sizeof(struct midi_track_header) +
	(KKEY_MAX_DELTA_BYTES + sizeof(struct midi_note_event)) * KKEY_MAX_EVENTS +
	KKEY_MAX_DELTA_BYTES + sizeof(struct midi_meta_event)];

static size_t midi_size;

static void prepare_midi(void)
{
	u8 * iter = midi_buf, *track_header_ptr, *data_ptr;
	memcpy(iter, &midi_header, sizeof midi_header);

	iter += sizeof(struct midi_header);
	track_header_ptr = iter;

	iter += sizeof(struct midi_track_header);
	data_ptr = iter;

	// this intial value ensures first delta is 0
	// if size == 0, this timetamp may be junk but we don't use it anyway
	u64 last_timestamp = kkey.events[0].timestamp;
	for (size_t i = 0; i < kkey.size; ++i) {
		u64 this_timestamp = kkey.events[i].timestamp;
		iter += write_delta(this_timestamp - last_timestamp, iter);
		last_timestamp = this_timestamp;
		struct midi_note_event event = {
			// we only have one channel whose index is 0
			.channel = 0,
			// 8 or 9 to indicate off and on respectively
			.cmd = 0x8 + kkey.events[i].off_on,
			.note = (u8)kkey.events[i].note,
			.velocity = kkey_attrs.velocity,
		};
		memcpy(iter, &event, sizeof event);
		iter += sizeof event;
	}

	iter += write_delta(0, iter);
	memcpy(iter, &event_end_of_track, sizeof event_end_of_track);
	iter += sizeof event_end_of_track;

	struct midi_track_header track_header = {
		.track_header_label = "MTrk",
		.track_length = cpu_to_be32(iter - data_ptr),
	};
	memcpy(track_header_ptr, &track_header, sizeof track_header);

	midi_size = iter - midi_buf;
	dirty = false;

	if (kkey_attrs.slowdown != 1) {
		u16 new_ticks = cpu_to_be16(HZ / kkey_attrs.slowdown);
		memcpy(track_header_ptr - sizeof(u16), &new_ticks, sizeof(u16));
	}
}

static ssize_t kkey_read(struct file * filep, char * __user buf, size_t count, loff_t *fpos)
{
	// cannot continue reading from middle of file after it's been invalidated
	if (dirty) {
		if (*fpos)
			return -EIO;
		else
			prepare_midi();
	}

	// makes e.g. cat work by providing EOF
	if (*fpos >= midi_size)
		return 0;

	count = min(count, midi_size - *fpos);

	if (copy_to_user(buf, midi_buf + *fpos, count))
		return -EFAULT;

	*fpos += count;

	return count;
}

static ssize_t kkey_write(struct file * filep, const char * __user buf, size_t count, loff_t *fpos)
{
	if (kkey.size >= KKEY_MAX_EVENTS) {
		pr_err("write: too many events (%zu)\n", kkey.size);
		return -ENOSPC;
	}

	if (count == 0)
		return 0;

	u8 input;
	if (get_user(input, buf)) {
		pr_err("unable to write to kernel\n");
		return -EFAULT;
	}

	kkey.events[kkey.size] = (struct kkey_event){
		.timestamp = get_jiffies_64(),
		.note = (unsigned)(uintptr_t)filep->private_data,
		.off_on = input != '0',
	};

	kkey.size++;

	dirty = true;
	
	return count;
}


static long kkey_ioctl(struct file * filep, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case KKEY_IOC_RESET:
		kkey.size = 0;
		dirty = true;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static loff_t kkey_llseek(struct file * filep, loff_t off, int whence)
{
	if (dirty)
		prepare_midi();

	loff_t base;
	switch (whence) {
	case SEEK_CUR:
		base = 0;
		break;
	case SEEK_SET:
		base = filep->f_pos;
		break;
	case SEEK_END:
		base = midi_size;
		break;
	default:
		return -EINVAL;
	}

	return (filep->f_pos = base + off);
}

struct file_operations kkey_fops = {
	.owner = THIS_MODULE,
	.open = kkey_open,
	.release = kkey_close,
	.read = kkey_read,
	.write = kkey_write,
	.unlocked_ioctl = kkey_ioctl,
	.llseek = kkey_llseek,
};

// entries in /dev belonging to class kkey should be RW to all users
static char * kkey_devnode(const struct device *dev, umode_t * mode)
{
	if (mode) {
		*mode = 0666;
	}

	return NULL;
}

static int __init kkey_init(void)
{
	int ret;

	if ((ret = alloc_chrdev_region(&kkey.devnum, 0, MIDI_NUM_NOTES, KKEY_NAME))) {
		pr_err("failed to allocate chrdev major/minor region: %s\n", errname(ret));
		goto err_alloc_chrdev_region;
	}

	kkey.class = class_create(KKEY_NAME);
	if (IS_ERR(kkey.class)) {
		ret = PTR_ERR(kkey.class);
		pr_err("failed to create device class: %s\n", errname(ret));
		goto err_class_create;
	}

	kkey.class->devnode = kkey_devnode;

	cdev_init(&kkey.cdev, &kkey_fops);

	if ((ret = cdev_add(&kkey.cdev, kkey.devnum, MIDI_NUM_NOTES))) {
		pr_err("failed to add kkey cdev: %s\n", errname(ret));
		goto err_cdev_add;
	}

	int i;
	for (i = 0; i < MIDI_NUM_NOTES; i++) {
		struct device * dev_i = device_create(kkey.class, NULL, MKDEV(MAJOR(kkey.devnum), i), NULL, KKEY_NAME "%03d", i);
		if (IS_ERR(dev_i)) {
			ret = PTR_ERR(dev_i);
			pr_err("failed to create kkey device %d: %s\n", i, errname(ret));
			goto err_device_create;
		}
		kkey.devices[i] = dev_i;
	}

	if ((ret = class_create_file(kkey.class, &kkey_attr_velocity))) {
		pr_err("failed to add velocity sysfs attr file: %s\n", errname(ret));
		// same cleanup as partial failure of device create
		goto err_device_create;
	}

	if ((ret = class_create_file(kkey.class, &kkey_attr_slowdown))) {
		pr_err("failed to add slowdown sysfs attr file: %s\n", errname(ret));
		goto err_sysfs_class_file_slowdown;
	}

	return 0;

err_sysfs_class_file_slowdown:
	class_remove_file(kkey.class, &kkey_attr_velocity);
err_device_create:
	for (; i > 0; i--)
		device_destroy(kkey.class, MKDEV(MAJOR(kkey.devnum), i-1));
err_cdev_add:
	class_destroy(kkey.class);
err_class_create:
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
err_alloc_chrdev_region:
	return ret;
}

static void kkey_exit(void)
{
	class_remove_file(kkey.class, &kkey_attr_slowdown);
	class_remove_file(kkey.class, &kkey_attr_velocity);
	for (int i = 0; i < MIDI_NUM_NOTES; i++)
		device_destroy(kkey.class, MKDEV(MAJOR(kkey.devnum), i));
	cdev_del(&kkey.cdev);
	class_destroy(kkey.class);
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
}

module_init(kkey_init);
module_exit(kkey_exit);

MODULE_LICENSE("GPL");
