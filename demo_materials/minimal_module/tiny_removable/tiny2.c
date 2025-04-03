#include <linux/module.h>

int tiny_init(void) {
	pr_info("Hello, world!\n");
	return -123;
}

void tiny_exit(void) {
	pr_info("Goodbye, world!\n");
}

module_init(tiny_init);
module_exit(tiny_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joel Savitz <jsavitz@redhat.com>");
