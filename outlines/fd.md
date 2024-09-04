# fd: open

[NR_OPEN_DEFAULT](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fdtable.h#L24)

[struct files_struct](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fdtable.h#L49)

[->files in task_struct](https://elixir.bootlin.com/linux/v6.5/source/include/linux/sched.h#L21)


[AT_FDCWD](https://elixir.bootlin.com/linux/v6.5/source/include/uapi/linux/fcntl.h#L96)

[ARCH_32_BIT_OFF_T](https://elixir.bootlin.com/linux/v6.5/source/arch/Kconfig#L348)


[`*at` version preferred](https://elixir.bootlin.com/linux/v6.10/source/include/linux/syscalls.h#L1003)

# demo 

strace ./twinkle_twinkle


[open->openat](https://elixir.bootlin.com/glibc/glibc-2.40.9000/source/sysdeps/unix/sysv/linux/open.c#L31)
[SYSCALL_CANCEL](https://elixir.bootlin.com/glibc/glibc-2.40.9000/source/sysdeps/unix/sysdep.h#L118)

Open:

1. Unless explicitly using an old 32-bit arch that supports 32-bit off_t, use 64-bit off_t

    1. O_LARGEFILE allows filesizes that won't fit in 32 bits

1. Call `do_sys_open` using AT_FDCWD

    1. Open relative to current working directory
    
    1. Same thing as openat, but with dfd set

    1. latter prefered by glibc, semantics are the sme

1. Build a `struct open_how` from `int flags` and `umode_t mode`
[do_sys_open](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1419)

    1. Added in fddb5d430ad9f ("open: introduce openat2(2) syscall")

    1. openat didn't check for unknown `flags`

    1. Extending with new features troublesome

    1. Openat defined in terms of openat2

[`struct open_how`](https://elixir.bootlin.com/linux/v6.5/source/include/uapi/linux/openat2.h#L19)


    1. Previously:
        
        1. Follow symlinks in each component of patch

        1. By default, follow for last component, unless O_NOFOLLOW

        1. O_PATH: another exeception, open path without following

    1. New RESOLVE flags for link resolution

    1. RESOLVE_BENEATH: all resolutions in subtree

    1. RESOLVE_NO_ROOT: 

    1. RESOLVE_NO_SYMLINKS: Like O_NOFOLLOW for whole path resolution process


[build_open_how()](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1191)

[S_IALLUGO](https://elixir.bootlin.com/linux/v6.5/source/include/linux/stat.h#L10)

1. read, write, exec for user, group, and other

    * setuid, setgid, sticky bit

[permission bits](https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html)

[build_open_flags()](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1207)


[call to getname](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1401)

[getname_flags](https://elixir.bootlin.com/linux/v6.5/source/fs/namei.c#L130)

getname: turn a string filename into a `struct filename *`

* copy_from_user for filenames

* allocates memory, validation


[do_filep_open()](https://elixir.bootlin.com/linux/v6.5/source/fs/namei.c#L3812)

[fd_install](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L602)


[SYSCALL_DEFINE0(fork)](https://elixir.bootlin.com/linux/v6.5/source/kernel/fork.c#L2994)

[kernel_clone() calls copy_process()](https://elixir.bootlin.com/linux/v6.5/source/kernel/fork.c#L2912)

[copy_process() calls copy_files()](https://elixir.bootlin.com/linux/v6.5/source/kernel/fork.c#L2492)

[copy_files()](https://elixir.bootlin.com/linux/v6.5/source/kernel/fork.c#L1769)

[dup_fd()](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L316)

[alloc_fdtable()](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L105)


1. Maybe talk about upstream struct fd patchset

### `close(2)`

1. `close(3)` indeed calls `close(2)`

    1. Verify with strace

#### [SYSCALL_DEFINE1(close)](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1534)

various ERESTART* return values translated into EINTR

1. File table entry cleared so can't restart

1. Go into ERESTARTSYS semantics in general

1. Demo: restarting syscall

#### [close_fd()](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L654)

Need to first get the `struct file *file` for `int fd` input from user

1. current->files acceses the thread's FDT

1. take spinlock `current->files_lock` before indexing into FDT

    1. Allows this function to be called in atomic context


#### [pick_file()](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L637)

Index into FDT properly

1. Do bounds checking on input value

1. Use [array_index_nospec()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/nospec.h#L55) macro to ensure index doesn't exceed bounds in case of CPU speculation

    
1. [array_index_mask_nospec arm64](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/include/asm/barrier.h#L83)

1. [array_index_mask_nospec x86](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/include/asm/barrier.h#L36)

    * It creates a mask based on whether the index is within bounds.

    * This mask is all 1s if the index is valid, all 0s if not.

    * The original index is then ANDed with this mask.

    * If the index was in bounds, it remains unchanged.

    * If the index was out of bounds, it becomes zero.

    * When out of bounds, we return anyway

    * Speculative indexing into the array will never read past the end of the buffer

Maybe a diagram in this would be appropriate

1. Then, we can index directly into the FDT

1. Mutate the value in the table using RCU

[rcu_assign_pointer](https://elixir.bootlin.com/linux/v6.5/source/include/linux/rcupdate.h#L494)

* ensures "that any concurrent RCU readers will see any prior initialization"

1. [`__put_unused_fd()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L568)
called instead of dunder version since we already have the file_lock spinlock held
by `close_fd()` which is a precondition to `pick_file()`

    * [`__clear_open_fd()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L268)
    clears bits in memory representing usage of this fg

    * `file->open_fds` contains bit i set iff fd i corresponds to an open `struct file *`

    * `file->full_fds_bits` contains bit i set only if at least one of the `sizeof(long)` number of fds starting from `i * sizeof(long) corresponds to an open `struct file *`

1. If we closed an fd less than the next fd we were going to return to the user,
then the closed fd is now the one we should next return to the user.

1. Implicitly return NULL on failure, otherwise the `struct file *` being closed

Now that `int fd` is freed for future use, we can safely close the underlying `struct file *`. This is the opposite order to what was done in `open(2)`, as one would expect.

1. First check that refcount in `filp->f_count` isn't being decremented beyond 0

1. [`CHECK_DATA_CORRUPTION()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/bug.h#L81)
    macro used, which may call `BUG()` on kernels configured to do so

1. [`BUG()`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/include/asm/bug.h#L17)
on arm64 uses preprocessor generated assembly from a header file in
[`ASM_BUG_FLAGS()`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/include/asm/asm-bug.h#L37)

    * high numbers in assembly are to avoid collisions 

1. If the `struct file operations file->f_op` field contains a `->flush` member, it's called here

1. Certain things don't need be done if the file is opened in path mode, e.g. with open flag `O_PATH` 

    1. Flush directory notifications using the `dnotify` system
        
        1. first filesystem event notificaton system in Linux added in 2001 in Linux 2.4.0

        1. allows a process to recieve notifications, typically `SIGIO` when CRUD changes occured in a directory

        1. Problems: only directory granularity, signals can be tricky, need open fd, not much info about events

        1. obsoleted `inotify` but kept for legacy reasons

    1. Remove POSIX locks associated with this file

Example POSIX locks usage from claude

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define FILE_NAME "example.txt"

int main() {
    int fd;
    struct flock fl = {0};
    char *message = "Hello, POSIX locks!\n";

    // Open the file
    fd = open(FILE_NAME, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    printf("Attempting to lock the file...\n");

    // Prepare the lock structure
    fl.l_type = F_WRLCK;    // Write lock
    fl.l_whence = SEEK_SET; // Lock starts at beginning of file
    fl.l_start = 0;         // Starting offset
    fl.l_len = 0;           // Lock entire file (0 means to EOF)

    // Try to set the lock
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl error");
        close(fd);
        exit(1);
    }

    printf("File locked successfully. Writing data...\n");

    // Write to the file
    if (write(fd, message, strlen(message)) == -1) {
        perror("Write error");
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
        close(fd);
        exit(1);
    }

    printf("Data written. Press Enter to unlock...");
    getchar();

    // Unlock the file
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl unlock error");
        close(fd);
        exit(1);
    }

    printf("File unlocked.\n");

    close(fd);
    return 0;
}
```

1. Next, we decrement the refcount with `fput()` which will close the underlying file if the count is `1`

Uses [atomic_dec_and_test()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L4500)

1. On the event of reference count reaching 0:

    1. Check if if we are in neither interrupt nor kernel thread execution context, and scheduled a delayed callback appropriately

        1. Note: this is a [decremented](https://elixir.bootlin.com/linux/v6.5/source/include/linux/preempt.h#L125) way to check if we are in an interrupt

        1. `in_interrupt()`, defined transitively by `irq_count()`, is a bitwise OR of all the NMI count, hardirq count, and softirq count values, which resolves to a nonzero integer when any of the three values

        1. The data source for this and many of the other macros in `<linux/preempt.h>` comes from the resolution of `preempt_count()`, whose definition varies by architecture

        1. On arm64, the value is retrieved from the `u32 count` member of the anonymous `struct` named `preempt`, a union member of the current task's [`struct thread_info thread_info`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/sched.h#L744) member

        1. The call is wrapped in the `READ_ONCE()` macro, which prevents the compiling from optimizing the read in ways that may generate race conditions ([more info](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/include/asm/rwonce.h#L36)).

        1. The `current_thread_info()` macro resolves to a simple cast of the `current` macro's underlying `struct task_struct` to a `struct thread_info`, which works because `struct thread_info thread_info` is the first member in a `struct task_struct`

        1. The test for whether the `PF_KTHREAD`, which stands for Process Flag: Kernel Thread, checks whether we are in kernel thread execution context

        1. A kernel thread runs in an execution context that differs from process context by having no access to userspace memory, but is otherwise far more similar to process context than interrupt context as it can sleep, be preempted, and call most kernel functions safely

        1. The conditional check for these contexts is within a `likely()` macro, which generates branch prediction hints in the code for `CPU`s that support this feature, and since there is both a performance boost on the true case as well as a performance hit on the false case, usage of this or the similar but inverted `unlikely()` macro only makes sense if the probability of one branch is much higher than another

        1. Why perform these execution context checks?
            
            1. When we are running in process context, we can schedule a callback to be run in the future on behalf of the current process, which is executed just before we return to userspace

            1. First, we use a wrapper function to initialize the callback functiion within the `struct file * file`'s `struct rcu_head f_rcuhead`, which is transitively defined as a [`struct callback_head`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/types.h#L232)

            1. The [`init_task_work()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/task_work.h#L11) helper simply assignes the argument to the approrpriate member, which cannot fail

            1. Second, we schedule the callback to `____fput` to be run when the task is switching into userspace

            1. Success of [`task_work_add()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/task_work.c#L42) is indicated by zero return value, but in the failure case we simply fall through to the alternative method

            1. Kernel thread execution context and interrupt context don't return to userspace, so we schedule the execution of callback using a global kernel delayed work queue set to run when the next timer interrupt strikes

            1. From [`schedule_delayed_work()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/workqueue.h#L670) we understand that delayed_fput is scheduled to be run by passing a value defined by [`DECLARE_DELAYED_WORK()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/workqueue.h#L196) and a value of `1` jiffy to represent one timer interupt in the future

            1. Since we want to make this call idempotent, we can restrict calls `schedule_delayed_work()` to nonzero return values of `llist_add()`, which indicates that the list had been empty before the call was made

            1. Another property of this restriction is that future calls to `llist_add()` in this function will add the file to the kernel global linked list of files that the next call to `delayed_fput()` will iterate over and pass to `__fput()`

            1. The [`llist`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/llist.h) API provides a lockless atomic linked list implementation, another mechanism to implement correct concurreny in the Linux kernel
    
1. The real `fput()` happens in [`__fput()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file_table.c#L358)

    1. The previously scheduled callback to either `delayed_fput()` or `____fput()` wraps the call to `__fput()`

    1. The simpler `____fput()` uses the [`container_of()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/container_of.h#L18) macro to obtain by struct member offset subtraction the address of the `struct file` in which the `struct callback_head * work` argument is a member, which is then directly passed to `__fput()`

    1. On the other hand, `delayed_fput()` handles a batch of one or more scheduled callbacks to `__fput()`, so we first detach the entire list from the handle known to the caller using `llist_del_all()` before calning `__fput()` on each `struct file *` queued to run between the previous and current calls to this function

    1. Finally, the "real guts of fput()" and hence `close(2)` begin

    1. First we make sure that the file was properly opened by checking for the `FMODE_OPENED` flag  in `file->f_mode`, which is set by [`do_dentry_open()`](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L918) in the `open(2)` codepath

    1. One way to look at `FMODE_OPENED` is that it makes a file closable by allowing execution of `__fput()`, however we still jump to `out` in this case to free the underlying `struct file`

    1. This is the only early return case in this function

    1. The `might_sleep()` macro prints a stack trace when called in atomic context for debugging

    1. Systems like `inotify` should be notified of the file close, and `fsnotify_close()` sends exactly that message to the `fsnotify` system, which sends out these messages to the appropriate consumers

    1. On systems with the commonly enabled [`CONFIG_EPOLL`](https://elixir.bootlin.com/linux/v6.5/source/init/Kconfig#L1594) kernel comfig option, which enables eventpoll support via several syscalls, our call [`eventpoll_release()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/eventpoll.h#L34) will clean up all resoruces associated with event polling on this `struct file`

    1. At this point, it's safe to release all locks associated with this `struct file` with [`locks_remove_file()`](https://elixir.bootlin.com/linux/v6.5/source/fs/locks.c#L2611)

    1. If we are using the Integrity Management Architecture (IMA) subsystem, which prevents tampering with file contents a a security measure, then we need to free IMA information associated with this file using [`ima_file_free()`](https://elixir.bootlin.com/linux/v6.5/source/security/integrity/ima/ima_main.c#L191)

    1. Any pending asnychronous operations on the file should be handled now, a mode indicated by `FASYNC` and implemented by a call to the `struct file_operation file->f_op`'s 'fasync()` member

    1. At this point, any extant `release()` member of `f_op` is called, e.g. invoking the `release()` handler in a character device

    1. If the `struct file`'s `struct inode * f_inode` member is a character device, then we drop a reference we had previoustly held by this `struct file` to that character device's `struct cdev f_cdev`

    1. We also held a reference to the `struct file_operations file->f_op` using a reference to any underlying module as a proxy, so `fops_put()` drops a refernce to the module using `module_put()`

    1. The `struct file` additionally holds a reference to the `pid` that owns it, we drop that reference held in a `struct pid * pid` member of the `struct fown_struct file->f_owner`, which stores other information about the file's owner

    1. Using `put_file_access()` we perform access mode specific tasks to clean up access to the file

    1. Drop a reference to the `dentry` for this file with `dput()`

    1. Some file modes will require at unmount at this point, which is handled by `dissolve_on_fput()`, covered in later material on namespaces

    1. Wrapping up, `mntput()` frees the `struct file`'s [`struct vfsmount`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mount.h#L70) member, which represents an abstraction of a mounted filesystem

    1. Lastly, `file_free()` finishes the job


1. Final steps

    1. Before the direct calls to the memory freeing functions, we perform a couple more cleanup steps before one more scheduling another callback to be run later

    1. Any security modules that allocated data associated with this file via the Linux Security Modules (LSM) framework need to by notified by [`security_file_free()`](https://elixir.bootlin.com/linux/v6.5/source/security/security.c#L2580) to clean up

    1. If the file is being used as a backing store for a device or file, e.g. a loopback device, then we'll need to drop our reference to the `struct path` associated with this other file

    1. Unless we are in the less probably case that the file is not being counted, we decrement the number of open files using a percpu counter periodically synchronized with the global count to optimize for concurrent access to the `nr_files` counter

    1. The final `file_free_rcu()` callback is scheduled by `call_rcu()` to make sure an existing readers get a chance to finish

    1. The last step before we begin freeing is to drop a release to the `struct cred file->f_cred` credentials structure

    1. The [`struct cred`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/cred.h#L110) stores the security context information of the task that opened a file

    1. If the file was opened as a backing file, we free the backing file structure, otherwise we return the memory containing this `struct file` to the `kmem_cache` from whence it came

    1. Following this we return to userspace, which concludes the implememntation of `close(2)`


## Next:

syscall restarting semantics

1. File table entry cleared so can't restart

1. Go into ERESTARTSYS semantics in general

1. Demo: restarting syscall

# `read(2)`

1. [`SYSCALL_DEFINE3(read, ...)`]( https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L621)

    1. Just calls `ksys_read()`

1. [`ksys_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L60)


    1. First, we get a reference to the file position using a `struct fd`

    1. Problem: we don't always need to have our own reference to the struct file

    1. struct fd contains borrowed or cloned references to a struct file

        1. Along with information about the reference stored in the flags

    1. struct fd and fdget/fdput allow us to borrow or clone a reference

        1. Only allowed when we are going to drop the reference before returning to userspace

        1. fine to borrow if fd table isn't shared

        1. Similarly optmized for the file position

        1. __fdget_pos() called by by fdget_pos()

    [`__fdget()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1028 )

        1. Just calls fdget light

        1. We check if it's safe to borrow the reference

[`__fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1055)


[`__fdget()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1028)

Relies on 
[`__fdget_light()`](https://elixir.bootlin.com/linux/v6.5/source/fs/file.c#L1002)

f.file => https://lore.kernel.org/all/20240730051625.14349-2-viro@kernel.org/


1. 

[`vfs_read()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L450)

[`file_ppos()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L597)

[`fdget_pos()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L72)

[`fdput_pos()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/file.h#L77)

[`rw_verify_area()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L355)




---

# `write(2)`

---

# In the begining

#### [`SYSCALL_DEFINE(write,...)`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L646)

1. All it does is ksys_write

1. Only one other caller in [s390 compat code](https://elixir.bootlin.com/linux/v6.5/source/arch/s390/kernel/compat_linux.c#L230)

1. [Originally](https://github.com/torvalds/linux/commit/e7a3e8b2edf54) there were more callers


---

# Where did these callers go?

While file descriptors are prefered as a userspace interface, the kernel is better off working directly with `struct file`s

[`ksys_write()` removed from init/initramfs.c](https://github.com/torvalds/linux/commit/bf6419e4d5440c6d414a320506c5488857a5b001)

[`ksys_write()` removed from init/do_mounts_rd.c](https://github.com/torvalds/linux/commit/bef173299613404f55b11180d9a865861637f31d)

1. Notice that ksys_lseek is entirely removed

---

# The other kernel interface

[`kernel_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L548)

1. Verify the write operation

1. 

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

1. See the [slides on read](https://inst1.dev.underground.software/slides/fd3.html)

1. We will skip right to `vfs_write()`

---

# Right into the meat

#### [`vfs_write()`](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L564)

# `ioctl(2)`


# `lseek(2)`

