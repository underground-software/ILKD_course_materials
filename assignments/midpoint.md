## Midpoint Presentation Guidelines 🕴️

You will create a brief, pre-recorded presentation on research into a Linux kernel related topic of your choice

### Presentation requirements

* 10 ± 1 minutes long

* Must include some sort of visual component

    * Slideshow

    * Diagrams

    * Etc.

* Must include some sort of live demo in the terminal, even if brief

* Must include discussion of some snippet(s) of code that is relevant to your topic

* Plenty of room to be creative 🙂

### What and how to submit

* You will propose your choice of topic ahead of time

    * You may chose a topic from the list below or come up with your own

    * Each student must choose a unique topic, so check the #midpoint channel to see if your choice is available

    * The proposal must begin with a one sentence summary of the topic you wish to cover

    * The proposal must contain a rough outline of what you plan to discuss in the form of approximately a half dozen bullet points

    * We will reply to your message with the proposal to let you know if it is approved or what our concerns are if we cannot approve it

    * **Submit your proposal in the #midpoint channel on Matrix by midnight February 27 2025**

* You will submit your presentation by posting a link to your recording in Matrix

    * Suggestion: use OBS to record your presentation as a livestream to youtube
    and set the video visibility to "unlisted" so only those with the link can access

    * You may use any video hosting platform of your choice, but make sure anyone with the link
    can acces your video before you submit

    * There is no peer review and additional final submission for this assignment

    * **Submit the link to your video in the #midpoint channel on Matrix by midnight 20 March 2025**

### Example topics

* What is the OOM killer and what algorithm does it implement?

* What is the Linux VDSO? What problem does it try to solve? How does it compare to other historical techniques like vsyscall?

* How does the Linux BPF system let userspace run arbitrary code in kernel mode? What techniques does it use to ensure that this is safe?

* Why are there multiple versions of some syscalls with numbers at the end (e.g. `accept`, `dup`, `pipe`, etc)? What do the numbers refer to?

* Why do some syscalls have a version ending in `at` (e.g. `open`, `fstat`, `rename`, `symlink`, etc)? What issue were they added to address?

* What mechanisms can be used in the kernel to do performance profiling and how?

* What role does the rust programming language play in the kernel?

* How do concurrent data structures and Read-Copy-Update (RCU) mitigate race conditions in the Linux kernel without using locks?

* How does Facebook's sched_ext BPF extension re-implement the scheduler?
