#include <linux/module.h>

int tiny_init(void) {
	return 0;
}

module_init(tiny_init);
MODULE_LICENSE("GPL");
