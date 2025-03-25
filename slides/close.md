# VFS: close syscall

---

# Learning objective

Gain a broad overview of many aspects of the kernel by understanding what's necessary to close a file descriptor

---

# Overview

1. Peel back the layers of `close(2)`

1. Removing entries of the FDT

1. Scheduling work to be done later

1. Execution context design considerations

1. Several more concurrency techniques

1. Execution context sentitive code

---

# What does `close(2)` do?

* Can close fail?

---

# Two main tasks

1. Invalidate `int fd` index in FDT

1. Close the `struct file *` if needed

---


# `close(2)` from start to finish

---


# demo

Verify with `strace` that `close(3)` indeed calls `close(2)`

---

# Entry point

#### [`SYSCALL_DEFINE1(close)`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1539)

1. cannot restart syscall since `struct file` is gone

1. If the file fails to be closed, the data may be hosed

---

# First layer

#### [`file_close_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L690)

1. Use `int fd` arugment to index into FDT

1. Obtain underlying `struct file`

1. What benefit could is there to using `spin_lock` here?

    * Can call in atomic context

---

# Safe indexing

#### [`file_close_fd_locked()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L671)

1. Index into FDT properly

1. Do bounds checking on input value

1. Use [array_index_nospec()](https://elixir.bootlin.com/linux/v6.13/source/include/linux/nospec.h#L55) macro for security

1. Use RCU to safely `NULL`ify FDT entry without locks

1. Concurrent readers of the FDT will see a value that makes sense

---

# Mitigating speculative execution attacks

#### `array_index_mask_nospec()` [(generic)](https://elixir.bootlin.com/linux/v6.13/source/include/linux/nospec.h#L28) [(arm64)](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/include/asm/barrier.h#L87) [(x86)](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/include/asm/barrier.h#L36)

1. Create bitmask based on index

1. All `1`s if within bounds, else `0`

1. Bitwise AND index to zero if out-of-bounds

1. Speculative indexing into the array always within bounds

---

# Returning the file descriptor

#### [`__put_unused_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L597)

1. Why don't we call `put_unused_fd()`?

* We already hold the `files->file_lock` spinlock

---

# Returning the file descriptor

#### [`__clear_open_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L322)

1. Update bitmaps holding open file info

1. full and low resolution maps used

1. BITS_PER_LONG-sized ranges checked if all fds in use

---

# Returning the file descriptor

#### [`__put_unused_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L597)

1. Smallest available `fd` stored for next `open(2)`

1. This free may require updating smallest `fd`


---

# Finish removing the FDT entry

#### [`file_close_fd_locked()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L671)

1. Return the `struct file` associated with the open `fd`

1. Return `NULL` if `fd` not open

---

# Return to the first layer

#### [`close_fd()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file.c#L690)

1. No file? Then `-EBADF`

1. Lastly, return whatever `filp_close()` returns

---

# Inverting `filp_open()`

#### [`filp_close()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1523)

1. sanity check reference count

1. Never should be `0`

1. Use [`CHECK_DATA_CORRUPTION()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/bug.h#L81)
    macro which may call `BUG()` on kernels configured to do so

---

# Another way to crash

#### [`BUG()`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/bug.h#L74)

1. [`ASM_BUG_FLAGS()`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/bug.h#L53) generates assembly from preprocessor macros

1. Why use high numbers in assembly labels?

* high numbers in assembly are to avoid collisions 

---

# Inverting `filp_open()`

#### [`filp_close()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1523)

1. If implemented call the `->flush()` file operation

1. Flush performs pre-closure cleanup

1. Example: writing buffered data to storage medium

---

# Path mode

1. Can `open(2)` a file with `O_PATH`

1. Lighweight efernce to filesystem path entry

1. No I/O

1. Example usage: permission checks, change of ownership

---

# Inverting `filp_open()`

#### [`filp_close()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1523)

For files with I/O context

1. Flush directory notifications using the `dnotify` system

1. Remove POSIX locks associated with this file

---

# dnotify: history

1. First Linux filesystem event notificaton system

1. Added in 2001 in Linux 2.4.0

1. Monitor CRUD chagnes in directory

1. Notifed via `SIGIO` usually

---

# dnotify: problems

1. Only directory granularity

1. Signal handling can be tricky

1. Need open fd

1. not much info about events

---

# dnotify: obsolete

1. No longer used

1. Kept for legacy reasons

1. Replacement: `inotify`

---

# POSIX locks

1. Can lock range of a file with `fcntl(2)`

---

# demo

example posix locks program

---

# Inverting `filp_open()`

#### [`filp_close()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L1523)

1. Call `fput()` to finish the job

1. No error code from `fput()`

1. Return value nonzero only when `flush` fails

---

# Optimal procrastination

#### [`fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L499)

Decrement the file's reference count (`file->f_count`)

1. Use [atomic_long_dec_and_test()](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L4536)

1. No other action taken when result is nonzero

1. If count reaches zero, instigate the real work

---

# Optimal procrastination

#### [`fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L499)

Why rush? Schedule a future callback

1. First method: only for process context

1. Second method: for any context

---

# Checking our execution context: interface

#### [`in_interrupt()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/preempt.h#L143)

A depreceated macro

1. Transitively defined by `irq_count()`

1. Bitwise OR three shifted values

1. NMI, softirq, and hardirq counts

1. Nonzero when any count is nonzero

---

# Checking our execution context: backend

#### [`preempt_count()`](https://elixir.bootlin.com/linux/v6.13/source/include/asm-generic/preempt.h#L9)

Architecture-specific data source

1. Value stored in `current->thread_info`

1. Can directly cast `current` since `struct thread_info` is first member

1. `READ_ONCE()` prevents racy compiler re-ordering 

---

# Kernel threads

1. Process context without userspace

1. Can sleep, be preempted

1. Can call most kernel funtions

1. No userspace memory to access

---

# Note on `likely()` macro

1. Generates branch prediction hints 

1. Not on all CPUs

1. `unlikely()` does the inverse

---

# Why be careful with `likely()`?

* Faster true case 

* Slower false case

* Helpful only when very likely true

* Otherwise considered harmful

---

# In the likely case of process context

Schedule callback to run on `current`'s behalf

---

# Task work

1. [`init_task_work()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/task_work.h#L11) wraps callback struct member assignment

1. [`task_work_add()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/task_work.c#L54) schedules the work

    1. Our callback is to [`____fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L476)

1. If this fails, just fallback to the other method

---

# Scheduling options

1. `TWA_SIGNAL` interrupts target task

1. `TWA_SIGNAL_NO_IPI` is more chill

    1. Signal, but don't interrupt

1. `TWA_RESUME` is the most relaxed

    1. Just wait for next kernel exit by target task

---

# The userspaceless case

1. Global delayed work queue

1. Create a list of files to pass to callback

1. Run them all in a jiffy (next timer tick)

---

# Global procrastination

#### [`fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L499)

Use [`schedule_delayed_work()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/workqueue.h#L814) to access global queue

1. Uses structure defined with [`DECLARE_DELAYED_WORK()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/workqueue.h#L253)

1. Do work after `delay` timer ticks pass

---

# Global procrastination

#### [`fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L499)

Avoid any extra scheduling

1. Conditionally call `schedule_delayed_work()`

1. Only on first list append

1. Resulting work will empty this queue

1. [`llist`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/llist.h): lockless linked list implementation optimized for concurrent access

---

# First callback
    
Two possible callers of [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

1. `____fput()` when using task work

1. `delayed_fput` when using global delayed work

---

# From task work

#### [`____fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L476)

1. Uses the [`container_of()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/container_of.h#L18) macro 

1. Use struct member offset subtraction 

1. Pass containing `struct file` to `__fput()`

---

# From global delayed work

#### [`delayed_fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L467)

1. Detach list of files from caller handle

1. Use special `llist` iterator

1. Pass each file to `__fput()`

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

What do we need to do?

1. Clean up file-associated resoruces 

1. Drop references held by file

1. Free allocated memory

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Is the file really open?

1. Check `FMODE_OPENED` flag  in `file->f_mode`,

1. Set by [`do_dentry_open()`](https://elixir.bootlin.com/linux/v6.13/source/fs/open.c#L949) in `open(2)` path

1. Without this flag, skip to memory freeing

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

A debugging helper: `might_sleep()`

1. Dump stack trace when called in atomic context

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Spead the news of this closure

1. `fsnotify` provides fs event info to other kernel systems

1. e.g. `inotify` consumes this data

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Call [`eventpoll_release()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/eventpoll.h#L34) to clean up all resoruces associated with event polling on this `struct file`


---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Safe to release the file's locks: [`locks_remove_file()`](https://elixir.bootlin.com/linux/v6.13/source/fs/locks.c#L2685)

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)


Integrity Management Architecture (IMA)

1. Prevents tampering with file contens

1. Allocates resources for each file

1. Cleanup with [`ima_file_free()`](https://elixir.bootlin.com/linux/v6.13/source/security/integrity/ima/ima_main.c#L193)

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Handle pending asynchronous operations

1. Only if file has `FASYNC` flag set

1. Call `fasync()` handler defined by underlying file implementation

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Call any extant `release()` file operation

1. A module implemeting a character device may define this handler

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Release reference to a character device and file operations

1. Only if file is backed by one

1. Reference to any underlying module implements fops reference
---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Drop reference to `pid` of file owner

1. Contained in `struct pid` in `struct fown_struct` 

1. Which is a member of `struct file`

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Use `put_file_access()` to perform access mode specific tasks to clean up access to the file

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Drop a reference to the `dentry` for this file with `dput()`

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Some file modes will require an unmount at this point

1. Handled by `dissolve_on_fput()`

1. May cover later material on namespaces

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

`mntput()` frees the `struct file`'s [`struct vfsmount`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/mount.h#L68) member

1. represents an abstraction of a mounted filesystem

---

# The "real guts of fput()"

#### [`__fput()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L424)

Finish the job with `file_free()`

1. Called even when file wasn't open


---

# The final function

#### [`file_free()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L66)

Notify Linux Security Modules (LSM) framework users to cleanup with [`security_file_free()`](https://elixir.bootlin.com/linux/v6.13/source/security/security.c#L2880) to clean up

---

# The final function

#### [`file_free()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L66)

Decrement open file counter

1. Directly decrement local percpu counter

1. Global total periodically calculated

---

# The final function

#### [`file_free()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L66)

Drop refrence to file's [`struct cred`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/cred.h#L111)

1. Stores the security context information task that opened the file


---

# The final function

#### [`file_free()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L66)

If the file is a backing store for a device or file, drop reference to associated `struct path`

1. Example: a loopback device

1. Last step before freeing memory

---


# The final function

#### [`file_free()`](https://elixir.bootlin.com/linux/v6.13/source/fs/file_table.c#L66)

Free the last structure's memory

1. Backing files free their backing file structure

1. Otherwise, return the `struct file` to its `kmem_cache()`

1. Back to whence it came

---

# That's it

We return to userspace, concluding the `close(2)` implementation

---

# Summary

The `close(2)` systemcall contains plenty of complexity and many layers

---

# Summary

Many different types of in-kernel resources may be associated with a file

---

# Summary

The kernel employs creative lock avoidant techniques to implement correct concurrency

---

# Summary

Correct reference counting is essential

---

# Summary

The codepath can invoke several file operations, including `release()`, `flush()`, and `fasync()`

---

# End

