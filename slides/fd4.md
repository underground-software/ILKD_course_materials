# file descriptors: write syscall

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


# In the begining

#### [`SYSCALL_DEFINE(write,...)`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L646)

1. All it does is `ksys_write()`

1. Only one other caller in [s390 compat code](https://elixir.bootlin.com/linux/v6.5/source/arch/s390/kernel/compat_linux.c#L230)

1. [Originally](https://github.com/torvalds/linux/commit/e7a3e8b2edf54) there were more callers


---

# Where did these callers go?

While file descriptors are prefered as a userspace interface, the kernel is better off working directly with `struct file`s

[`ksys_write()` removed from init/initramfs.c](https://github.com/torvalds/linux/commit/bf6419e4d5440c6d414a320506c5488857a5b001)

[`ksys_write()` removed from init/do_mounts_rd.c](https://github.com/torvalds/linux/commit/bef173299613404f55b11180d9a865861637f31d)

1. Notice that `ksys_lseek()` is entirely removed

---

# The other kernel interface

#### [`kernel_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L548)

1. Verify the write operation

1. Acquire a filesystem resource

1. Perform the underlying operation

1. Release the filesystem resource

Almost a  simplified `vfs_write()`

---

# Callable from userspace and the kernel

####  [`ksys_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L626)

1. Obtain a reference to the file position or bail

1. Create a local copy of the file position

1. Perform virtual filesystem (vfs)

1. If needed, update the file position

1. Drop any held reference 

---

# Spot the difference

####  [`ksys_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L626)

How does the function differ from `ksys_read()`?

* `vfs_write()` instead of `vfs_read()`

* `const char __user * buf` instead of `char __user * buf`


---

# Keeping these slides DRY

1. DRY: "Don't Repeat Yourself"

1. See the [slides on read](/slides/fd3.html)

1. We will skip right to `vfs_write()`

---

# Right into the meat

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

1. Verify and validate the operation

1. Acquire filesystem resources

1. Perform the write operation

1. Account for the operation

1. Release filesystem resources

---

# First validation

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

1. Make sure file open for writing (`FMODE_WRITE`)

1. Make sure writing makes sense (`FMODE_CAN_WRITE`)

1. Make sure `buf` is a userspace address range


---

# Verifying the target

#### [`rw_verify_area`](https://elixir.bootlin.com/linux/v6.5/C/ident/rw_verify_area)

1. Disallow count values with top bit set

1. Sanity check the file position

    1. Signed offsets may wrap or exceed bounds

1. Verify write access


---

# Security checks

##### [`security_file_permission()`](https://elixir.bootlin.com/linux/v6.5/source/security/security.c#L2542)

1. Use [`MAY_WRITE`](`https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L96) as our mask

1. Call an arbitrary number of `file_permission` security hooks

1. If permission is granted, set off notifications

---

# The hook caller

#### [`call_int_hook()`](https://elixir.bootlin.com/linux/v6.5/source/security/security.c#L773)

1. IRC: initial return code if all hooks reutrn 0

1. Call each hook at stop if one fails

1. Statement expression evaluates to return code

---

# hlist detour

#### [`hlist_for_each_entry()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/list.h#L1053)

"Hash List"

1. Head contains pointer to only first node

1. Regular list head has first & last pointers

1. Useful when list is frequently empty, like hash list buckets

---

# LSM XARGS

#### [`struct security_hook_heads`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/lsm_hooks.h#L38)

1. Define a macro in particular way

1. Resolve [many instances](https://elixir.bootlin.com/linux/v6.5/source/include/linux/lsm_hook_defs.h) of this macro

1. Undefine the macro to allow later re-use

[`LSM_HOOK(..., file_permission, ...)`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/lsm_hook_defs.h#L168)

---

# Example `file_permission` hooks

#### [`selinux_file_permission()`](https://elixir.bootlin.com/linux/v6.5/source/security/selinux/hooks.c#L3563)

1. Security Enhanced Linux: Fine-grained mandatory access control (MAC)

1. Associated with `file_permission` hook [here](https://elixir.bootlin.com/linux/v6.5/source/security/selinux/hooks.c#L7019)

1. Registered with security subsystem by [`security_add_hooks()`](https://elixir.bootlin.com/linux/v6.5/source/security/selinux/hooks.c#L7253)

1. Quick demo: `ls -lZ`


---

# Example `file_permission` hooks


### [`apparmor_file_permission()`](https://elixir.bootlin.com/linux/v6.5/source/security/apparmor/lsm.c#L522)

1. AppArmor: Per-program security profiles

1. Associated with `file_permission` hook [here](https://elixir.bootlin.com/linux/v6.5/source/security/apparmor/lsm.c#L1242)

1. Registered with security subsystem by [`security_add_hooks()`](https://elixir.bootlin.com/linux/v6.5/source/security/apparmor/lsm.c#L1906)

---

# More information about LSM

[Upstream documentation](https://docs.kernel.org/admin-guide/LSM/index.html)

---

# Notify of permission granted

#### [`fsnotify_perm()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fsnotify.h#L105)

1. Called with `MAY_WRITE`

```c
	if (!(mask & (MAY_READ | MAY_OPEN)))
		return 0;
```

Therefore, this is a no-op

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

One last check:

1. count >= [`MAX_RW_COUNT`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L578)

1. Ensures maximum value is rounded down to page bondary

1. Exactly the same as read

---

# Acquire filesystem resources

#### [`file_start_write()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L2548)

1. Check whether this is a regular file

1. A regular file is 0 or more bytes on disk

1. Not regular: character devices, directories, links

1. [`S_ISREG()`](https://elixir.bootlin.com/linux/v6.5/source/include/uapi/linux/stat.h#L22)

---

# Acquire filesystem resources

#### [`sb_start_write()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L1567)

1. Acquire superblock write access

1. Each filesystem has one superblock

1. Contains meta-information about filesystem

1. Only relevant for regular files

---

# Don't freeze me!

##### [`SB_FREEZE_WRITE`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L1140) and [`struct super_block`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L1154)

1. Freezing enables snapshot fs backups

1. Select from an array of percpu reader-writer locks

1. Read is CPU local, write is cross-core

---

# demo

Freezing a filesystem

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

Now we can actually write!

1. `f_op->write()` calls into the filesystem or module

1. Like read, fallback to `f_op->write_iter`

1. We should never hit the `-EINVAL` case if `FMODE_CAN_WRITE` is set

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

When we write some bytes:

1. Notify of file modification

1. Account for bytes written by this task

---

# Back to the VFS

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

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

Linux Security Modules (LSM) provies a flexible way to enforce sets of security policies at the kernel level

---

# Summary

Memory footprint minimization in the kernel is critical and this justifies `hlist`, which saves one pointer in the head instead of two

---

# Summary

Kernel internal use of system call functionality is still evolving

---

# End

