# VFS: ioctl and lseek syscalls

---

# Learning objective

Revisit familiar patterns and round out our understanding of file descriptors and the VFS

---

# Overview

1. `ioctl(2)`

    1. Background and history

    1. Entry point and codepath

    1. Common IOCTLS to all file descriptors

1. `lseek(2)`

    1. History and offset extension

    1. Entry point and codepath

---

# IOCTL

1. Commonly pronounced "eye-ock-toll"

1. Abbreviation: Input/Output Control

1. General purpose interface

---

# Origins

1. Introduced in Unix version 7

    1. 1960s-70s

1. Operations beyond read/write

1. Became standard device-specific communication method

1. Replaced (now unimplemented [stty](https://man7.org/linux/man-pages/man2/stty.2.html) & [gtty](https://man7.org/linux/man-pages/man2/gtty.2.html)

---

# Standardization

1. Included in POSIX.1-2001

1. Widely used in Linux and friends

1. Compare to `DeviceIoControl()` in Win32

---

# An unusual interface

`int ioctl(int fildes, int request, ... /* arg */);`

1. Variable number of arguments!

1. From [current standard](https://pubs.opengroup.org/onlinepubs/9699919799/functions/ioctl.html)

1. "For non-STREAMS devices, the functions performed by this call are unspecified"

    1. [STREAMS](https://pubs.opengroup.org/onlinepubs/009695399/basedefs/stropts.h.html) is an obsolete character device protocol

1. "The `ioctl()` function may be removed in a future version."

---

# In Linux

1. [`man 2 ioctl`](https://man7.org/linux/man-pages/man2/ioctl.2.html)

1. [in glibc](https://elixir.bootlin.com/glibc/glibc-2.40/source/sysdeps/unix/sysv/linux/ioctl.c)

```c
int
__ioctl (int fd, unsigned long int request, ...)
```

1. Relies on [crazy macros](https://elixir.bootlin.com/glibc/glibc-2.40/source/sysdeps/unix/sysdep.h#L51)

1. Notice that args after `arg` are ignored

---

# Entering the kernel

#### [`SYSCALL_DEFINE3(ioctl,...)`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L892)

1. `unsigned long int` from userspace implicitly converted to `unsigned int`

1. `unsigned long arg` can be used to hold pointer

1. No `ksys_ioctl()` here!

    1. Used to [exist](https://github.com/torvalds/linux/commit/cbb60b924b9f3e4d7c67a1c9dcf981718f926e4e) but was [removed years ago](https://github.com/torvalds/linux/commit/863b67e15177a7cd0c27b3e36e42fe7907dec9bd)

---

# Overview

#### [`SYSCALL_DEFINE3(ioctl,...)`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L892)

1. Validate and take reference to file

1. Check security modules to validate operation

1. Perform underlying IOCTL

1. Release the file reference

---

# `fdget()` covered elsewhere

See the [slides on read](/slides/fd3.html)

1. This check make sure `fd` is valid

---

# Security check

#### [`security_file_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L2906)

1. Similar to `file_permisison` hook covered in [write slides](/slides/write.html#13)

1. Checks depend on `cmd`

1. Example in [selinux](https://elixir.bootlin.com/linux/v6.13/source/security/selinux/hooks.c#L3697)

1. Not present in [apparmor](https://elixir.bootlin.com/linux/v6.13/source/security/apparmor/lsm.c#L1432)

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

1. Common to any file descriptor

1. Not specific to any filesystem or device

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIOCLEX` and `FIONCLEX`: Set or clear the "close-on-exec" flag

1. Can also do this with `fcntl(2)` and `open(2)` with the `O_CLOEXEC` flag

1. Close `fd` if `current` succeeds at `execve(2)`

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIONBIO`: Uses [`ioctl_fionbio()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L341) to set or clear the nonblocking IO flag

1. Note single `cmd` here

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIOASYNC`: Uses [`ioctl_fioasync()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L364) to enable or disable asynchronous IO notifications

1. Note `-ENOTTY` means this IOCTL doesn't apply to this `fd`

1. Makes sense: `f_op->fasync()` must be defined

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIOQSIZE`: get file's size

1. Works for directories and links, not just regular files

1. A directory's size is the sum of all entries

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIFREEZE` and `FITHAW`: freeze or thaw a filesystem

1. Useful for snapshotting and backups

1. Interaction with write covered [write slides](/slides/write.html#24)

1. Uses [`ioctl_fsfreeze()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L386) and [`ioctl_fsthaw()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L403)

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FS_IOC_FIEMAP`: Get the physical layout of a file on disk

1. Useful for optimization and defragmentation

1. See [`ioctl_fiemap()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L200) for more info

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIGETBSZ`: get the block size of a filesystem

1. Check the superblock of this inode

1. Not always relevant

1. A simple operation

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FICLONE`, `FICLONERANGE`, and `FIDEDUPERANGE`: Copy-on-write file cloning

1. First can clone a whole file ([`ioctl_file_clone()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L231))

1. Second can clone part of a file ([`ioctl_file_clone_range()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L251))

1. Third can de-duplicate data across multiple files ([`ioctl_file_dedupe_range()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L416))

---

# demo

A simple `cp` implementation in `ioctl_copy.c`

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FIONREAD`: How many bytes left to read in a file?

1. This is one place IOCTL may call into a filesystem and/or module

1. For regular file, this is simple subtraction

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FS_IOC_GETFLAGS` and `FS_IOC_SETFLAGS`: Set and get file flags

1. Different than those that can be set with `open(2)` or `fcntl(2)`

1. Many are persistent beyond this `fd`

1. E.g. `FS_APPEND_FL` make a file append-only

1. E.g. `FS_IMMUTABLE_FL` make a file immutable

1. Uses [`ioctl_getflags`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L704) and [`ioctl_setflags`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L715)

---

# First, the common

#### [`do_vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L802)

`FS_IOC_FSGETXATTR` and `FS_IOC_FSSETXATTR`: Get and set extended filesystem-level attributes

1. Multiple uses, including SELinux labels

1. Stored separately from main file information

1. Related to, but different than `xattrs`

---

# Regular files

#### [`file_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L885)

A couple of commands only relevant for regular files, including:

1. Mapping logical to physical block numbers

1. Allocate uninitialized space for a file

1. Deallocate the physical space for a file

1. Zero out a file range

---

# Next, the specific implementation

#### [`vfs_ioctl()`](https://elixir.bootlin.com/linux/v6.13/source/fs/ioctl.c#L44)

1. Call `f_op->unlocked_ioctl()` if it exists

1. Unlocked == no global kernel lock taken

1. Global kernel lock long removed, so no other option

*This concludes `ioctl(2)`*

---

# LSEEK

1. Short for "long seek"

1. Change offset of an open file

1. Implies an historical non-long seek

---

# History of lseek

1. In the beginning (~1970), there was `seek()`

1. Used signed, 16-bit offset

1. Very limited!

1. 2^15 bytes per file

---

# History of lseek

1. `lseek()` was introduced to expand computer potential

1. Now, the offset was a signed 32-bit integer

1. Files could be an entire 2GB!

1. POSIX standardized `lseek()` but not `seek()`

1. Therefore, `seek()` found the dustbin of history

---

# Current standard

"...off_t shall be [a] signed integer [type]" -- [POSIX](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html)

1. `off_t` => `__kernel_off_t` in [`<linux/types.h>`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/types.h#L26)

1. `__kernel_off_t` => `__kernel_long_t` in [`<asm-generic/posix_types.h>`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/posix_types.h#L87)

1. Finally: `__kernel_long_t` => `long` in the [same file](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/posix_types.h#L15), per POSIX

---

# Longer offsets

1. An `loff_t`, however, is a `long long` (64-bit)

1. On 64-bit systems, the `long` type is 64-bits

1. 2^63 = 17,179,869,184/2 gigabytes

1. This should be enough for [all humans](https://lwn.net/Articles/80696/)

---

# Back to the code

#### [`SYSCALL_DEFINE3(lseek,...)`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L403)

1. Another `ksys_*` instance

1. Used by the 32-bit compatibility entry point too

---

# A familiar pattern

#### [`ksys_lseek()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L386)

1. Get a valid reference to the file descriptor or exit

1. Make sure the `whence` is within range

    1. This value modifies the file offset

1. Perform the operation

1. Check for errors (downcast and upcast)

1. Release the reference and return

---

# The long becomes longer

#### [`vfs_llseek()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L378)

1. Bail if this is a pipe, socket, or FIFO

1. [`ESPIPE`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/errno-base.h#L33) is a specific error for seeking on a pipe

1. If all goes well, call into the filesystem or module

1. `f_op->llseek`: long long seek (64-bit)

    1. Fallback implementation [`default_llseek()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L325)


*This concludes `lseek(2)`*

---

# Summary

Many system calls have a varied and interesting history that explains many of their quirks

---

# Summary

`ioctl(2)` provides a versatile way to implement all sorts of interfaces to a kernel modules

---

# Summary

Though quite a simple syscall, understanding `lseek(2)` provides insight into Linux, Unix, and computer history.

---

# Summary

After seeing six syscall implementations, many common patterns should become apparent

---

# Summary

This code is being actively worked on upstream. Contribute!

---

# End
