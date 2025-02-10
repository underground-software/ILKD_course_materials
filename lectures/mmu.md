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

Working Demo:
--------
1. asm init program which spawns a child and waits for it to execute, then reboots
  1. we want to see if the child can access and modify shared memory with NOMMU
  1. with NOMMU enabled, show that there should not be any address protection by default.
  1. is there address protection baked into RISCV?
    1. when booting a qemu riscv, mmu=off option may be necessary in order to truly run without adress protection
    1. the `no address protection` message gets triggered regardless of whether the option is set
  1. seems like when NOMMU, there is no address protection and a child process is able to modify the shared address!
  1. now lets turn off the NOMMU
  1. what do you think will happen?
  1. yes, we get EFAULT
  1. but where does it fault?
  1. seems like MMU does indeed provide address protection
  1. does the mmu cpu flag on QEMU make a difference?
    1. if the flag is on with NOMMU kernel, behaviour doesn't change
    1. if flag is on with MMU kernel, then it will hang, we will not even see an EFAULT
1. now see the C version of init
  1. easier to discern what is going on, command execution and debugging
