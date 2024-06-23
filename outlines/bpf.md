# BPF

1. Learning objectives:

    1. Understand the purpose of the Linux kernel's BPF subsystem

    1. Understand how to peform basic tracing and analysis with the bpftrace tool

1. Overview

    1. What is BPF?

    1. Where did BPF come from?

    1. Major features of BPF

    1. Probes

    1. BCC: BPF Compiler Collection

    1. Using bpftrace and some internals

1. What is BPF?

    1. BPF stands for BSD Packet Filter

    1. Original BPF: packet filter

    1. Modern BPF: general-purpose in-kernel virtual machine

1. History of BPF

    1. History of the name

    1. 1993 [paper](https://www.tcpdump.org/papers/bpf-usenix93.pdf)

        1. Used SunOS

        1. Compare to pre-filter copier STREAMS NIT

        1. Huge performance gain

    1. Motivation: efficient packet filtering

        1. Discard packets as early as possible

        1. Speed up e.g. tcpdump

    1. 2013 [rewrite](https://lkml.org/lkml/2013/9/30/627)

        1. Generalize BPF instruction set

        1. Enable just-in-time compilation from C to BPF to native code

        1. Verified programs by searching through execution paths and disallowing loops

        1. Subsystems can provide BPF functions

1. Major features of BPF

    1. Event-driven programming rather than task-driven

    1. Tracing and observability

    1. Runtime verification

    1. Networking

        1. XDP: Express data path

            1. send/recv data packets faster than networking stack

            1. Open source, multi platform

    1. Performance checks

        1. e.g. `perf`

1. Probes

    1. History of dynamic instrumentation

        1. Risky: live modification of kernel code

        1. 1990s: same technique debuggers use to place breakpoints

            1. e.g. kerninst, which has a really old [website](https://www.paradyn.org/projects/legacy/kerninst/)

        1. First proposal: DProbes by IBM

            1. Patchset rejected in 2000

            1. Kernel engineers were skeptical

            1. Many preferred C over bytecode

            1. Rejection used as case study in [this paper](http://halobates.de/on-submitting-patches.pdf)

        1. Popularized by Sun Microsystems in Solaris

            1. DTrace released in Solaris 10

            1. Scripting lanuage for DTrace was called D

            1. Not to be [confused](https://en.wikipedia.org/wiki/D_(disambiguation)#Computing) with the [D programming language](https://en.wikipedia.org/wiki/D_(programming_language))

            1. Widely promoted by Sun Microsystems and lead to popular adoption

            1. Linux kprobes and uprobes based on this work

    1. Focus of next few examples: probe types

        1. We will return to syntax details later

    1. kprobes

        1. Dynamic kernel code instrumentation

        1. Replace first byte of instruction with breakpoint

        1. Similar concept: kretprobes for function return

        1. demo: nanosleep

            1. `do_nanosleep()` [source](https://elixir.bootlin.com/linux/v6.5/source/kernel/time/hrtimer.c#L2038)

            1. kretprobe and kprobe same function

    1. uprobes

        1. Dynamic userspace program instrumentation

        1. demo: tracing bash's readline function (readline)

            1. Dynamic library bash depends on

            1. Readline [definition](https://git.savannah.gnu.org/cgit/readline.git/tree/readline.c#n354)

    1. tracepoints

        1. see: `sys_enter#sname` in [include/linux/syscalls.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/syscalls.h)

        1. this is part of `SYSCALL_METADATA` from the `SYSCALL_DEFINE` macro

        1. tracepoint metadata defined here: [include/trace/events/syscalls.h](https://elixir.bootlin.com/linux/v6.5/source/include/trace/events/syscalls.h)

        1. demo: tracepoint probe type for open syscall (tracepoint_open)

        1. [open syscall definition](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1426)

1. Using BCC

    1. BPF compiler collection

    1. demo: `bcc/tools/opensnoop.py`

    1. There are also various C programs, libbpf, gobpf

        1. [BCC git](https://github.com/iovisor/bcc)

    1. bpftrace is simpler
        
        1. demo: `bpftrace/tools/opensnoop.bt`

        1. less well-featured than BCC version

1. Using bpftrace

    1. Similar to awk

        1. demo: awk program (awk_easy)

    1. Invocation

        1. As seen, `#!/bin/bpftrace` to write a script

        1. To run a oneliner from shell: `bpftrace -e`

        1. To list possible probes: `bpfrace -l`

            1. Can use `*` wildcards

    1. Maps: key value store

        1. Use @ syntax, can be unnamed

        1. Created with syscall: `bpf(BPF_MAP_CREATE,...`

        1. demo: count syscalls invoked per process (syscalls)

        1. demo: histogram of bytes read by a process (read_histogram)

    1. stack access with kstack and ustack

        1. demo: `sudo bpftrace -e 'k:ksys_read {printf("%s\n", kstack);}'`

        1. demo: `sudo bpftrace -e 'k:ksys_read {printf("%s\n", ustack);}'`

    1. getting into internals

        1. https://www.brendangregg.com/Slides/LISA2021_BPF_Internals/#20

        1. https://www.brendangregg.com/Slides/LISA2021_BPF_Internals/#21

    1. bpftrace code to AST

        1. Show usage with -d instruction

        1. flex and bison used to create lexer and parser

            1. lexer [source](https://github.com/bpftrace/bpftrace/blob/master/src/lexer.l)

            1. parser [source](https://github.com/bpftrace/bpftrace/blob/master/src/parser.yy)

    1. LLVM Intermediate Representation

        1. LLVM target [source](https://github.com/llvm/llvm-project/tree/main/llvm/lib/Target/BPF)

    1. BTF: BPF type format from kernel

    1. The `bpf(2)` system call

        1. BPF_PROG_LOAD and arguments

        1. demo: strace a bpf program

    1. All BPF programs JITed

        1. Used to be an interpreted option

        1. JIT Faster

        1. Allows patching JIT to mitigate security flaws

    1. In-kernel Verification

        1. [kernel/bpf_verifier.c](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c)

        1. Hundreds of error returns!

        1. [Can only read initialized memory](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c#L4480)

        1. [All loops bounded](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c#L14650)

        1. [All instructions reachable](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c#L14702)

        1. [All codeppaths exit](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c#L44)

    1. In-kernel representation

        1. Use `bpftool` to examine the kernel's BPF programs

        1. get id: `sudo bpftool prog show`

        1. show bytecode: bpftool prog dump xlated id <ID>

        1. Then, the real jitting:

            1. show assembly: bpftool prog dump jitted id <ID>

        1. demo: do this process to an example bpftrace program

    1. bpf functions called in the kernel:

        1. Example: definition of [`bpf_get_current_pid_tgid()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/helpers.c#L217)

        1. What is unlikely?

            1. See [include/linux/compiler.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/compiler.h#L77)

            1. Compiler optimization for code arrangement

        1. include/uapi/linux/{bpf,bpf_common,filter}.h


    1. Note:  bpftrace reads on a per-cpu basis: may be out of order

    1. See more: [bpftrace kselftests](https://elixir.bootlin.com/linux/v6.5/source/tools/testing/selftests/bpf)

        1. [General selftest info](https://www.kernel.org/doc/html/v6.5/dev-tools/kselftest.html)

1. Summary

    1. Linux BPF is an in-kernel general-purpose execution engine

    1. BPF programing is event-driven

        1. Various probe types provide different triggers for invoking BPF

        1. `kprobes` provide dynamic kernel instrumentation

        1. `uprobes` provide dynamic userspace instrumentation

        1. `tracepoints` provide a more stable, static kernel tracing interface

    1. BPF refers to Linux eBPF, a major rewrite of classic BPF

        1. Classic BPF is an optimization for network packet filtering

    1. `BCC` and `bpftrace` are two common frontends to the kernel's BPF subsystem

        1. Use `BCC` for more serious tool development

        1. Use `bpftrace` for quick interaction and prototyping tools

    1. `bpftrace` is an awk-like scripting lanuage

        1. `bpftrace` provides a quick and modern way to see what's going on inside the kernel

    1. [Good talk on more internals](https://www.youtube.com/watch?v=_5Z2AU7QTH4&t=5s)

    1. [Good book to dive into more depth](https://www.brendangregg.com/bpf-performance-tools-book.html)
