# VFS: write syscall

---

# Learning objective:

Gain greater depth of understanding file descriptors by comparing read and write

---

# Overview

1. Userspace and kernel entry points

1. Contrast with `read(2)`

1. A look at security hooks

1. Superblocks and filesystem snapshotting

---


# In the beginning

#### [`SYSCALL_DEFINE3(write,...)`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L739)

1. All it does is `ksys_write()`

1. Only one other caller in [s390 compat code](https://elixir.bootlin.com/linux/v6.13/source/arch/s390/kernel/compat_linux.c#L238)

1. [Originally](https://github.com/torvalds/linux/commit/e7a3e8b2edf54) there were more callers


---

# Where did these callers go?

While file descriptors are preferred as a userspace interface, the kernel is better off working directly with `struct file`s

[`ksys_write()` removed from init/initramfs.c](https://github.com/torvalds/linux/commit/bf6419e4d5440c6d414a320506c5488857a5b001)

[`ksys_write()` removed from init/do_mounts_rd.c](https://github.com/torvalds/linux/commit/bef173299613404f55b11180d9a865861637f31d)

1. Notice that `ksys_lseek()` is restricted to static linkage

---

# The other kernel interface

#### [`kernel_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L643)

1. Verify the write operation

1. Acquire a filesystem resource

1. Perform the underlying operation

1. Release the filesystem resource

Almost a  simplified `vfs_write()`

---

# Callable from userspace and the kernel

####  [`ksys_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L643)

1. Obtain a reference to the file position or bail

1. Create a local copy of the file position

1. Perform virtual filesystem (VFS) write

1. If needed, update the file position

1. Drop any held references

---

# Spot the difference

####  [`ksys_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L643)

How does the function differ from `ksys_read()`?

* `vfs_write()` instead of `vfs_read()`

* `const char __user * buf` instead of `char __user * buf`


---

# Keeping these slides DRY

1. DRY: "Don't Repeat Yourself"

1. See the [slides on read](/slides/read.html)

1. We will skip right to `vfs_write()`

---

# Right into the meat

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

1. Verify and validate the operation

1. Acquire filesystem resources

1. Perform the write operation

1. Account for the operation

1. Release filesystem resources

---

# First validation

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

1. Make sure file open for writing (`FMODE_WRITE`)

1. Make sure writing makes sense (`FMODE_CAN_WRITE`)

1. Make sure `buf` is a userspace address range

---

# Verifying the target

#### [`rw_verify_area()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L444)

1. Disallow count values with top bit set

1. Sanity check the file position

    1. Signed offsets may wrap or exceed bounds

1. Verify write access

---

# Security checks

##### [`security_file_permission()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L2837)

1. Use [`MAY_WRITE`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L102) as our mask

1. Call an arbitrary number of `file_permission` security hooks

---

# The hook caller

#### [`call_int_hook()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L970)

1. [`__label__`](https://gcc.gnu.org/onlinedocs/gcc/Local-Labels.html) to declare local label.

    1. Why?

1. RC =  [`LSM_RET_DEFAULT(NAME)`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L929) initial return code if all hooks return 0

    1. Where is `file_permission_default` defined?

1. Call each hook and stop if one fails

1. Statement expression evaluates to return code

---

# unroll

#### [`LSM_LOOP_UNROLL()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L47)

1. Recursively defined macro

1. [`#define UNROLL(...`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/unroll.h#L12)

1. Changed from [hlist](https://elixir.bootlin.com/linux/v6.13/source/include/linux/list.h) iteration in Summer 2024 by [`417c5643cd67a`](https://github.com/torvalds/linux/commit/417c5643cd67a)

1. Macro counting done for [`MAX_LSM_COUNT`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/lsm_count.h#L112)

---

# LSM XARGS

#### [`union security_list_options`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/lsm_hooks.h#L38)

1. Define a macro in particular way

1. Resolve [many instances](https://elixir.bootlin.com/linux/v6.13/source/include/linux/lsm_hook_defs.h) of this macro

    1. Specifically [`LSM_HOOK(..., file_permission, ...)`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/lsm_hook_defs.h#L188)

1. Undefine the macro to allow later re-use

    1. This technique is called xmacros

---

# demo

`xmacros_example`

---

# Example `file_permission` hooks

#### [`selinux_file_permission()`](https://elixir.bootlin.com/linux/v6.13/source/security/selinux/hooks.c#L3625)

1. Security Enhanced Linux: Fine-grained mandatory access control (MAC)

1. Associated with `file_permission` hook [here](https://elixir.bootlin.com/linux/v6.13/source/security/selinux/hooks.c#L7211)

1. Registered with security subsystem by [`security_add_hooks()`](https://elixir.bootlin.com/linux/v6.13/source/security/selinux/hooks.c#L7451)

1. Quick demo: `ls -lZ`


---

# Example `file_permission` hooks

### [`apparmor_file_permission()`](https://elixir.bootlin.com/linux/v6.13/source/security/apparmor/lsm.c#L545)

1. AppArmor: Per-program security profiles

1. Associated with `file_permission` hook [here](https://elixir.bootlin.com/linux/v6.13/source/security/apparmor/lsm.c#L1457)

1. Registered with security subsystem by [`security_add_hooks()`](https://elixir.bootlin.com/linux/v6.13/source/security/apparmor/lsm.c#L2250)

---

# More information about LSM

[Upstream documentation](https://docs.kernel.org/admin-guide/LSM/index.html)

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

One last check:

1. count >= [`MAX_RW_COUNT`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L2704)

1. Ensures maximum value is rounded down to page boundary

1. Exactly the same as read

---

# Acquire filesystem resources

#### [`file_start_write()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L2960)

1. Check whether this is a regular file

1. A regular file is 0 or more bytes on disk

1. What are some examples of files that are NOT regular

* Not regular: character devices, directories, links

* [`S_ISREG()`](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/linux/stat.h#L22)

---

# Acquire filesystem resources

#### [`sb_start_write()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L1859)

1. Calls [`__sb_start_write()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L1723)

1. Acquire superblock write access

1. Each filesystem has one superblock

1. Contains meta-information about filesystem

1. Only relevant for regular files

---

# Don't freeze me!

##### [`SB_FREEZE_WRITE`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L1239) and [`struct super_block`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L1255)

1. Freezing enables snapshot fs backups

1. Select from an array of percpu reader-writer locks

1. Read is CPU local, write is cross-core

---

# demo

Freezing a filesystem

`free.c` and `make_loop.sh`

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

Now we can actually write!

1. `f_op->write()` calls into the filesystem or module

1. Like read, fallback to `f_op->write_iter`

1. We should never hit the `-EINVAL` case if `FMODE_CAN_WRITE` is set

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

When we write some bytes:

1. Notify of file modification

1. Account for bytes written by this task

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L659)

Unconditionally:

1. Account for write syscall count by this task

1. Release any filesystem resources acquired earlier

1. Return bytes written or errno to userspace

*This concludes `write(2)`*

---

# Summary

Writing is quite similar to reading, but a bit more complex

---

# Summary

Linux Security Modules (LSM) provides a flexible way to enforce sets of security policies at the kernel level

---

# Summary

Memory footprint minimization in the kernel is critical and this justifies `hlist`, which saves one pointer in the head instead of two

---

# Summary

Kernel internal use of system call functionality is still evolving

---

# End

