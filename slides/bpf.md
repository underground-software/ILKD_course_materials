# BPF

---

# Learning objectives:

1. Understand the purpose of the Linux kernel's BPF subsystem

1. Understand how to peform basic tracing and analysis with the bpftrace tool

---

# Overview

1. What is BPF?

1. Where did BPF come from?

1. Major features of BPF

1. Probes

1. BCC: BPF Compiler Collection

1. Using bpftrace and some internals

---

# What is BPF?

1. BPF stands for BSD Packet Filter

1. Original BPF: packet filter

1. Modern BPF: general-purpose in-kernel virtual machine

---

# History of BPF: motivation

Efficient packet filtering

---

# History of BPF: origins

1993 [paper](https://www.tcpdump.org/papers/bpf-usenix93.pdf)

Huge performance gain for SunOS

---

# History of BPF: in Linux

2013 [rewrite](https://lkml.org/lkml/2013/9/30/627)

1. Generalize BPF instruction set

1. Verified just-in-time compilation from C to BPF to native code

1. Subsystems can provide BPF functions

---

# Major features

Tracing and observability

1. Main focus of later examples

---

# Major features

Event-driven programming rather than task-driven

---

# Major features

Runtime verification

---

# Major features

Networking

Example: XDP: Express data path

---

# Major features

Performance profiling

1. e.g. `perf`

---

# Dynamic instrumentation: History

Dyanmic instrumentation considered harmful

1. Used in 1990s: same technique debuggers use to place breakpoints

1. e.g. kerninst, which has a really old [website](https://www.paradyn.org/projects/legacy/kerninst/)

---

# Dynamic instrumentation: Linux rejects

DProbes by IBM rejected in 200

1. Many preferred C over bytecode

1. Rejection used as case study in [this paper](http://halobates.de/on-submitting-patches.pdf)

---

# Dynamic instrumentation: rusing popularity

Promoted by Sun first, and not for Linux!

1. DTrace released to wide acclaim in Solaris 10

1. Scripting lanuage for DTrace was called D

1. Not to be [confused](https://en.wikipedia.org/wiki/D_(disambiguation)#Computing) with the [D programming language](https://en.wikipedia.org/wiki/D_(programming_language))

---

# Note on next couple of examples

Looking at probe types, not syntax details

---

# kprobes

Dynamic kernel code instrumentation

1. Replace first byte of instruction with breakpoint

1. Similar concept: kretprobes for function return

---

# demo: tracing `do_nanosleep`

`do_nanosleep()` [source](https://elixir.bootlin.com/linux/v6.5/source/kernel/time/hrtimer.c#L2038)

---

# uprobes

Dynamic userspace program instrumentation

---

# demo: tracing bash's readline function

Readline [definition](https://git.savannah.gnu.org/cgit/readline.git/tree/readline.c#n354)

---

# tracepoints

statically defined: more stable interface

1.  `sys_enter#sname` in [include/linux/syscalls.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/syscalls.h)

1. tracepoint metadata defined here: [include/trace/events/syscalls.h](https://elixir.bootlin.com/linux/v6.5/source/include/trace/events/syscalls.h)

---

# demo: tracepoint probe type for open syscall

1. [open syscall definition](https://elixir.bootlin.com/linux/v6.5/source/fs/open.c#L1426)

---

# Using BCC: BPF compiler collection

Set of [tools and libraries](https://github.com/iovisor/bcc)

---

# demo

`bcc/tools/opensnoop.py`

---

# bpftrace

The simpler, more lightweight option

---

# demo
        
`bpftrace/tools/opensnoop.bt`

1. less well-featured than BCC version

---

# bpftrace syntax

Similar to awk

---

# demo

simple awk program

---

# bpftrace invocation

1. `#!/bin/bpftrace` to write a script

1. To run a oneliner from shell: `bpftrace -e`

1. To list possible probes: `bpfrace -l`

    1. Can use `*` wildcards

---

# bpftrace maps

Key value store

1. Use `@<id>`

1. can be unnamed

---

# demo

Count syscalls invoked per-process

---

# demo

Histogram of bytes read by a process

---

# bpftrace live stacks

Use `kstack` and `ustack` builtins

---

# Note

bpftrace reads on a per-cpu basis

---

# demo

`sudo bpftrace -e 'k:ksys_read {printf("%s\n", kstack);}'`

---

# demo

`sudo bpftrace -e 'k:ksys_read {printf("%s\n", ustack);}'`

---

![bpftrace internals](https://www.brendangregg.com/Slides/LISA2021_BPF_Internals/LISA2021_BPF_Internals_020.jpg)

---

![bpftrace compilation](https://www.brendangregg.com/Slides/LISA2021_BPF_Internals/LISA2021_BPF_Internals_021.jpg)

---

# bpftrace code to AST

bpftrace lexer [source](https://github.com/bpftrace/bpftrace/blob/master/src/lexer.l) and parser [source](https://github.com/bpftrace/bpftrace/blob/master/src/parser.yy)

1. Generates LLVM Intermediate Representation using [this target](https://github.com/llvm/llvm-project/tree/main/llvm/lib/Target/BPF)

---

# demo

The bptrace `-d` option 

---

# BTF

BPF type format from kernel

---

# demo

strace a bpf program: `bpf(2)` system call in action

---

# BPF JIT

All BPF programs JITed

1. Used to be an interpreted option

---

# In-kernel verification

Hundreds of error returns!

1. [kernel/bpf_verifier.c](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/verifier.c)

---

# In-kernel representation

1. LLVM IR to bytecode

1. Bytecode to native code

---

# demo

Use `bpftool` to see translated and jitted BPF program

---

# BPF kernel entry points

Example: definition of [`bpf_get_current_pid_tgid()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/bpf/helpers.c#L217)

---

# Further exploration

include/uapi/linux/{bpf,bpf_common,filter}.h

---

# detour: unlikely()?

Compiler optimization for code arrangement

1. See [include/linux/compiler.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/compiler.h#L77)

---

# Entry point for further exploration

See more: [bpftrace kselftests](https://elixir.bootlin.com/linux/v6.5/source/tools/testing/selftests/bpf)

1. [General selftest info](https://www.kernel.org/doc/html/v6.5/dev-tools/kselftest.html)

1. Good first contribution

---

# Summary

Linux BPF is an in-kernel general-purpose execution engine

--- 

# Summary

BPF programing is event-driven

1. Various probe types provide different triggers for invoking BPF

---

# Summary

1. `kprobes` provide dynamic kernel instrumentation

1. `uprobes` provide dynamic userspace instrumentation

1. `tracepoints` provide a more stable, static kernel tracing interface

---

# Summary

BPF refers to Linux eBPF, a major rewrite of classic BPF

1. Classic BPF is an optimization for network packet filtering

---

# Summary

`BCC` and `bpftrace` are two common frontends to the kernel's BPF subsystem

1. Use `BCC` for more serious tool development

1. Use `bpftrace` for quick interaction and prototyping tools

---

#  Summary

`bpftrace` is an awk-like scripting lanuage

1. provides a quick, modern way to see what's going on inside the kernel

---

# End

Check out Brendan Gregg's work

1. [Good talk on more internals](https://www.youtube.com/watch?v=_5Z2AU7QTH4&t=5s)

1. [Good book to dive into more depth](https://www.brendangregg.com/bpf-performance-tools-book.html)
