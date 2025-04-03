#include <linux/module.h>

int minimal_init(void) {
	return 0;
}

module_init(minimal_init);
MODULE_LICENSE("GPL");
