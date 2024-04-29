**KDLP Fall 2023 Midterm Presentation Guidelines**

* Overview:
    * Short presentation about research into a topic of your choice live during class
    * Propose topic and outline ahead of time
    * Plenty of room to be creative 🙂
* Example midpoint presentation topics (pick one of these or propose your own):
    * What is the OOM killer?
    * Why are there multiple versions of some syscalls with numbers at the end? What issue were they added to address?
        * pipe, pipe2, pipe3
        * dup, dup2, dup3
        * etc.
    * Why do some syscalls have a version ending in `at`? What issue were they added to address?
        * open vs openat
        * fstat vs fstatat
        * rename vs renameat
        * symlink vs symlinkat
        * etc
    * What does .config do? How does it interact with the build process for the kernel?
    * What is physical and virtual memory? Why is there a distinction between the two?
    * What is the history of git? How does it fit into the bigger picture of version control software?
    * Or pick your own!
* Select a topic and submit your proposal with a short outline as soon as possible. The hard due date is at 23:59 Eastern on 10/31/2023
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
        * There will be two presentations per day on L16, L17, L18, L19, and L20
