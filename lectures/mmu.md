#### notes

Memory management

Virtual memory, and memory separation (software/HW interaction)

Address space isolation

Disuss namespaces

namespasce basic demo

`clone(CLONE_VM)`

CLONE_VM: The new process shares the same memory space as the parent.
CLONE_FS (Optional): Shares file system information.
CLONE_FILES (Optional): Shares file descriptors.
CLONE_SIGHAND (Optional): Shares signal handlers.

fork definition:
it's just clone lol

2887 #ifdef __ARCH_WANT_SYS_FORK
2888 SYSCALL_DEFINE0(fork)
2889 {
2890 #ifdef CONFIG_MMU
2891         struct kernel_clone_args args = {
2892                 .exit_signal = SIGCHLD,
2893         };
2894
2895         return kernel_clone(&args);
2896 #else
2897         /* can not support in nommu mode */
2898         return -EINVAL;
2899 #endif
2900 }
2901 #endif


address space isolation with NOMMU: not possible


memory allocation, deallocation,slub, and reclaim

page reclaim :D

Topology and tracking physical and virtual memory

NUMA

do

Possible demos:
--------
1. shell/busybox in no MMU mode, even something simple
    problems:
        needs fairly well featured C library, uses many headers
        tries linking with lm and lrt even when not needed
1. riscv program that calls clone and makes a different pid

1. nommu programs that touch each other

1. busybox in MMU mode, now we have a userspace
