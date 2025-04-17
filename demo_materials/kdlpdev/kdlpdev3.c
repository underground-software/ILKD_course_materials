#include <linux/module.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static dev_t major_minor;

static int __init kdlpdev_init(void) {
	pr_info("init\n");

	int ret = alloc_chrdev_region(&major_minor, 0, 10, "kdlpdev");
	if (ret < 0) {
		pr_err("Unable to allocate major/minor: %d (%s)\n", ret, errname(ret));
		return ret;
	}

	char buf[64];
	pr_info("major:minor = %s\n", format_dev_t(buf, major_minor));

	return 0;
}

static void kdlpdev_cleanup(void) {
	pr_info("cleanup\n");
	unregister_chrdev_region(major_minor, 10);
}

module_init(kdlpdev_init);
module_exit(kdlpdev_cleanup);

MODULE_LICENSE("GPL");
