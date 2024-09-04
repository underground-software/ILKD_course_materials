### Course Schedule

Week 00 | 26 May 2024
--

Assignment due: none

Monday:
 - L00
   - Syllabus overview, who are we, linux crash course slides <https://kdlp.underground.software/course/slides/linux_crash_course.html>
      * <https://kdlp.underground.software/course/fall2023/policies/course_policies.md>
      * <https://kdlp.underground.software/course/fall2023/policies/submission_guidelines.md>
      * <https://kdlp.underground.software/course/fall2023/policies/peer_review.md>
      * TBA: turn these into full syllabus
      * reentrancy
      * kernelspace vs userspace
      * buffer overflow severity
    * Unix vs Linux distinction
        * TBA: more detail on history, relation to other OSes like BSD or Darwin
        * <https://kdlp.underground.software/course/fall2023/lectures/L11.md>
   - Before next class watch video on cmdline basics <https://youtu.be/qcue__Z2n6w> and git <https://kdlp.underground.software/course/slides/git.html>

Wednesday:
 - L01
   - Working through setup process <https://youtu.be/Ub62duJEPhQ> live, student guided, hands on in the terminal
    * what is a shell?
      * Material [here](https://kdlp.underground.software/course/fall2023/lectures/L01.md)
      * P0 intro [video](https://www.youtube.com/watch?v=JqljsuVaUIU)

Week 01 | 2 June 2024
--

Assignment due: [setup](assignments/setup.md)

Monday:
 - L02
   - Some sort of in class git workshop *TBA: need to design this*
    * git
      * [Our cgit](https://kdlp.underground.software/cgit)
      * [Slide deck](https://kdlp.underground.software/course/slides/git.html)
      * [Some intro material](https://kdlp.underground.software/course/fall2023/lectures/L02.md)

Wednesday:
 - L03
   - Build a distro, working with makefiles, configure scripts <https://kdlp.underground.software/course/fall2023/lectures/L04.md> <https://youtu.be/aAoJyUaA1ok>
   - kbuild system, Kconfig
   * old video: E0 intro [video](https://www.youtube.com/watch?v=zYB72Rnz3TA)

Week 02 | 9 June 2024
--

No Wednesday class due to Shavuot holiday: cannot have assignments due then

Assignment due: none

Monday:
 - L04
   - Write example C program + makefile <https://kdlp.underground.software/course/fall2023/lectures/L08.md>
   - Work through phases of C compilation, what is assembly vs machine code, what are object files, reveal all steps in makefile

Week 03 | 16 June 2024:
--

Assignment due: [E0](assignments/E0.md)

Monday:
 - L05
  - Basically the OS assignment where you run grep in a child process <https://cs.uml.edu/~bill/cs308/Assignment_2.pdf>
  - open read write fork exec dup pipe wait close exit

Wednesday:
- L06
   - Worked example, what are processes, what are syscalls, what are file descriptors *TBA: design this content*
      [Demo part 1](https://kdlp.underground.software/course/fall2023/lectures/L01.md)
      [Demo part 2](https://kdlp.underground.software/course/fall2023/lectures/L03.md)

Week 04 | 23 June 2024:
--

Assignment due: [P0](assignments/P0.md)

Monday:
 - L07
   - What is CPU privilege, trying to execute forbidden instructions, what happens and why <https://kdlp.underground.software/course/fall2023/lectures/L05.md>

Wednesday:
 - L08
   - What causes kernel code to run, exceptions, traps, interrupts <https://kdlp.underground.software/course/fall2023/lectures/L08.md>

Week 05 | 30 June 2024:
--

No Monday lecture: the studies according the Thursday (no effect on ability to have hw due this week)

Assignment due: [E1](assignments/E1.md)

Wednesday:
 - L09
   - tracing kernel code with ftrace <https://kdlp.underground.software/course/fall2023/lectures/L07.md> *TBA: design specific demo for this*

Week 06 | 7 July 2024:
--

Assignment due: [P1](assignments/P1.md)
Discuss and assign: [Midpoint Presentation](assignments/mid_pres_guide.md)

Monday:
 - L10
    - tracing kernel code with bpftrace

Wednesday:
 - L11
   - What do we want for presentations in this class, no exams means we have high expectations <https://kdlp.underground.software/course/fall2023/assignments/mid_pres_guide.md>
   - We are able to run forbidden stuff when we are writing kernel code <https://kdlp.underground.software/course/fall2023/lectures/L06.md>

Week 07 | 14 July 2024:
--

Assignment due: none

Monday:
 - L12
   - Write simple hello world assembly program, begin tracing how syscall instruction works <https://kdlp.underground.software/course/fall2023/lectures/L10.md> <https://kdlp.underground.software/course/fall2023/lectures/L08.md>

Wednesday:
 - L13
   - Finish syscalls end to end, how do we get back to syscall in our userspace program <https://kdlp.underground.software/course/fall2023/lectures/L09.md>

Week 08 | 21 July 2024:
--

Assignment due: [E2](assignments/E2.md)

Monday:
 - L14
   - playing around with /proc /sys /dev *TBA: design specific demo*  <https://kdlp.underground.software/course/fall2023/lectures/L13.md> <https://youtu.be/RwvSsDOGWXE>
    * <https://kdlp.underground.software/course/fall2023/lectures/L11.md>

Wednesday:
 - L15
   - writing a hello world module <https://kdlp.underground.software/course/fall2023/lectures/L12.md> <https://youtu.be/Phh05oG4WHg>

Week 09 | 28 July 2024:
--

Assignment due: [P2](assignments/P2.md)

Monday:
 - L16
   - kernel data types `struct file`, `struct inode`, `struct file_operations`, `dev_t`, etc <https://kdlp.underground.software/course/fall2023/lectures/L15.md> <https://youtu.be/KiKGc67107Y> <https://kdlp.underground.software/course/fall2023/lectures/L16.md> <https://youtu.be/L1VJ9kEZ2zY>

Wednesday:
 - L17
   - advanced char dev operations, ioctl, lseek, etc *TBA: do we have stuff from an even earlier semester?*

Week 10 | 4 August 2024:
--

Assignment due: [F0](assignments/F0.md)

Monday:
 - L18
   - Debug/fix userspace program with race conditions *TBA: need to design this demo*
   - kernel locking API

Wednesday:
 - L19
   - Add locking to P2 code

Week 11 | 11 August 2024:
--

Assignment due: none

Monday:
 - L20
   - Convert P2 code to use a misc-device

Wednesday:
 - L21
   - Live debugging tic tac toe driver 

Week 12 | 18 August 2024
--

Assignment due: [F1](assignments/F1.md)

Monday:
- L22
   - Cont. Live debugging tic tac toe driver

Wednesday:
- L23
   - Guest lecture maybe

Notes & Ideas
--

TBA: Final presentation design and plan

TBA: chroot demo? and if so where & when?

TBA: Demo qemu/kvm gdb debugging [documentation](https://www.kernel.org/doc/html/latest/dev-tools/gdb-kernel-debugging.html)
