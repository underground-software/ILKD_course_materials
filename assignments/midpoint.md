## Midpoint Presentation Guidelines 🕴️

You will deliver a brief presentation on research into a kernel related topic of your choice

### Presentation requirements

* 7 ± 1 minutes long

* Must include some sort of visual component

    * Slideshow

    * Diagrams

    * Etc.

* Must include some sort of live demo in the terminal, even if brief

* Must include discussion of some snippet(s) of code that is relevant to your topic

* Plenty of room to be creative 🙂

### General Advice

* Go deep

    * It is much more interesting to pick a specific detail of some topic and really dig into it than offer a broad but shallow overview of a large scope

* Show don't tell

    * It is much more interesting to see e.g. the actual code, commit message, mailing list discussion, running behavior than a coarse summary or a bullet on a slide

### What and how to submit

* You will propose your choice of topic ahead of time

    * You may chose a topic from the list below or come up with your own

    * The proposal must begin with a one sentence summary of the topic you wish to cover

    * The proposal must contain a rough outline of what you plan to discuss in the form of approximately a half dozen bullet points

    * We will reply to your message with the proposal to let you know if it is approved or what our concerns are if we cannot approve it

    * **Submit your proposal in the #midpoint channel on Matrix by midnight October 14th 2025**

    * Each student must choose a unique topic, so check the #midpoint channel to see if your choice is available

        * Note: it is potentially acceptable that two proposals start from the same example idea, as long as the details are substantially different

        * This goes hand in hand with the suggestion above to go deep, many students can pick different specific details to focus on.

* You will make your presentation live during class during a pre-assigned slot at the beginning of class on weeks 6, 7, 8

### Example topics

* Compare and contrast open source operating systems:

    * Pick an operating system with an open source kernel and find out how it differs from the Linux kernel

    * Focus on a specific design decision and unpack the history and reasoning behind it.

    * Compare and show off the relevant code for the contrasting features in both kernels

    * Demo the relevant features live

    * Here are some examples of open source operating systems and notable features (this list is definitely not exhaustive, just a starting point):

    * Minix

        * Microkernel vs monolithic kernel

    * Darwin XNU / Mach

        * Ports as IPC primitive vs shared memory or pipes

    * GNU Hurd

        * Translators vs FUSE

        * Servers vs device drivers

    * FreeBSD

        * Jails vs namespaces

    * NetBSD

        * Rump kernel vs FUSE

    * OpenBSD

        * unveil/pledge vs seccomp / LSM

    * DragonFly BSD

        * Serializing tokens vs RCU

    * OpenSolaris

        * Solaris Zones for virtualization vs kvm or namespaces

        * Dtrace vs Ftrace

    * SerenityOS

        * Fully C++ kernel vs C kernel

        * Structured json data in /proc vs unstructured data

    * Fuchsia

        * Capability-based permissions vs ambient authority

    * plan9

        * 9p protocol and Process namespaces vs Standard File System Hierarchy

* Something else:

    * Rather than listing specific suggestions, I encourage you to come up with something that really interests you

    * Should be related to the kernel, with a particular emphasis on design decisions, their supporting rationale and the resulting code
