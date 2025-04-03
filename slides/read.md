# VFS: read syscall

---

# Learning objective

Gain greater depth of understanding of file descriptors by seeing how read uses them

---

# Overview

1. `read(2)` entry

1. Advanced reference count optimization

1. Reading through the virtual filesystem

---

# Entry point

#### [`SYSCALL_DEFINE3(read, ...)`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L715)

1. Just calls `ksys_read()`

1. Only one other caller in [s390 compat code](https://elixir.bootlin.com/linux/v6.13/source/arch/s390/kernel/compat_linux.c#L230)

1. [Originally](https://github.com/torvalds/linux/commit/e7a3e8b2edf54) there were more callers

---

# Callable from userspace and the kernel

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L697)

1. Obtain a reference to the file position or bail

1. Create a local copy of the file position

1. Perform virtual filesystem (vfs) read

1. If needed, update the file position

1. Drop any held references

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1185)

* Invoked via `CLASS(...` referencing [`DEFINE_CLASS(fd_pos,...)`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/file.h#L85)

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1185)

What is this `struct fd` and why might we want something more than just the `struct file`?

* We don't always need to have our own reference to the `struct file`

* We need to keep track of which references need to be dropped

---

# Optimizing the references

#### [`fd_file()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/file.h#L44)

1. Get an `unsigned long`

1. Split it into a `struct fd`

```c
#define fd_file(f) ((struct file *)((f).word & ~(FDPUT_FPUT|FDPUT_POS_UNLOCK)))
```

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1185)

1. First, do we need the file lock?

1. Then, do we need the file position lock?

---

# Optimizing the references

#### [`fdget()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1158)

Get a reference to a file descriptor unless it's opened in path mode

---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1132)

1. If the refcount is 1, we can borrow it

1. Otherwise, we need our own reference

    1. And we will need to free it later

---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1132)

1. Use  [`atomic_read_acquire()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L47) to get the current reference count

1. Call [`files_lookup_fd_raw()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fdtable.h#L72) directly

1. The `unsigned long` return value will be cast

---

# Layers surrounding increment

1. [`__fget_files()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1044)

1. [`__fget_files_rcu()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L968)


---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1132)

In the case we cannot borrow, mark the lower bits of the pointer

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1185)

Question: what is `fd_file()` doing?

---

# Check if we need the fpos lock

#### [`file_needs_f_pos_lock()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1179)

When do we need the file position lock?

---

# It is standardized

Any regular file or directory has `FMODE_POS_ATOMIC` set

1. [in `do_dentry_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L919)

1. [POSIX.1-2017 2.9.7](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html)

In addition, we check the file_count and for a shared iterator

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1185)

To finish up, lock and [set another bit](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1191) if needed


---

# Back where we came from

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L697)

First check whether the file is open with `fd_empty()`

1. Recent [patchset](https://lore.kernel.org/all/20240730051625.14349-2-viro@kernel.org/) by maintainer

1. Introduced `fd_empty()` and `fd_file()`

1. Used to check `f.file`

---

# No position in a stream

#### [`file_ppos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L692)

Otherwise, this just gets the address of the file position

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L545)

Overview:

1. Validate the operation and its inputs

1. Execute the specific read handler

1. Notify of completion

---


# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L545)

First three checks

1. Make sure the file is open for reading

1. Make sure that the file can be read

1. Make sure the output buffer is a sane address

---

# Check the area to read from

#### [`rw_verify_area()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L444)

1. Sanity check the file position

    1. Signed offsets may wrap or exceed bounds

1. Verify read access

    1. [`security_file_permission()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L2837)

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L545)

Check that count isn't too big

1. count >= [`MAX_RW_COUNT`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L2704)

1. Ensures maximum value is rounded down to page boundary

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L545)

Call the actual read!

1. Call the `read()` member of file operations

1. Otherwise, call `read_iter()`

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L545)

If we are successful:

1. Tell fsnotify to let others know of this access

1. Account for task's bytes written

Unconditionally:

1. Account for the task's read system call

See [`struct task_io_accounting`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/task_io_accounting.h#L12)

---

# Back where we came from

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.13/source/fs/read_write.c#L697)

Last steps to wrap up

1. Update the file position if relevant

1. Drop any references we may have

1. Return the number of bytes read or an error

---

# Drop any references we may have

### [`fdput_pos()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/file.h#L76)

Question: When does control flow call this function?

* When we reach the end of the scope where `CLASS(...` was used

---

# Drop any references we may have

### [`fdput_pos()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/file.h#L76)

1. If we locked the file position: [`__f_unlock_pos()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L1197)

1. If we locked the file: [`fdput()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/file.h#L60) calls `fput()`

---

# Summary

`read()` doesn't need to do as much as `open()` or `write()`

---

# Summary

Small optimizations on file descriptor operations add up to significant performance improvements

---

# Summary

Watch out for data storage in unexpected places like the lower bits of a pointer!

---

# End
