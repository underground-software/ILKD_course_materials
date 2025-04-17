#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static struct cdev kdlpdev;

static dev_t major_minor;

static int kdlpdev_open(struct inode * inode, struct file * filep) {

	unsigned long bytes;
	get_random_bytes(&bytes, sizeof(bytes));

	pr_info("open %d:%d and assign %lu\n", imajor(inode), iminor(inode), bytes % 256);

	filep->private_data = (void *)(bytes % 256);

	return 0;
}

static int kdlpdev_close(struct inode * inode, struct file * filep) {
	pr_info("close %d:%d\n", imajor(inode), iminor(inode));

	return 0;
}

static ssize_t kdlpdev_read(struct file * filep, char * __user buf, size_t count, loff_t * fpos) {
	pr_info("read %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	char _buf[64];
	size_t buflen = sprintf(_buf, "kdlpdev's random number is %lu\n", (unsigned long)filep->private_data % 256);
	if (buflen < 0) {
		pr_err("Unable to write to _buf\n");
		return -ENOMEM;
	}

	if (*fpos >= buflen)
		return 0;

	if (count > buflen - *fpos)
		count = buflen - *fpos;

	size_t bytes_not_written = copy_to_user(buf, _buf + *fpos, count);
	if (bytes_not_written > 0)
		return -EFAULT;

	*fpos += count;

	pr_info("read %zu bytes\n", count);

	return count;
}

static ssize_t kdlpdev_write(struct file * filep, const char * __user buf, size_t count, loff_t * fpos) {
	pr_info("write %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	return -ENOTRECOVERABLE;
}

static struct file_operations kdlp_fops = {
	.owner = THIS_MODULE,
	.open = kdlpdev_open,
	.release = kdlpdev_close,
	.read = kdlpdev_read,
	.write = kdlpdev_write,
};

static int __init kdlpdev_init(void) {
	pr_info("init\n");

	int ret = alloc_chrdev_region(&major_minor, 0, 10, "kdlpdev");
	if (ret < 0) {
		pr_err("Unable to allocate major/minor: %d (%s)\n", ret, errname(ret));
		return ret;
	}

	char buf[64];
	pr_info("major:minor = %s\n", format_dev_t(buf, major_minor));

	cdev_init(&kdlpdev, &kdlp_fops);
	cdev_add(&kdlpdev, major_minor, 1);

	return 0;
}

static void kdlpdev_cleanup(void) {
	pr_info("cleanup\n");
	unregister_chrdev_region(major_minor, 10);
	cdev_del(&kdlpdev);
}

module_init(kdlpdev_init);
module_exit(kdlpdev_cleanup);

MODULE_LICENSE("GPL");
