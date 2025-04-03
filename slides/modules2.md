# modules 2: line by line

---

# Learning objective

Understand how to write some useful kernel code

---

# Overview

1. Describe the features of a simple program

1. Go through each change necessary to rewrite the program as a kernel module

1. Review the final product of this process

---

# User program: data flow

A string with length

1. Allocated and freed in `main`

```c
struct example
{
	char *message;
	size_t size;
};
```
---

# User program: data flow

Careful with memory

1. In userspace commonly not a material issue

1. Can check with valgrind

1. More serious in kernel

---


# User program: control flow

```
main() {
    handle = allocate("data")
    print(get(handle))
    set(handle, "new_data")
    print(get(handle))
    deallocate(handle)
}
```
---

# User program: control flow

Using `<string.h>`

---

# User program: error flow

Is goto considered ["harmful"](https://homepages.cwi.nl/~storm/teaching/reader/Dijkstra68.pdf)?

---

# User program: error flow

Benefits:

1. Less repetitive

1. Less indentation

As it is [written](https://www.kernel.org/doc/Documentation/process/coding-style.rst): "if you need more than 3 levels of indentation, you're screwed anyway and you should fix your program".



[(more)](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD10xx/EWD1009.html)

---

# User program: error flow

Pseudocode from user code:

```
    if unable to allocate x:
        goto no_need_to_free;
    if unable to allocate more:
        goto need_to_free;
success (fallthrough):
    return x
need_to_free:
    free(x)
no_need_to_free:
    return x;

```

---

# User program: error flow

Note:

Using `err()` from `<err.h>`

`err(3)` = `perror(3)` + `exit(2)`

---

# User program: error flow

`errno` clobbered by each systemcall

1. `temperrno` used to get around this

---

# User program: system flow

Using standard output:

```
$ ./user_example
hello
goodbye
```

---

# What do we need to change?

What works differently in the kernel?

---

# Switch to kernel headers

No C library in the kernel

```diff
-#include <err.h>
-#include <errno.h>
-#include <stdbool.h>
-#include <stdio.h>
-#include <stdlib.h>
-#include <string.h>
+#include <linux/module.h>
+#include <linux/string.h>
+#include <linux/slab.h>
```

---

# Switch to kernel headers

[<linux/module.h\>](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h)

1. Basic module building blocks

1. `module_{init,exit}()` macros defined

1. `MODULE_LICENSE()` macro defined

1. `printk()` indirectly via [<linux/dynamic_debug.h>](https://elixir.bootlin.com/linux/v6.13/source/include/linux/dynamic_debug.h#L302)

---

# Switch to kernel headers

[<linux/string.h\>](https://elixir.bootlin.com/linux/v6.13/source/include/linux/string.h)

1. `strlen()` has same name

1. `kstrdup()` is equivalent of `strdup()`
like `kstrdup()` take on new names and new arguments.

---

# Switch to kernel headers

[<linux/slab.h\>](https://elixir.bootlin.com/v6.13/latest/source/include/linux/slab.h)

1. `kmalloc()` and `kfree()`

1. [Kernel allocator](https://lwn.net/Articles/229984/)

---

# Anything wrong with this struct?

```c
struct example
{
	char *message;
	size_t size;
};
```

* no

---

# Digression: fixed size types

Kernel provides: `u8`, `u16`, ...

1. Dunder (e.g. `__u8`) versions [provided to userspace](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/int-l64.h#L21)

1. Compare to userspace `<stdint.h>`

1. Example: [`include/asm-generic/int-ll64.h`](https://elixir.bootlin.com/linux/v6.13/source/include/asm-generic/int-ll64.h#L17)

---

# Interface-specific types

Some common types have `typedef`s

1. e.g. `pid_t` hides int

1. Problems?

* Ambiguous, e.g. how to printf?

---

# typedef considered harmful

Just show me the type!

1. e.g. `struct task_struct`

---

# Memory allocation with benefits

`kmalloc()` takes an extra argument?

```diff
-	struct example *ex = malloc(sizeof *ex);
+	struct example *ex = kmalloc(sizeof *ex, GFP_KERNEL);
 	if(!ex)
 		goto out;
```

---

# Memory allocation with benefits

`kstrdup()` takes the same argument?

```diff
 	ex->size = strlen(msg);
-	ex->message = strdup(msg);
+	ex->message = kstrdup(msg, GFP_KERNEL);
 	if(!ex->message)
 		goto out_free;
```

---

# Memory allocation with benefits

`kfree()` works as expected

```diff
 	return ex;
 out_free:
-	free(ex);
+	kfree(ex);
 	ex = NULL;
 out:
 	return ex;
```

---

# Memory allocation with benefits

`kfree()` works as expected 2

```diff
@@ -30,17 +27,17 @@ out:
 
 static void example_destroy(struct example *ex)
 {
-	free(ex->message);
-	free(ex);
+	kfree(ex->message);
+	kfree(ex);
 }
```

---

# One more snippet of diff
 
```diff
 static bool example_update_message(struct example *ex, const char *msg)
 {
 	size_t size = strlen(msg);
-	char *data = strdup(msg);
+	char *data = kstrdup(msg, GFP_KERNEL);
 	if(!data)
 		return false;
-	free(ex->message);
+	kfree(ex->message);
 	ex->message = data;
 	ex->size = size;
 	return true;
```

---

# An additional allocation argument

From [`include/linux/slab.h`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/slab.h#L907)

```c
static __always_inline __alloc_size(1) void *kmalloc_noprof(size_t size, gfp_t flags)
```

---

# Macro wrapper: allocation tagging

[`alloc_hooks()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/alloc_tag.h#L233)

* For tracking memory allocations

* Security benefits e.g. [`KASAN: Kernel Address Sanitizer`](https://docs.kernel.org/dev-tools/kasan.html)

* Profiling and performance enhancements

---

# Simplified declaration

```diff
void *kmalloc(size_t size, gfp_t flags)
```

---

# What is a `gfp_t`?

From [`include/linux/slab.h`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/slab.h#L857)

```c
 * Below is a brief outline of the most useful GFP flags
 *
 * %GFP_KERNEL
 *	Allocate normal kernel ram. May sleep.
 *
 * %GFP_NOWAIT
 *	Allocation will not sleep.
 *
 * %GFP_ATOMIC
 *	Allocation will not sleep.  May use emergency pools.
 *
```

---

# It's just an unsigned int

From [`include/linux/types.h`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/types.h#L158)

```c
typedef unsigned int __bitwise gfp_t;
```

---

# More flags

```
 * Also it is possible to set different flags by OR'ing
 * in one or more of the following additional @flags:
 *
 * %__GFP_ZERO
 *	Zero the allocated memory before returning. Also see kzalloc().
 *
 * %__GFP_HIGH
 *	This allocation has high priority and may use emergency pools.
 *
 * %__GFP_NOFAIL
 *	Indicate that this allocation is in no way allowed to fail
 *	(think twice before using).
 *
 * %__GFP_NORETRY
 *	If memory is not immediately available,
 *	then give up at once.
 *
 * %__GFP_NOWARN
 *	If allocation fails, don't issue any warnings.
 *
 * %__GFP_RETRY_MAYFAIL
 *	Try really hard to succeed the allocation but fail
 *	eventually.
```

---

# A note on underscores

Section [`6.4.2`](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=73) of the C standard says:

"All identifiers that begin with a double underscore (__) or begin with an underscore (_)
followed by an uppercase letter are reserved for any use, except those identifiers which are
lexically identical to keywords"

---

# The Linux kernel C source is not standards compliant!

😳

* But: no collisions with compliant programs

---

# To simplify:

```c

foo(); // may be EXPORT_SYMBOL'd as part of an API

_foo(); // helper function called by foo()

__foo(); // more internal guts of _foo()

___foo(); // maybe going a little too far

________foo(); // HERE BE DRAGONS

```

---

# Usage varies by execution context

Process context: usually `GFP_KERNEL`

1. Can sleep, no emergency

Interrupt context: `GFP_ATOMIC`

1. Also called "atomic context"

---

# Goodbye main

```diff
-int main(void)
+int example_init(void)
```

Is this change necessary?

* Only for aesthetics and documentation

---

# Errors on the other side

```diff
+	int ret = -ENOMEM;
```

We don't check for errors, we *generate* the errors

---

# Analogy

|Concept|Userspace representation|Kernelspace representation|
|--|--|--|
|Files|`int fd`|`struct file`|
|Errors|library-provided `errno`|`return -ESOMERR`|

---

# Our error

Default to [`-ENOMEM`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/errno-base.h#L16)

1. We can't fail in any other way

1. Error value frequently defaults to `0` (success)

---

# What about pointer return values?

1. Generate with: `void * ERR_PTR(long err)`

1. Check with: `long IS_ERR(const void *ptr)`

---

# Buffering with style

```diff
+	const char *msg;
 	struct example *ex = example_create("hello");
+	msg = KERN_ERR "unable to allocate memory";
```
---

# What would make this legal?

What can appear right next to a string literal?

---

# Adjacent strings are concatenated

[Section 6.4.5 of the standard requires it](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=85)

---

# Logic

1. The kernel source code compiles

1. Adjacent strings literals are concatenated

1. Nothing else is valid C in our  context

1. Therefore, `KERN_ERR` resolves to a string literal

---

# But why?

From [Section 6.4.4.4 of the C standard](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=82)

1. Octal escape sequences are allowed

1. e.g. `\024`, `\21`, `\0`

1. digits in base 8 

---


From: [include/linux/kernel_levels.h](https://elixir.bootlin.com/linux/v6.13/source/include/linux/kern_levels.h#L11)

```c
#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

#define KERN_DEFAULT	""		/* the default kernel loglevel */
```

---

# Kernel logging levels

Relatively [well-documented](https://www.kernel.org/doc/html/v6.13/core-api/printk-basics.html)

---

# More common usage

`printk(KERN_ERR "foobar")`

1. Separated to emphasize the concatenation

---

# Preferred API

`pr_*` macros described in the [printk documentation](https://www.kernel.org/doc/html/v6.13/core-api/printk-basics.html)

1. Less room for error

1. Can re-define `pr_fmt()` for custom wrappers

---

# Cleaning up error handling with `goto`

```diff
 	if(!ex)
-		err(1, "unable to allocate memory");
-	printf("%s\n", example_get_message(ex));
-	if(!example_update_message(ex, "goodbye")) {
-		int temperrno = errno;
-		example_destroy(ex);
-		errno = temperrno;
-		err(1, "unable to update");
-	}
-	printf("%s\n", example_get_message(ex));
+		goto out;
```

---

# More `pr_info`

Print to the kernel ring buffer instead of standard out

```diff
+
+	pr_info("%s\n", example_get_message(ex));
+
+	msg = KERN_ERR "unable to update\n";
+	if(!example_update_message(ex, "goodbye"))
+		goto out_free;
+
+	pr_info("%s\n", example_get_message(ex));
```

---

# Three clean exits

```diff
+	ret = 0;
+	msg = NULL;
+out_free:
 	example_destroy(ex);
-	return 0;
+out:
+	if(msg)
+		printk(msg);
+	return ret;
+}
```

---

# A stub

Why do we need this?

```
+void example_exit(void)
+{
 }
```

* Without defining an exit function, we cannot unload the module

---

# The final plumbing

```diff
+module_init(example_init);
+module_exit(example_exit);
```

What are these?

---

# Generated by build process

`<module_name>.mod.c`

```c
__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};
```

---

# Where is this from?

[include/linux/module.h](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h#L410)

[`init()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h#L466) member type is `int (*)(void)`

[`exit()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h#L580) member type is `void (*)(void)`

---

# The meaning of the macros

[`module_init()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h#L130) defined here

`__inittest()` used for type checking

The [alias function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-alias-function-attribute) binds `init_module` to our chosen identifier.

The `cleanup_module` function is similarly generated

---

![One more thing](https://www.macworld.com/wp-content/uploads/2023/01/every-one-more-thing-apple-has-ever-announced-main-1.png)

---

# Required metadata

```diff
+MODULE_LICENSE("GPL");
```

Build [fails without this](https://elixir.bootlin.com/linux/v6.13/source/scripts/mod/modpost.c#L1568)

---

# Metadata generation

[`#define __MODULE_INFO(...)`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/moduleparam.h#L23)

1. Generates "license=GPL" string found in binary

---

# Summary: changes to data flow

Use `kmalloc()/kfree()` to call kernel memory allocator

1. Additional argument depending on execution context

1. Be very careful with memory in the kernel!

---

# Summary: changes to control flow

Entry point for load-time setup specified by `module_init()`

Entry point for unload-time cleaup specified by `module_exit()`

---

# Summary: changes to control flow

Don't be afraid of a little `goto` action

1. Be afraid of 3+ layers of indentation

---

# Summary: changes to error flow

Return negative errno values 

---

# Summary: changes to system flow

Print informational messages to kernel ring buffer

1. View with the `dmesg` commmand

```dmesg
[33684.303081] hello
[33684.303277] goodbye
```

---

# End

Further reading: <https://kdlp.underground.software/articles/module_translation>

1. Slide deck based on this article
