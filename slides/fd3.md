# file descriptors: read syscall

---

# Learning objective

Gain greater depth of understanding file descriptors by seeing how read uses them

---

# Overview

1. `read(2)` entry

1. Advanced reference count optimization

1. Reading through the virtual filesystem

---

# Entry point

#### [`SYSCALL_DEFINE3(read, ...)`]( https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L621)

1. Just calls `ksys_read()`

1. Only one other caller in [s390 compat code](https://elixir.bootlin.com/linux/v6.5/source/arch/s390/kernel/compat_linux.c#L230)

1. [Originally](https://github.com/torvalds/linux/commit/e7a3e8b2edf54) there were more callers

---

# Callable from userspace and the kernel

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L60)

1. Obtain a reference to the file position or bail

1. Create a local copy

1. Perform virtual filesystem (vfs) read

1. If needed, update the file position

1. Drop any held reference 

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L72)

What is this `struct fd` and why might we want something more than just the `struct file`?

* We don't always need to have our own reference to the struct file

* We need to keep track of which references need to be dropped

---

# Optimizing the references

#### [`fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L72)

1. Get an `unsigned long`

1. Split it into a `struct fd`

```c
static inline struct fd __to_fd(unsigned long v)
{
	return (struct fd){(struct file *)(v & ~3),v & 3};
}
```

---

# Optimizing the references

#### [`__fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1055)

1. First, do we need the file lock?

1. Then, do we need the file position lock?

---

# Optmizing the references

#### [`__fdget()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1028)

Get a reference to a file descriptor unless it's opened in path mode

---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1002)

1. If the refcount is 1, we can borrow it

1. Othewrise, we need our own reference

    1. And we will need to free it later

---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1002)

1. Use  [`atomic_load_and_acquire()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L47) to get the current reference count

1. Call [`files_lookup_fd_raw()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fdtable.h#L83) directly

1. The `unsinged long` return value will be cast
    

---

# Many layers surrounding increment

1. [`__fget()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L920)

    1. dunder since we already mask `FMODE_PATH`

1. [`__fget_files()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L908)

1. [`fget_files_rcu()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L854)

1. [`get_file_rcu()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L1014)


---

# Get what's needed

#### [`__fget_light()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1002)

In the case we cannot borrow, mark the lower bits of the pointer

---

# Optimizing the references

#### [`__fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1055)

The following line should be more clear:

```c
struct file *file = (struct file *)(v & ~3);
```

---

# Check if we need the fpos lock

#### [`file_needs_f_pos_lock()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1049)

When do we need the file position lock?

---

# It is standardized

Any regular file or directory has `FMODE_POS_ATOMIC` set

1. [in `do_dentry_open()`](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L893)

1. [POSIX.1-2017 2.9.7](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html)

In addition, we check the file_count and for a shared iterator

---

# Optimizing the references

#### [`__fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1055)

To finish up, lock and set another bit if needed

1. The return value is split into pointer and flags in `__to_fd()`

---

# A note on CLASS

Not used much yet, but may be soon

1. [`DEFINE_CLASS(fd,...)`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L84)

1. [`#define DEFINE_CLASS(...)`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L84)

---

# Back where we came from

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L60)

First check whether the file is open with `f.file`

1. Maybe soon to become `fd_empty()` and `fd_file()`

1. Recent [patchset](https://lore.kernel.org/all/20240730051625.14349-2-viro@kernel.org/) by maintainer

---

# No position in a stream

#### [`file_ppos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L597)

Otherwise, this just gets the address of the file position

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

Overview:

1. Validate the operation and its inputs

1. Execute the specifc read handler

1. Notify of completion

---


# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

First three checks

1. Make sure the file is open for reading

1. Make sure that the file can be read

1. Make sure the output buffer is a sane address

---

# Check the area to read from

#### [`rw_verify_area()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L355)

1. Sanity check the file position

    1. Signed offsets may wrap or exceed bounds

1. Verify read access

    1. [`security_file_permission()`](https://elixir.bootlin.com/linux/v6.5/source/security/security.c#L2542)

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

Check that count isn't too big

1. count >= [`MAX_RW_COUNT`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L2314)

1. Ensures maximum value is rounded down to page bondary

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

Call the actual read!

1. Call the `read()` member of file operations

1. Otherwise, call `read_iter()`

---

# The meat of the read

#### [`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

If we are successful:

1. Tell fsnotify to let others know of this access

1. Account for task's bytes written

Unconditionally:

1. Account for the task's read syscall

See [`struct task_io_accounting`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/task_io_accounting.h#L12)

---

# Back where we came from

#### [`ksys_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L60)

Last steps to wrap up

1. Update the file position if relevant

1. Drop any references we may have

1. Return the number of bytes read or an error

---

# Drop any references we may have

### [`fdput_pos()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L77)

1. If we locked the file position: [`__f_unlock_pos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1067)

1. If we locked the file: [`fdput()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L43) calls `fput()`

---

# Summary

Read doens't need to do as much as open or write

---

# Summary

Small optimizations on file descriptor operations add up to significant performance improvements

---

# Summary

Watch out for data storage in unexpected places like the lower bits of a pointer!

---

# End
