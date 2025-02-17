#### notes

Memory management

(maybe link OSTEP)

What is memory management?

Allocation, paging, phsyical, virtual

## Virtual memory


Start without any: this is NOMMU mode

### Address space isolation

discuss clone system call

namespasce basic demo

`clone(CLONE_VM)`

CLONE_VM: The new process shares the same memory space as the parent.
CLONE_FS (Optional): Shares file system information.
CLONE_FILES (Optional): Shares file descriptors.
CLONE_SIGHAND (Optional): Shares signal handlers.

Question: How is fork defined?

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

Disuss namespaces: isolated view of system resources

Mount Namespace (mnt) – Isolates filesystem mount points.
Process Namespace (pid) – Provides separate process ID trees.
Network Namespace (net) – Gives processes their own networking stack.
User Namespace (user) – Provides separate user and group IDs.
UTS Namespace (uts) – Isolates hostname and domain name.
IPC Namespace (ipc) – Isolates inter-process communication mechanisms.
Cgroup Namespace (cgroup) – Provides separate views of control groups.

address space isolation with NOMMU: not possible

Page tables and the TLB

Can we acheive address space isolation without the MMU? No

Building the page tables

Types of kernel kernel addresses: logical, virtual

## Slob/Slab/Slub allocators

memory allocation, deallocation,slub, and reclaim

## Paging, page reclaim, swap

What's bigger: 10G of physical memory of 10G of virtual memory (joke)

What's bigger: physical or virtual memory?

Answer: virtual memory. Why?

On 64 bit system each process had address space of size (1ULL << 63)!

Therefore: need to swap out pages so phys mem doesn't get full


What is swap?

Demo: look at `free -mh`

Move memory pages onto disk

Least recently used (LRU) algorithm


### OOM

What happens when you run out of memory?

Answer: out of memory killer is activated to kill a memory-hogging process

Demo: look at mm/oom_kill.c

### Page allocation: buddy system

Pages allocation in power-of-two groups

## Memory topology

Topology and tracking physical and virtual memory

### NUMA

## Summary

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
