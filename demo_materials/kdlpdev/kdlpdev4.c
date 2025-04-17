#include <linux/module.h>

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static int __init kdlpdev_init(void) {
	pr_info("init\n");
	return 0;
}

static void kdlpdev_cleanup(void) {
	pr_info("cleanup\n");
}

module_init(kdlpdev_init);
module_exit(kdlpdev_cleanup);

MODULE_LICENSE("GPL");
