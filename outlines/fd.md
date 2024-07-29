# fd: open

[NR_OPEN_DEFAULT](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fdtable.h#L24)

[struct files_struct](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fdtable.h#L49)

[->files in task_struct](https://elixir.bootlin.com/linux/v6.5/source/include/linux/sched.h#L21)


[AT_FDCWD](https://elixir.bootlin.com/linux/v6.5/source/include/uapi/linux/fcntl.h#L96)

[ARCH_32_BIT_OFF_T](https://elixir.bootlin.com/linux/v6.5/source/arch/Kconfig#L348)


[*at version preferred](https://elixir.bootlin.com/linux/v6.10/source/include/linux/syscalls.h#L1003)

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

https://elixir.bootlin.com/linux/v6.10.2/source/fs/open.c#L1197)
[S_IALLUGO](https://elixir.bootlin.com/linux/v6.5/source/include/linux/stat.h#L10)

1. read, write, exec for user, group, and other
    * setuid, setgid, sticky bit

[permission bits](https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html)

[build_open_flags()](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1207)


[https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1401]

[getname_flags](https://elixir.bootlin.com/linux/v6.5/source/fs/namei.c#L130)

getname: turn a string filename into a struct filename *

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

