#include <linux/cdev.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define MIDI_NUM_NOTES 128

// TODO: NOTE: ALERT: MIDI FORMAT IS BIG ENDIAN

#define MIDI_HEADER_CHUNK_BEGIN 0x4D54686400000006	// "MThd" + u32 header length (always 6)

#define MIDI_FORMAT_SINGLE_TRACK 	0
#define MIDI_FORMAT_MULTI_TRACK_SYNC 	1
#define MIDI_FORMAT_MULTI_TRACK_ASYNC 	2

#define MIDI_60_BPM_IN_TICKS_PER_QUARTER_NOTE 0x60

// 4/4 time = 1 BPS/60 BPM
// 1 quarter note = HZ * jififes

#define MIDI_TRACK_CHUNK_BEGIN 0x4D54726B		// "MTrk"

#define KKEY_NAME "kkey"

#define KKEY_DEFAULT_MSGCAP 4096

#define MIDI_DELTA_MAX_SIZE sizeof(u8)


/*
start: 0 = release with velocity 63, 1 = press with velocity 63

level 2:
x = 0-127, press with velocity x
(x = 0-127) + 0x80 (128-255), release with velocity x

jiffies:
	record jiffies value, on/off bool, note on each event
	when reading: difference between jiffies maps to delta value

*/

enum midi_event {
	MIDI_EVENT_NOTE_OFF 	= 0x08,
	MIDI_EVENT_NOTE_ON 	= 0x09,
	MIDI_EVENT_META 	= 0xFF,
};

enum midi_meta_event {
	MIDI_META_EVENT_END_OF_TRACK = 0x2F,
};

union midimsg {
	struct {
		u8 delta;
		u8 cmd_and_channel;	// bits 0-3: channel, bits 4-7: midi_event_cmd
		u8 note_number;
		u8 velocity;
	} notemsg;
	struct {
		u8 delta;
		u8 cmd;
		u8 meta_cmd;
		u8 data;
	} metamsg;
	char raw[4];
};

union midifile {
	struct {
		u64 header_chunk_begin;
		u8 header_chunk_mode;
		u8 num_tracks;
		u8 ticks_per_quarter_note;
		u32 track_chunk_begin;
		u32 track_length_bytes;
		union midimsg * msgs;
		size_t size;
		size_t cap;
		union midimsg end_of_track;
	} midi;
	struct {
		char raw1[sizeof(u64) + sizeof(u8) * 3 + sizeof(u32) * 2];
		union midimsg * msgs;

		char raw2[sizeof(size_t) * 2 + sizeof(union midimsg)];
	} raw;
};

struct midinote {
	u8 note;
	struct device * device;
	union midifile * file_ref;
};

struct kkey {
	dev_t devnum;
	struct class * class;
	struct cdev cdev;
	struct midinote notes[MIDI_NUM_NOTES];
	struct mutex lock;
	union midifile file;
} kkey;

static int kkey_open(struct inode * inode, struct file * filep) {
	pr_info("open\n");

	struct device * kkey_device = class_find_device_by_devt(kkey.class, inode->i_rdev);
	if (!kkey_device) {
		char buf[64];
		pr_err("device not found for major/minor %s\n", format_dev_t(buf, inode->i_rdev));
		return -ENODEV;
	}
	struct midinote * note = dev_get_drvdata(kkey_device); // container_of??
	filep->private_data = note;

	return 0;
}

static int kkey_close(struct inode * inode, struct file * filep) {
	pr_info("close\n");
	return 0;
}


static ssize_t kkey_read(struct file * filep, char * __user buf, size_t count, loff_t *fpos) {
	pr_info("read");

	struct midinote * note = (struct midinote *)filep->private_data;
	union midifile * file_ref = note->file_ref;

	if (*fpos >=1)
		return 0;

	size_t filesize = sizeof(u64) + sizeof(u8) * 3 + sizeof(u32) * 2 + sizeof(char) * file_ref->midi.size + sizeof(size_t) * 2 + sizeof(union midimsg);

	count = min(filesize, count);

	copy_to_user(buf, file_ref->raw.raw1, sizeof(u64) + sizeof(u8) * 3 + sizeof(u32) * 2);

	for (int i = 0; i < file_ref->midi.size; i++)
		copy_to_user(buf, file_ref->midi.msgs[i].raw, sizeof(char) * 4);

	copy_to_user(buf, file_ref->raw.raw2, sizeof(size_t) * 2 + sizeof(union midimsg));

	*fpos += 1;

	return count;
}


static inline bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}


#define KKEY_INPUT_MAXLEN 64
static ssize_t kkey_write(struct file * filep, const char * __user buf, size_t count, loff_t *fpos) {
	pr_info("write");

	union midimsg msg;
	char kbuf[KKEY_INPUT_MAXLEN + 1];
	int bytes_not_copied, i;
	struct midinote * note = (struct midinote *)filep->private_data;
	union midifile * file_ref = note->file_ref;

	count = min(count, KKEY_INPUT_MAXLEN);

	if ((bytes_not_copied = copy_from_user(kbuf, buf, count))) {
		pr_err("write: failed to copy %d bytes from user\n", bytes_not_copied);
		return -EFAULT;
	}

	kbuf[count] = '\0';

	int offsets[2], offsets_seen = 0;
	char c;
	for (i = 0; offsets_seen < 2 && (c = kbuf[i]); i++) {
		if (!is_digit(c) && c != '-') {
			pr_err("invalid charcter %c in input string '%s'\n", c, kbuf);	// BUG when null term placed already
			return -1;
		}

		if (kbuf[i] == '-') {
			offsets[offsets_seen++] = i;
			kbuf[i] = '\0';
		}
	}

	if (offsets_seen < 2) {
		pr_err("missing %d dashes in input string '%s'\n", 2 - offsets_seen, kbuf);
		return -1;
	}

	u64 inputs[3] = { 0, 0, 0 }; // = { delta, on/off, velocity }
	if (kstrtoull(kbuf, 10, &inputs[0]) || kstrtoull(kbuf + offsets[0], 10, &inputs[1]) ||
			kstrtoull(kbuf + offsets[1], 10, &inputs[2])) {
		pr_err("failed to convert one of the input strings to an integer\n");
		return -ERANGE;
	}

	if (file_ref->midi.size >= file_ref->midi.cap) {
		pr_err("NO SPACE!!! :(\n");
		return -ENOSPC;
	}
	i = file_ref->midi.size;
	file_ref->midi.msgs[i].notemsg.delta = inputs[0]; // only 7 bits available here, truncate all deltas accordingly
       	// only on or off >:( (must be followed by 4 zeros for channel number)
	file_ref->midi.msgs[i].notemsg.cmd_and_channel = (MIDI_EVENT_NOTE_OFF + inputs[1] % 2) << 4;
	file_ref->midi.msgs[i].notemsg.note_number = note->note;
	file_ref->midi.msgs[i].notemsg.velocity = inputs[2];

	file_ref->midi.size++;

	return count;
}

static long kkey_ioctl(struct file * filep, unsigned int cmd, unsigned long arg) {
	pr_info("ioctl");
	return 0;
}

static loff_t kkey_llseek(struct file * filep, loff_t off, int whence) {
	pr_info("llseek");
	return 0;
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
static char * kkey_devnode(const struct device *dev, umode_t * mode) {
	if (mode) {
		*mode = 0666;
	}

	return NULL;
}

static int __init kkey_init(void) {
	int ret;
	pr_info("init start");

	if ((ret = alloc_chrdev_region(&kkey.devnum, 0, MIDI_NUM_NOTES, KKEY_NAME))) {
		pr_err("failed to allocate chrdev major/minor region: %s", errname(ret));
		goto err_alloc_chrdev_region;
	}

	char buf[64];
	pr_info("allocated major:minor = %s through minor %d\n", format_dev_t(buf, kkey.devnum), MIDI_NUM_NOTES);

	kkey.class = class_create(KKEY_NAME);
	if (IS_ERR(kkey.class)) {
		ret = PTR_ERR(kkey.class);
		pr_err("failed to create device class: %s\n", errname(ret));
		goto err_class_create;
	}

	pr_info("class '%s' created\n", KKEY_NAME);

	kkey.class->devnode = kkey_devnode;

	cdev_init(&kkey.cdev, &kkey_fops);

	if ((ret = cdev_add(&kkey.cdev, kkey.devnum, MIDI_NUM_NOTES))) {
		pr_err("failed to add kkey cdev: %s\n", errname(ret));
		goto err_cdev_add;
	}

	pr_info("added cdev\n");

	int i;
	for (i = 0; i < MIDI_NUM_NOTES; i++) {
		struct device * dev_i = kkey.notes[i].device;
		dev_i = device_create(kkey.class, NULL, MKDEV(MAJOR(kkey.devnum), i), &kkey.notes[i], KKEY_NAME "%d", i);
		if (IS_ERR(dev_i)) {
			ret = PTR_ERR(dev_i);
			pr_err("failed to create kkey device %d: %s\n", i, errname(ret));
			goto err_device_create;
		}
	}

	pr_info("created %d kkey devices\n", MIDI_NUM_NOTES);

	kkey.file.midi.msgs = kzalloc(sizeof(union midimsg) * KKEY_DEFAULT_MSGCAP, GFP_KERNEL);
	if (!kkey.file.midi.msgs) {
		ret = -ENOMEM;
		pr_err("failed to allocate %d midimsg structs: %s\n", KKEY_DEFAULT_MSGCAP, errname(ret));
		goto err_device_create;
	}


	kkey.file.midi.header_chunk_begin = MIDI_HEADER_CHUNK_BEGIN;
	kkey.file.midi.header_chunk_mode = MIDI_FORMAT_SINGLE_TRACK;
	kkey.file.midi.num_tracks = 1;
	kkey.file.midi.ticks_per_quarter_note = MIDI_60_BPM_IN_TICKS_PER_QUARTER_NOTE;

	kkey.file.midi.track_chunk_begin = MIDI_TRACK_CHUNK_BEGIN;
	kkey.file.midi.num_tracks = 1; //WRONG?????? big/lil endian issue possible?
				  //
	kkey.file.midi.size = 0;
	kkey.file.midi.cap = KKEY_DEFAULT_MSGCAP;

	kkey.file.midi.end_of_track.metamsg.delta = 0;
	kkey.file.midi.end_of_track.metamsg.cmd = MIDI_EVENT_META;
	kkey.file.midi.end_of_track.metamsg.meta_cmd = MIDI_META_EVENT_END_OF_TRACK;
	kkey.file.midi.end_of_track.metamsg.data = 0;

	for (i = 0; i < MIDI_NUM_NOTES; i++) {
		kkey.notes[i].file_ref = &kkey.file;
		kkey.notes[i].note = i;
	}

	return 0;

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

static void kkey_exit(void) {
	kfree(kkey.file.midi.msgs);
	for (int i = 0; i < MIDI_NUM_NOTES; i++)
		device_destroy(kkey.class, MKDEV(MAJOR(kkey.devnum), i));
	cdev_del(&kkey.cdev);
	class_destroy(kkey.class);
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
	pr_info("exit end\n");
}

module_init(kkey_init);
module_exit(kkey_exit);

MODULE_LICENSE("GPL");
