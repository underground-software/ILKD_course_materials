#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static struct cdev kdlpdev;

static dev_t major_minor;

static struct file_operations kdlp_fops = {
	.owner = THIS_MODULE,
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
