# modules 2: line by line

1. Learning objective: Understand how to write some useful kernel code

1. Overview

    1. Describe the features of a simple program

    1. Go through each change necessary to rewrite the program as a kernel module

    1. Review the final product of this process

1. User program

Before we proceed, let's note a few key features.

*Data flow**


```c
struct example
{
	char *message;
	size_t size;
};
```

This pair of elements represents a simple byte string and its size.
Take note that both the data structure itself
and the memory located at `message`
can be allocated either statically or dynamically,
and we take care to ensure that these two layers are handled appropriately.

Our typical userspace entry point,
the `main` function,
declares a pointer to one of these `struct example` types
and then immediately assigns the return value of a constructor-style
function `example_create()`,
whose job is to encapsulate the finer details of allocation and initialization.

In good style, `main` is responsible for cleaning up its own mess,
and this task is executed right before `main` returns back to the C library
at the bottom of the function by invocation of `example_destroy()`.
When implementing a more complex program, we may pass a pointer to our
local reference in order to zero the value to avoid subsequent misuse by the caller,
i.e. a dangling pointer, however this is unnecessary complexity for this simple example
and it suffices to simply ensure that our program does not leak memory.
Usage of the userspace tool `valgrind` will validate this property of our program,
but we do admit for a short-lived program such as this example whose memory is cleaned up
by the kernel at termination, the fuss and rigor around memory leaks appears pedantic beyond
the practice of good habits. Though practice is reason enough,
we will soon find ourselves in kernelspace where there is no one to clean up after us.
In the kernel, a memory leak will persist until reboot and in the meantime will clog the tubes of the
[memory allocator](https://lwn.net/Articles/229984/).


**Control Flow**

Our example program implements a control flow that should
not raise the eyebrows of a C programmer with beyond novice-level skill.
We don't do anything fancy with the entry point,
and we don't create any threads.
We invoke a constructor to allocate our memory in fairly standard form,
using the old reliable `malloc` function from the `<stdlib.h>` section
of the trusty C library. During instantiation, we make a couple of calls
to the `<string.h>` section in the form of `strlen()` and `strdup()`,
both of which assume as a precondition a nicely null-terminated input string
as the `msg` parameter. Likewise, we perform the same operations
in `example_update_message()`, assuming the same precondition.

Each call to `malloc()` pairs with a corresponding call to `free()`,
both at the level of the allocated message and the data structure itself,
and in just the same pattern our `example_create()` constructor function pairs
with our `example_destroy()` destructor function.
The `example_get_message()` implements a getter and `example_update_message()`
implements a setter. The complexity of the latter is due to the need to duplicate the
byte-string `msg` argument and free the now-junk memory residing at the
address contained in `ex->message`.

**Error Flow**

A careful reader of our example may take alarm at a particular feature.
We too have heard these rumors, that the C `goto` statement is considered
["harmful"](https://homepages.cwi.nl/~storm/teaching/reader/Dijkstra68.pdf).
Despite these tall tales, we inform you with confidence that while there
are many paths to correct code, the
[shortest path](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)
to readability and maintainability is often by use of this fearsome little keyword.
For one, correct usage of `goto` and error case labeling as seen in our example
eliminates the need for repetitive code and unnecessary indentation.
As it is
[written](https://www.kernel.org/doc/Documentation/process/coding-style.rst):
"if you need more than 3 levels of indentation, you're screwed anyway and you should fix your program".
We will not elaborate any
[further](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD10xx/EWD1009.html).

Next, note our usage of `err()` from `<err.h>`.
This handy tool lets us perform the work of `perror()` and `exit`
with a single invocation.
We first pass the return code we would have handed to `exit`
and then we specify the string snatched from the jaws of `perror()`.

The final point worth noting is our usage of `temperrno`.
Think of this as if we were "pushing" the value of `errno`
at that instance onto the stack,
like we would do at the assembly level for a register
before a jump or call into a section of code that may clobber said register.
Usage of the C library function `free()`
in our call to `example_destroy()` may overwrite the previous value of `errno`,
but this is the relevant value to report in the context
of cleaning up after a failed call to `example_update_message()`.


**System Flow**

The program sends the following text to `stdout` when run:

```
$ ./user_example
hello
goodbye
```

Other than this, the program does not interact with the system
in any manner worth noting.

#### Leaving Kansas

Now that we have analyzed the user code with
excruciatingly thorough exposition,
let us turn to the primary task at hand.

In order to satisfy what we assume to be our reader's
ravenous appetite for kernel module code and alleviate
the all-too-familiar pangs of hunger for privileged execution,
we'll begin by dropping the complete `diff -up` output between
the above program and its kernel equivalent:

```diff
$ diff -Naup user_code.c kernel_code.c
--- user_code.c	2023-11-07 23:30:25.792075105 -0500
+++ kernel_code.c	2023-11-07 23:30:16.628563819 -0500
@@ -1,9 +1,6 @@
-#include <err.h>
-#include <errno.h>
-#include <stdbool.h>
-#include <stdio.h>
-#include <stdlib.h>
-#include <string.h>
+#include <linux/module.h>
+#include <linux/string.h>
+#include <linux/slab.h>
 
 struct example
 {
@@ -13,16 +10,16 @@ struct example
 
 static struct example *example_create(const char *msg)
 {
-	struct example *ex = malloc(sizeof *ex);
+	struct example *ex = kmalloc(sizeof *ex, GFP_KERNEL);
 	if(!ex)
 		goto out;
 	ex->size = strlen(msg);
-	ex->message = strdup(msg);
+	ex->message = kstrdup(msg, GFP_KERNEL);
 	if(!ex->message)
 		goto out_free;
 	return ex;
 out_free:
-	free(ex);
+	kfree(ex);
 	ex = NULL;
 out:
 	return ex;
@@ -30,17 +27,17 @@ out:
 
 static void example_destroy(struct example *ex)
 {
-	free(ex->message);
-	free(ex);
+	kfree(ex->message);
+	kfree(ex);
 }
 
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
@@ -51,20 +48,39 @@ static char *example_get_message(struct
 	return ex->message;
 }
 
-int main(void)
+int example_init(void)
 {
+	int ret = -ENOMEM;
+	const char *msg;
 	struct example *ex = example_create("hello");
+	msg = KERN_ERR "unable to allocate memory";
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
+
+	pr_info("%s\n", example_get_message(ex));
+
+	msg = KERN_ERR "unable to update\n";
+	if(!example_update_message(ex, "goodbye"))
+		goto out_free;
+
+	pr_info("%s\n", example_get_message(ex));
+
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
+
+void example_exit(void)
+{
 }
 
+module_init(example_init);
+module_exit(example_exit);
+
+MODULE_LICENSE("GPL");
+

```

The length of this `diff` output exceeds the length of the original user program.
We will proceed with an explanation of each change.

#### Welcome to Oz

The transition to writing kernel code is a shift to another plane of reality.
Previous assumptions about what a C program looks like may no longer hold,
and the reader may encounter strange looking constructs and ludicrously
deep layers of macro invocations, generating the sense of a fever dream.
When all appears to be lost, bear in mind one key point:
There is no escape from the kernel.
The kernel has been running since the CPU exited the bootloader
and only a semi-magical illusion has hidden this raw truth from your eyes.
Today, we lift this curse from the reader,
revealing, as the scales fall from their eyes,
the vibrant glory of kernel module code,
and forever dispelling the last remnant
of prestidigitation from their mental model of the computer.
Magic no more! The entirety of the machine,
software and hardware stack united as one,
lies bare before the attentive reader,
and nothing, save polynomial time factoring of large numbers,
remains beyond reach.

Well then, lets get started.

**Switch to kernel headers**

First off, the C standard library is not available
within the kernel, so we discard the inclusion
of the header files that provide C library
declarations:

```
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

Instead, we include headers declaring
Linux kernel API entry points.
These paths are relative to the `include`
directory within the kernel repository.

The first,
[<linux/module.h\>](https://elixir.bootlin.com/linux/latest/source/include/linux/module.h),
provides the basic building blocks for a kernel module,
such as `#define`s of the `module_init()` and `module_exit()` macros we encounter later on.
Importantly, this file also `#define`s the mandatory `MODULE_LICENSE()` macro,
which we will return to at the end, as well as `printk()` and the associated macros.

Next, we include
[<linux/string.h\>](https://elixir.bootlin.com/linux/latest/source/include/linux/string.h)
to replace some of the functionality we accessed via the C library's `string.h`.
Some of the functions retain their familiar names, like `strlen()`, while others
like `kstrdup()` take on new names and new arguments.

Finally, in order to allocate and free memory, we include
[<linux/slab.h\>](https://elixir.bootlin.com/linux/latest/source/include/linux/slab.h),
which gives us the duo of `kmalloc()` and `kfree()`,
second cousins of the familiar userspace versions.

That's all for the `#include`s.
Here we can briefly note that the `struct example` we defined in userspace
is perfectly suitable for usage in kernelspace, so we skip right over it.

**Memory allocation with a twist**

```
 struct example
 {
@@ -13,16 +10,16 @@ struct example
 
 static struct example *example_create(const char *msg)
 {
```

Now, we arrive at our first usage of `kmalloc()`.
Like userspace `malloc()`,
this function takes a number of bytes to allocate
as its first argument, but `kmalloc()` takes a mysterious
second argument. In fact, this is the same argument
passed as the mysterious second argument to `kstrdup()`.
Luckily for the simplicity of this paragraph, `kfree()`
works exactly like `free()`.

```
-	struct example *ex = malloc(sizeof *ex);
+	struct example *ex = kmalloc(sizeof *ex, GFP_KERNEL);
 	if(!ex)
 		goto out;
 	ex->size = strlen(msg);
-	ex->message = strdup(msg);
+	ex->message = kstrdup(msg, GFP_KERNEL);
 	if(!ex->message)
 		goto out_free;
 	return ex;
 out_free:
-	free(ex);
+	kfree(ex);
 	ex = NULL;
 out:
 	return ex;
@@ -30,17 +27,17 @@ out:
 
 static void example_destroy(struct example *ex)
 {
-	free(ex->message);
-	free(ex);
+	kfree(ex->message);
+	kfree(ex);
 }
 
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
@@ -51,20 +48,39 @@ static char *example_get_message(struct
 	return ex->message;
 }
```

The changes to the three functions `example_init()`,
`example_destroy()`, and `example_update_message()` are
all limited to these three substitutions, two of which introduce
this mysterious second `GFP_KERNEL` argument.
We will pause here to discuss this in more depth before getting into
the real funky stuff.

We find the declaration of kmalloc in the latter half of
[`include/linux/slab.h`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/slab.h#L590)
and the included comment provides us with far more articulate explication than we could muster.

We include a snippet of the
[Linux `v6.6` kmalloc comment](https://elixir.bootlin.com/linux/v6.6/source/include/linux/slab.h#L548)
verbatim:

```
 * The @flags argument may be one of the GFP flags defined at
 * include/linux/gfp_types.h and described at
 * :ref:`Documentation/core-api/mm-api.rst <mm-api-gfp-flags>`
 *
 * The recommended usage of the @flags is described at
 * :ref:`Documentation/core-api/memory-allocation.rst <memory_allocation>`
 *
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


The curious reader should feel free to pursue any rabbit hole
referenced within that comment.

The signature of `kmalloc()` itself is quite simple when the funny business is hidden:

		void *kmalloc(size_t size, gfp_t flags)

The second argument is a `typedef`ed wrapper for what is really nothing more than a fancy
[`unsigned int`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/types.h#L154),
but, in good style,
these implementation details are hidden from us
unless we search for them.
Essentially, this second `flags` argument is used to specify additional options
to the memory allocator.
One could easily implement such a compact bit-flags argument in userspace,
and certainly many of our readers have done so,
but we understand the confusion a novice kernel programmer may encounter
when forced to select options from a menu of foreign-language items in order
to perform a task as apparently simple as memory allocation.

Let us back up a couple of steps and motivate this complexity.
As we noted in our discussion of the userspace program in the "Data Flow" section,
there is no other process within a system who will come save the kernel.
Without expanding the scope of our analysis
[beyond a single system](https://en.wikipedia.org/wiki/Virtual_machine)
or into the realm of
[exotic hardware](https://en.wikipedia.org/wiki/Intel_Active_Management_Technology),
we must operate under the knowledge that
the kernel is the sovereign and absolute monarch of a computer system
from the time that the bootloader kindly requests that the CPU jump into the kernel code
to the time the computer is either reset or physically destroyed.
While this absolute authority grants the CPU the enjoyment of maximally privileged execution,
this absolute responsibility yokes the CPU with the burden of maximally privileged execution.

When we write kernel code, in this case a kernel module that allocates and frees memory,
we can't just blindly type up some half-baked garbage willy-nilly
and grind out a compile/valgrind/debug loop until all the errors are ironed out.
Certainly
[there are tools](https://docs.kernel.org/dev-tools/kmemleak.html)
for searching the kernel for memory leaks,
but the instrumentation of the kernel is not nearly as trivial
as the runtime instrumentation performed by `valgrind`.
To zoom into our particular context, take a closer look at the three `GFP_*` flags
in the `kmalloc()` comment which are not prefixed by a double underscore ("dunder"):

```
 * %GFP_KERNEL
 *	Allocate normal kernel ram. May sleep.
 *
 * %GFP_NOWAIT
 *	Allocation will not sleep.
 *
 * %GFP_ATOMIC
 *	Allocation will not sleep.  May use emergency pools.
```

We briefly note a
[(non-standards compliant)](https://stackoverflow.com/questions/73542215/leading-underscores-in-linux-kernel-programming)
design choice:
identifiers that begin with an underscore
are more "internal" than those without one,
and those two are are extra internal.
While internal is doing a lot of heavy lifting
in that sentence, the context of each usage clarifies the details.
A less "internal" API function may be
[exported as a symbol](https://docs.kernel.org/core-api/symbol-namespaces.html)
to the rest of the kernel,
while a more "internal" identifier may provide an entry point
to a kernel function that skips certain locking steps,
or in case of
[`_copy_from_user()`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/uaccess.h#L143),
[permissions and protection checks](https://unix.stackexchange.com/questions/674962/why-are-copy-from-user-and-copy-to-user-needed-when-the-kernel-is-mappe).
In the case of the `GFP_*` flags above,
the dunder versions are declared as such
to hint to kernel engineers that these flags
are generally not used directly like the non-dunder versions.

As can be validated by a `ctrl+f`,
our kernel module uses `GFP_KERNEL`.
This is because we are running in the context of
a user process and therefore it's ok if the
codepath of the allocation includes a sleep or two
before returning to the caller.
We may even schedule out and switch processes multiple times
before the allocation spits out the needed valid memory address.
However, the CPU may be executing code in a context
where sleep is not only undesirable,
but theoretically terminal for the entire system.
One example of such a context is within the
[top-half or bottom-half](https://static.lwn.net/images/pdf/LDD3/ch10.pdf#page=18)
of an interrupt handler.

The crucial topic of kernel context
deserves its own thorough treatment,
so we will only briefly touch upon it here.
The essential difference for our purpose
is that kernel code can sleep in user context,
while it cannot sleep in atomic or interrupt context.
In process context, we have a process associated with
the running kernel thread, though the immediate business
of the kernel may not be directly relevant to that particular
userspace process.
These kernel threads can copy data to or from userspace memory,
send signals to the current process,
and generally muck around with the
[`struct task_struct`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/sched.h#L743)
found by dereferencing the address the `current` macro resolves to.
On the other hand,
a kernel thread running in interrupt or atomic context
is not associated with any userspace process.
Though `current` will point to the process whose execution
this kernel thread is interrupting,
this thread must accomplish its business as soon as possible.
It cannot sleep at all,
so any memory allocation must return immediately.
The `GFP_NOWAIT` flag requests this behavior with less urgency,
however the `GFP_ATOMIC` flag marks the allocation request with
a huge, red, bold exclamation mark attached,
and requests to be fed with the emergency reserves in the case of low memory.
This is sane, as we would like something like our keyboard to be able to
send interrupts that are immediately received and processed,
even when the bloated closed-source
software we run by choice or by force decides to consume all of our system resources.

tl;dr just use `GFP_KERNEL` unless you have a good reason not to.


At last, we move on to the changes to our classical userspace entry point.

**Entry to the other side**

```
-int main(void)
+int example_init(void)
```

This change simply renames `main` to `example_init`.
Do not take this for any sort of magic
as this is nothing but a naming convention
whose purpose will be discussed near the bottom
of this diff analysis.
We could just as well call our module initialization function `main`,
but this would be confusing.
The demotion of this function
from the known entry point styled `main`
sets our footing loose from
that familiar foundation
of the userspace coding environment,
and we will return to this concern
near the bottom of this diff analysis.

```
+	int ret = -ENOMEM;
```

While the classic idiom of a
print to standard error and
nonzero-argument invocation of the exit syscall
consolidated with the `err()` API call suited our needs
quite satisfactorily back in Kansas,
we will find this exit strategy
falls flat on its face here in Oz.
To begin with, this exit strategy
relies on the invocation of a system call,
that is to say,
an explicit invocation of the kernel by userspace code,
and more specifically, a request for the kernel
to terminate the calling process.
As we are already executing in kernel mode,
there is no need to invoke ourselves,
and we certainly don't wish to commit suicide
on behalf of anyone in the failure case,
least of all on behalf of the kernel itself.
Instead, as the userspace integral file descriptor
is to the kernelspace `struct file`,
the thread-local userspace integral errno variable is
to the kernelspace negative integral errno value.
Though the specific reason for the convention of negativity
is unimportant and perhaps
[unknowable](https://stackoverflow.com/questions/1848729/why-return-a-negative-errno-e-g-return-eio),
one should take note of the convention itself.
We default to the negated out-of-memory errno value of
[-ENOMEM](https://elixir.bootlin.com/linux/v6.6/source/include/uapi/asm-generic/errno-base.h#L16)
as the return code for our function since
that is the only error we check for.
Once we confirm that we are in fact able
to allocate the necessary memory,
we set this value to zero.
One may frequently see code
that defaults the value of the return code to zero.
A careful treatment of that flamewar
is beyond the scope of this section.

When one of these errno return values is propagated
all the way back to userspace in the context
of a systemcall,
the userspace caller will then be able to
access this value via the thread-local errno variable.

Keep in mind that a thread-local variable in userspace
corresponds to a per-task variable from the perspective of kernelspace.
A process ID in kernelspace, known as a `pid`, corresponds one-to-one
with a userspace thread ID, known as a `tid`.
Confusingly, a userspace process is identified by
the more common usage of the same term "process ID" or `pid`,
which contains one or more threads, each identified by
a unique thread ID, or `tid`.
When a userspace process contains but a single thread,
the `pid` and the `tid` are the same,
and the kernelspace `pid` refers to the `struct task_struct`
representing the single userspace thread.
For a multi-threaded userspace process,
a userspace `pid` is associated with multiple `tid` values,
and each of these userspace `tid` values corresponds
one-to-one with a kernelspace `pid` value and a representative
`struct task_struct` as the Linux implementation of
the more general concept of a
[Process control block](https://en.wikipedia.org/wiki/Process_control_block).
These threads are grouped together logically,
and so as one might expect, the kernel refers to
the collection of kernelspace `pid` values grouped
under a single userspace `pid` value as userspace `tid`s
by the term "Thread-group ID", abbreviated as `tgid`.

To summarize:

|Concept|Userspace name|Kernelspace name|
|--|--|--|
|Single thread|`tid`|`pid`|
|Logical Process|`pid`|`tgid`|

**Buffering with style**

```c
+	const char *msg;
 	struct example *ex = example_create("hello");
+	msg = KERN_ERR "unable to allocate memory";
```

Though this construct appears strange at first glance,
we will quickly demystify this last assignment
with a quick exposition of C string syntax.
[Section 6.4.5](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=85)
of the C standard defines
the syntax of a string literal.
As a C-literate reader should expect,
a string literal is defined to be a series of characters
from a slight restriction of the character set called "s-chars"
in between terminating double quote characters.
Optionally, the string may be prefixed by what the standard terms an "encoding prefix"
but the details of that are not important here.
To quote the 1 April 2023 working draft, an "s-char" is:
"any member of the source character set except
the double-quote ", backslash \, or new-line character".
[Section 5.1.1.2](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=29)
specifies the order of precedence for translation stages during compilation,
and we see that item 6 clearly states that:
"Adjacent string literal tokens are concatenated."
Therefore, by process of elimination and
before even looking up the definition of `KERN_ERR`,
we know that `KERN_ERR` must be a string literal
because this code compiles and we have no other option.
That covers the syntactic mystery,
but it does not explain the semantics of this statement.

Allow us one more quick detour that will be necessary just below.
[Section 6.4.4.4](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=82)
of the standard specifies various character constants,
including the encoding prefixes we mention just above.
We see that an "octal-escape-sequence" is a valid "escape-sequence",
and that it is specified with one, two, or three octal digits following a backslash.
The "octal-escape-sequence" is the only one
which is implemented with no character between the backslash
and the value itself.
For example, one begins a hexadecimal escape sequence using "\x",
and a
[universal character name](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=75)
using a "u" or "U".


Let us turn to the
[definition](https://elixir.bootlin.com/linux/v6.6/source/include/linux/kern_levels.h#L11)
of this symbol in the `kern_levels.h` header,
whose brief 39 lines we will include in their entirety from the v6.6 source:


```c
$ cat include/linux/kern_levels.h
/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __KERN_LEVELS_H__
#define __KERN_LEVELS_H__

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

/*
 * Annotation for a "continued" line of log printout (only done after a
 * line that had no enclosing \n). Only to be used by core/arch code
 * during early bootup (a continued line is not SMP-safe otherwise).
 */
#define KERN_CONT	KERN_SOH "c"

/* integer equivalents of KERN_<LEVEL> */
#define LOGLEVEL_SCHED		-2	/* Deferred messages from sched code
					 * are set to this special level */
#define LOGLEVEL_DEFAULT	-1	/* default (or last) loglevel */
#define LOGLEVEL_EMERG		0	/* system is unusable */
#define LOGLEVEL_ALERT		1	/* action must be taken immediately */
#define LOGLEVEL_CRIT		2	/* critical conditions */
#define LOGLEVEL_ERR		3	/* error conditions */
#define LOGLEVEL_WARNING	4	/* warning conditions */
#define LOGLEVEL_NOTICE		5	/* normal but significant condition */
#define LOGLEVEL_INFO		6	/* informational */
#define LOGLEVEL_DEBUG		7	/* debug-level messages */

#endif
```

By examination of the above header,
we observe the resolved value of `KERN_ERR`
to be a string literal containing two bytes,
the "\001" octal escape sequence which represents
"start of heading" in the
[ASCII](https://man7.org/linux/man-pages/man7/ascii.7.html)
standard, followed by the ASCII character literal "3",
which can just as easily be represented using "\063",
however the kernel chooses to be readable.

This may be obvious by this point,
but these bytes are used to specify
the relatively
[well-documented](https://www.kernel.org/doc/html/latest/core-api/printk-basics.html)
kernel logging level.
The usage of usage of the `KERN_*` prefix before a string literal
is generally done within the parenthesis of a `printk()` invocation,
such as the one we use later in the code,
however we assign the resulting string value to a local `char *` variable
to demonstrate what is really going on
and dispel any illusions the reader may hold.
We believe this more verbose,
multi-step usage is less likely to trigger
that part of the trained C programmer's brain
which says that there is a comma missing.

Though direct usage of `printk()` is acceptable,
we recommend the usage of the `pr_*` macros
described in the
[printk documentation](https://www.kernel.org/doc/html/latest/core-api/printk-basics.html),
as these helpful wrappers will prevent
one's attempted kernel build
from generating strange-looking macro-resolution errors
in the case one makes a typo.
Usage of `KERN_ERROR` is such an example.
We believe it will be easier to spot the error
when one attempts to build kernel code containing
the alternative equivalent mistaken usage of `pr_error()`
in place of the correct `pr_err()`.
In addition, the code is cleaner and shorter when
using the `pr_*` family of functions,
and you can define customized wrappers on a per-file basis
by redefining `pr_fmt()`, which we will explain below.

```
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

With our handy `goto` statement,
we can dispose of all that mid-function
error handling code and consolidate the codepaths
of this function to flow through a single exit point.

```
+
+	pr_info("%s\n", example_get_message(ex));
+
+	msg = KERN_ERR "unable to update\n";
+	if(!example_update_message(ex, "goodbye"))
+		goto out_free;
+
+	pr_info("%s\n", example_get_message(ex));
```

Here, we make use of the `pr_info()` macro helper
to do exactly what `printk()` would have done,
but without having to include that strange looking
syntax prefixing the format string with a macro
separated by nothing but whitespace.
Actually, as we mention above,
the `pr_*` family provides one extra feature
that we do not use but we feel is worth a quick discussion.

The
[definition](https://elixir.bootlin.com/linux/v6.6/source/include/linux/printk.h#L528)
of `pr_info()`
passes the format string wrapped with yet another macro,
this being `pr_fmt`.
As the
[API documentation](https://www.kernel.org/doc/html/latest/core-api/printk-basics.html#c.pr_fmt) tells us,
we can define a custom format to be used each time
a `pr_*` macro is subsequently invoked in that translation unit.
The example given in the documentation is
yet another macro, `KBUILD_MODNAME`,
Which is resolved at build time by
[Kbuild](https://docs.kernel.org/kbuild/kbuild.html),
the Linux kernel's bespoke build system,
and a flag set by
[scripts/Makefile.lib](https://elixir.bootlin.com/linux/v6.6/source/scripts/Makefile.lib#L126)
is passed to the compiler, defining this value appropriately in each context.
This is common, but one may use any string they like,
or leave out the definition entirely, as we do in this module.

These two invocations of `pr_info()`
are the kernelspace replacements
for the two `printf()` calls back in our userspace code,
and here too, the success of these two calls results in
the strings "hello" and "goodbye" appearing in some external buffer.

**Three clean exits**

```
+	ret = 0;
```
The value of `ret` before this assignment is `-ENOMEM`,
so we must clear the error and set the return value
to 0, which indicates success.

```
+	msg = NULL;
```

As the `msg` variable contains an error message,
we set it to `NULL` to skip the invocation of `printk()` just below.

```
+out_free:
 	example_destroy(ex);
-	return 0;
+out:
+	if(msg)
+		printk(msg);
+	return ret;
+}
```
Finally, we conclude the definition of `example_init()`
by overlapping three exit cases together
using the `goto` statements defined earlier and the two labels
we define just above.
This is less complex than it may seem,
and as you may notice, we only use one level of indentation.

First, the success case.
If all goes right, the CPU arrives at the code following
the `out_free` label, continues right along
after invocation of `example_destroy()`,
moves right past `out`, and jumps past the `printk()`
due to the `NULL` value of `msg` set just above.
We return with the value of `ret` set to `0`,
which is also taken care of just above,
and that's that for an error-free execution of `example_init()`.

Second, our first call to `kmalloc()` to allocate memory
for a `struct example` may fail.
Then, our error-checking conditional leaves us
on the `goto out;` line just following,
and right away, the CPU is then executing just below the `out` label.
At this point, the value of `msg` is
the string "unable to allocate memory"
prefixed by "\001" "3", a.k.a `KERN_ERR`.
As this value is in fact not `NULL`,
we pass it to `printk()`
and we expect to see this string show up in our kernel ring buffer.
As always, we can check this with `dmesg`.
To conclude, we return the value of `ret`,
which is unmodified since its initialization and declaration
and therefore is `-ENOMEM`, which is correct.

Third and finally,
we may succeed in allocating memory
for a `struct example`,
but then fail somewhere in `example_update_message()`,
which is indicated by a logically false, i.e. `0` return value
from the conditional wrapped invocation.
We can inspect this short function
and see that this failure can only happen in a single case,
and that case is also failed allocation,
but this detail is not important here.
What is important in the context of handling this error in the caller
is that we are responsible for `free`ing the memory
we allocated just before this to store our `struct example`.
If we were to simply return to the caller of `example_init()` right now,
not only would we lose the syntactically clean unified exit path,
we would generate a memory leak.
We also want to print the contents of the string data
at `msg`'s address to the kernel ring buffer,
and for whatever remains of brevity in this example,
we don't bother modifying the contained string.
Therefore, we jump over the second `pr_info()` invocation
and the assignment of appropriate success-case values
to `ret` and `msg`,
and immediately invoke `example_destroy()` on the address
we obtained from the initial and successful call to `kmalloc()`.
This closes the loop in terms of allocation
and prevents the module from leaking memory.
Do not forget that the severity of a memory leakage in the kernel
is almost always far greater than in a user program,
especially a short-lived one.
As you may recall from the exposition above,
should we modify our example user program program above
to leak memory, which can be implemented by the removal
of one or more calls to `free()`, we can easily debug
the issue with `valgrind`,
and regardless,
the kernel will clean up our mess
upon termination of the process and its threads.
In the kernel, every similar memory leak
will persist until the system is reset.
We emphasize this to illustrate the importance
of correctly managing the memory of kernel code
even in the more subtle codepaths such as this third case.
Once we free the memory at `ex`,
the non-`NULL` value of `msg` triggers the call to `printk()`
just as in the second case,
and finally,
we return  `-ENOMEM`,
also just like the second case.

```
+void example_exit(void)
+{
 }
```
We define this empty function because we need
to give a callable address with a particular type signature
to the kernel's module subsystem.
This is explained just below.

**The final plumbing**

```
+module_init(example_init);
+module_exit(example_exit);
```

In order to properly explain
these two macro invocations,
we first need to take a step back
and talk about the bigger picture.

We are translating a C program designed
to compile into an executable binary file
that creates a single thread and interacts with Linux from userspace
into a C program designed to
compile into the Linux implementation of a
[loadable kernel module](https://en.wikipedia.org/wiki/Loadable_kernel_module)
that interacts with the kernel API
and expects to run on a CPU in privileged mode.
To build and run this code,
we first need to write an idiomatic makefile
and make sure the files necessary to build
modules specifically for the running or target kernel
are present in their expected locations.
When all of this is in place,
we can build a "kernel object" file,
whose filename is canonically but meaninglessly
suffixed with ".ko".
Using a utility like
[`insmod`](https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git/tree/tools/insmod.c)
we can pass this kernel object
to either the
[`init_module(2)` or `finit_module(2)`](https://man7.org/linux/man-pages/man2/init_module.2.html)
syscall, though in practice the `insmod` and `modprobe` utilities from
[kmod](https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git) exclusively invoke the
[latter](https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git/tree/shared/missing.h#n36)
due to an engineering preference for working with file descriptors.

This syscall loads the module into kernel memory,
and if needed, relocates symbols and initializes module parameters.
After this, the kernel invokes the module's `init` function.
Now as we have made abundantly clear by now,
the `main` function that the C standard so generously specifies in
[section 5.1.2.2.1](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=31)
is not relevant to a Linux kernel module.
Instead of using a pre-defined name as our entry point,
we simply set the module's init function
to the address of a function of our choice
with the only constraint being the type signature,
which must be `int (*)(void)`.
Within the definition of the intuitively-named
[struct module](https://elixir.bootlin.com/linux/v6.6/source/include/linux/module.h#L402),
we find a member named
[`init`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/module.h#L458),
with just the type signature we expect.
This `init` member holds the address of the init function
defined by a given module and
this `struct module` is the in-kernel representation
of a Linux kernel module.
Likewise, when we wish to unload a kernel module,
we use a tool such as
[`rmmod`](https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git/tree/tools/rmmod.c)
or the
[removal mode](https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git/tree/tools/modprobe.c#n114)
of modprobe,
which passes the name of the module into the kernel by way of the
[`delete_module(2)`](https://man7.org/linux/man-pages/man2/delete_module.2.html)
syscall.
After checking whether the supplied name refers to
an extant loaded kernel module
with no outstanding references
held by other modules,
the kernel checks whether an `exit` function
is defined for the module.
If so, it is invoked before the module is unloaded.
The address of this `exit` function,
just like `init`,
is stored within a module's `struct module`
as a member helpfully named
[`exit`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/module.h#L568).
While specification of a module `init` function is mandatory,
specification of an `exit` function is not.
If we don't ever need or want to unload the module,
then `exit` will never be called,
so we can exclude it entirely.
Since we do in fact wish to be able to unload our module
but we don't have anything to cleanup at unload time,
we simply define a dummy function and set `exit` to its address.

We now return to the point
from which we took a step back,
namely, the usage of the `module_init` and `module_exit`
seen just above.
This is the method we use
to set the `init` and `exit` members
of the soon-to-be-generated `struct module`
that will be packaged into the kernel object file
by the kernel build system.
The two macros are
[defined](https://elixir.bootlin.com/linux/latest/source/include/linux/module.h#L130)
one right after the other.
The first part of the definition may initially bamboozle the reader,
however when we take away the semantically irrelevant
[`static` storage class specifier](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=116),
the
[`inline` function specifier](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf#page=141),
and
[`unused` function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-unused-function-attribute)
hiding right behind the
[`__maybe_unused` macro](https://elixir.bootlin.com/linux/v4.14/source/include/linux/compiler-gcc.h#L128),
we find the definition of a dummy function named `__inittest`
which takes no arguments and returns a value of type `initcall_t`.
The sole statement in the function body returns the address
of our candidate to be set as the module's `init` function.
The `unused` attribute hints at the true intent of this function.
The compiler will throw an error if the type signature of our chosen function
differs from that of
[initcall_t](https://elixir.bootlin.com/linux/v6.6/source/include/linux/init.h#L118).
This dummy function implements that compliance check at compile time,
saving all users the headache of debugging the runtime consequences
of a module author mistakenly using something exotic and noncompliant.
The final line is the business end of the macro definition.
We declare a function named `init_module` with the same type signature as `initcall_t`.
Then, we utilize the
[alias function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-alias-function-attribute)
to bind the address of our `init` function to the `init_module` symbol.
One of the artifacts generated and used by a kernel module build
is a file given the same filename as the primary C program source,
but with a `.mod.c` extension replacing the simple `.c`.
This file contains the following snippet, or something similar:

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

Thus the `init` function of our choosing is set as the module's `init` function
and made available to the kernel on a module load via its inclusion in this
generated `struct module`.
The `cleanup_module` function is similarly generated
by the `module_exit` macro.
This is how the kernel knows about the `example_init` and `example_exit`
functions in our kernel module example.


```
+MODULE_LICENSE("GPL");

```

This line is required.
If we attempt to build our module without it,
we find that the modpost stage of the kernel build system
[complains and explodes](https://elixir.bootlin.com/linux/v6.6/source/scripts/mod/modpost.c#L1731).
This suicide stems from a failed check for a string
beginning with "`license=`"
in the module binary.
This string is emitted by the
[definition of `__MODULE_INFO`](https://elixir.bootlin.com/linux/v6.6/source/include/linux/moduleparam.h#L23)
which is the powerhouse of the
[`MODULE_LICENSE` macro](https://elixir.bootlin.com/linux/v6.6/source/include/linux/module.h#L230).
We use the string `"GPL"` to refer to the
[GNU General Public License](https://en.wikipedia.org/wiki/GNU_General_Public_License),
specifically
[version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).
Our usage of this license designation in the module source code
assigns this free software license to our code.
This allows any individual or company to ensure
that their usage of our module or any other
complies with their legal and philosophical constraints.

#### The Emerald City

We have reached the end
of the yellow brick road
of our diff.
Although everything you need
to generate the final kernel driver is above,
we include the final result right here
for emphasis and ease.

```c
$ cat kernel_code.c
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>

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
```

For further convenience,
we include an idiomatic makefile
which will build the above kernel module
on a properly configured system.

```Makefile
$ cat Makefile
obj-m += kernel_code.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(shell pwd)
clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(shell pwd)
load:
	sudo insmod kernel_code.ko
unload:
	-sudo rmmod kernel_code
```
