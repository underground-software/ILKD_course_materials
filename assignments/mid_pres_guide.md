**KDLP Fall 2023 Midterm Presentation Guidelines**

* Overview:
    * Short presentation about research into a topic of your choice live during class
    * Propose topic and outline ahead of time
    * Plenty of room to be creative 🙂
* Example midpoint presentation topics (pick one of these or propose your own):
    * What is the OOM killer?
    * Why are there multiple versions of some syscalls with numbers at the end (e.g. `accept`, `dup`, `pipe`, etc)? What do the numbers refer to? What issue were they added to address?
    * Why do some syscalls have a version ending in `at` (e.g. `open`, `fstat`, `rename`, `symlink`, etc)? What issue were they added to address?
    * What does `.config` do? How does it interact with the build process for the kernel?
    * What is physical and virtual memory? Why is there a distinction between the two?
    * What is the history of git? How does it fit into the bigger picture of version control software?
    * How do container technologies like Podman, Docker, or LXC "share" a kernel with the host machine? What do they not share? What mechanisms of the linux kernel underly this technology?
    * How does the linux BPF system let userspace run arbitrary code in kernel mode? What techniques does it use to ensure that this is safe?
    * What concept does the phrase "Don't break userspace" refer to in the linux kernel? What does it look like in practice?
    * What is the linux VDSO? What problem does it try to solve? How does it compare to other historical techniques like vsyscall?
    * Or suggest your own!
* Select a topic and submit your proposal with a short outline as soon as possible in the new `#midpoint` channel on matrix.
    * The hard due date for proposals is 23:59 on 10/10/2024
    * The proposal must begin with a one setance summary of the topic you wish to cover (e.g. "What does .config do?")
    * The proposal must contain a rough outline of what you plan to discuss in the form of approximately a half dozen bullet points
    * We will reply to your message with the proposal to let you know if it is approved or what our concerns are if we cannot approve it
* Presentation requirements:
    * 10 ± 1 minutes long
    * Must include some sort of visual component:
        * Slideshow
        * Diagrams
        * Etc.
    * Must include some sort of live demo in the terminal
        * Must include discussion of some snippet(s) kernel code that is relevant to your topic
    * Present live during class time in the class Jitsi
        * Share your screen for terminal + visual elements
        * There will be three presentations per day at the start of class during L11, L12, L13, L14, L15, L16, L17, L18, L19, L20, and L21
        * The exact schedule for which student is presenting on what day will be posted on matrix in the general channel
