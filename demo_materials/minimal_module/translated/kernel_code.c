#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

struct example
{
	char *message;
	size_t size;
};

static struct example *example_create(const char *msg)
{
	struct example *ex = kmalloc(sizeof *ex, GFP_KERNEL);
	if(!ex)
		goto out;
	ex->size = strlen(msg);
	ex->message = kstrdup(msg, GFP_KERNEL);
	if(!ex->message)
		goto out_free;
	return ex;
out_free:
	kfree(ex);
	ex = NULL;
out:
	return ex;
}

static void example_destroy(struct example *ex)
{
	kfree(ex->message);
	kfree(ex);
}

static bool example_update_message(struct example *ex, const char *msg)
{
	size_t size = strlen(msg);
	char *data = kstrdup(msg, GFP_KERNEL);
	if(!data)
		return false;
	kfree(ex->message);
	ex->message = data;
	ex->size = size;
	return true;
}

static char *example_get_message(struct example *ex)
{
	return ex->message;
}

int example_init(void)
{
	int ret = -ENOMEM;
	const char *msg;
	struct example *ex = example_create("hello");
	msg = KERN_ERR "unable to allocate memory";
	if(!ex)
		goto out;

	pr_info("%s\n", example_get_message(ex));

	msg = KERN_ERR "unable to update\n";
	if(!example_update_message(ex, "goodbye"))
		goto out_free;

	pr_info("%s\n", example_get_message(ex));

	ret = 0;
	msg = NULL;
out_free:
	example_destroy(ex);
out:
	if(msg)
		printk(msg);
	return ret;
}

void example_exit(void)
{
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
