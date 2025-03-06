## VFS: open syscall

Learning objective: Follow the outline of the open syscall

### Overview

1. Invocation of open
2. Internal translation to main handler
3. Manipulating the file descriptor table

### What does open need to do?

Thoughts?

* Get a handle to an `inode`

* Create entry in the file descriptor table

* Invoke underlying filesystem-specific code


### FDT: file descriptor table

[`struct files_struct`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fdtable.h#L38)

[`->files in task_struct`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/sched.h#L1154)

### General process

1. Resolve a path
2. Perform validation
3. Create entry in file descriptor table

#### The obvious entry point

[`SYSCALL_DEFINE3(open,...`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1421)

`openat(2)` is preferred

### `force_o_largefile()`

Do we have [`ARCH_32_BIT_OFF_T`](https://elixir.bootlin.com/linux/v6.13/source/arch/Kconfig#L348)

1. If not, all file offsets can be 64-bit

### demo

`strace ./hello_world`

### C Library prefers openat

[`open` becomes `openat`](https://elixir.bootlin.com/glibc/glibc-2.40.9000/source/sysdeps/unix/sysv/linux/open.c#L31)

1. [`SYSCALL_CANCEL()`](https://elixir.bootlin.com/glibc/glibc-2.40.9000/source/sysdeps/unix/sysdep.h#L118) used for thread semantics

### Non-AT versions deprecated

[list of syscalls](https://elixir.bootlin.com/linux/v6.13/source/include/linux/syscalls.h#L1019)

### A fake file descriptor

[`AT_FDCWD`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/linux/fcntl.h#L93)

1. Open relative to current working directory

### New: `struct open_how`

1. Built from `int flags` and `umode_t mode`
2. For openat, done in [`do_sys_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1414)
3. Added in fddb5d430ad9f ("open: introduce openat2(2) syscall")

### Why the need for another open?

1. openat didn't check for unknown `flags`
2. Extending with new features troublesome

### New structure

[`struct open_how`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/linux/openat2.h#L19)

### Open's name resolution

Follow symlinks in each component of patch

By default, follow for last component, unless `O_NOFOLLOW`

1. `O_PATH`: another exception, open path without following

### New RESOLVE flag highlights

1. `RESOLVE_BENEATH`: all resolutions in subtree
2. `RESOLVE_NO_ROOT`: can't cross into another mount namespace
3. `RESOLVE_NO_SYMLINKS`: `Like O_NOFOLLOW` for whole path resolution process

### Creating the new structure

[`build_open_how()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1186)

1. Invalid flags quietly discarded
2. `openat2` would `-EINVAL` with bad flags

### A note on mode

[`S_IALLUGO`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/stat.h#L10): [`permission bits`](https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html)

read, write, exec for user, group, and other

1. Also: setuid, setgid, sticky bit

### More validation

[`build_open_flags()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1202)

`struct open_how` => `struct open_flags`

### Name resolution

[`getname called`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1396) => [`getname_flags`](https://elixir.bootlin.com/linux/v6.13/source/fs/namei.c#L129)

1. Like a `copy_from_user()` for filenames
2. allocates memory, performs validation
3. namei: name-to-inode

### easter egg?

What the heck is "omirr"

Important enough for [fundamental changes to pathname lookup](https://elixir.bootlin.com/linux/v6.13/source/fs/namei.c#L49)

[In and out of the kernel](https://lkml.iu.edu/hypermail/linux/kernel/9901.2/1232.html)

1. What became of "omirr"?

### FDT Origin

[`SYSCALL_DEFINE0(fork)`](https://elixir.bootlin.com/linux/v6.13/source/kernel/fork.c#L2888)

[`kernel_clone()` calls `copy_process()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/fork.c#L2806)

[`copy_process()` calls `copy_files()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/fork.c#L2382)

[`copy_files()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/fork.c#L1775)

### Back into the file code

[`dup_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L363)

1. Structure is [cached](https://elixir.bootlin.com/linux/v6.13/source/kernel/fork.c#L3186)

[`alloc_fdtable()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L157)

### Get next available fd

[`get_unused_fd_flags()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L591)

### Back to the FDT

[`do_filp_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/namei.c#L4006)

1. Relies on [`path_openat()`](https://elixir.bootlin.com/linux/v6.13/source/fs/namei.c#L3967)
2. Finally: [`do_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/namei.c#L3784)
   1. Calls `vfs_open()`

### Enter the virtual filesystem

[`vfs_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1070)

1. Uses [`struct dentry`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/dcache.h#L82) in [`struct path`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/path.h#L8)
2. dentry: cache directory entry items: inode+path

### Calling into module handler

[call to fops_get](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L922)

[call to fops->open()](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L945)

### Putting the file in the table

[`fd_install()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L631)

### demo

`sudo bpftrace -e 'k:do_dentry_open /comm=="kdlp_hello"/ { printf("%s\n", kstack); }'`

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <linux/openat2.h>
#include <err.h>

int main(void)
{
	struct open_how how = {
		.flags = O_RDONLY,
		.mode = 0,
		.resolve = 0,
	};
	int fd = syscall(SYS_openat2, AT_FDCWD, "hello.txt", &how, sizeof how);
	if (fd < 0)
		err(1, "openat2 failed");

	close(fd);
	return 0;
}
```

### Summary

* Open resolves a path into an inode

* An open file descriptor refers to a `struct file` in the `current->files` structure

* The newer `openat2` provides useful symlink resolution options

* Everything is a file descriptor in Linux, and open is the first step
