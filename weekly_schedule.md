### Course Schedule

Week 00 | 1 September 2024
--

Assignment due: none

Thursday:
 - L00
   - Syllabus overview, who are we, linux crash course slides <https://kdlp.underground.software/course/slides/linux_crash_course.html>
   - Before next class watch video on cmdline basics <https://youtu.be/qcue__Z2n6w> and git <https://kdlp.underground.software/course/slides/git.html>

Week 01 | 8 September 2024
--

Assignment due: none

Tuesday:
 - L01
   - Working through setup process <https://youtu.be/Ub62duJEPhQ> live, student guided, hands on in the terminal

Thursday:
 - L02
   - Some sort of in class git workshop *TBA: need to design this*

Week 02 | 15 September 2024
--

Assignment due: [setup](assignments/setup.md)

 - L03
   - Build a distro, working with makefiles, configure scripts <https://kdlp.underground.software/course/fall2023/lectures/L04.md> <https://youtu.be/aAoJyUaA1ok>
 - L04
   - Write example C program + makefile <https://kdlp.underground.software/course/fall2023/lectures/L08.md>
   - Work through phases of C compilation, what is assembly vs machine code, what are object files, reveal all steps in makefile

Week 03 | 22 September 2024
--

Assignment due: [E0](assignments/E0.md)

Tuesday:
 - L05
   - Basically the OS assignment where you run grep in a child process <https://cs.uml.edu/~bill/cs308/Assignment_2.pdf>
   - open read write fork exec dup pipe wait close exit

Thursday:
 - L06
   - Worked example, what are processes, what are file descriptors, what are syscalls, what are signals?
   - Uncover the underlying linux facilities that these posix concepts map on to.

Week 04 | 29 September 2024
--

Assignment due: [P0](assignments/P0.md)

Tuesday:
 - L07
   - What is CPU privilege, trying to execute forbidden instructions, what happens and why <https://kdlp.underground.software/course/fall2023/lectures/L05.md>

Thursday:
 - L08
   - We are able to run forbidden stuff when we are writing kernel code <https://kdlp.underground.software/course/fall2023/lectures/L06.md>

Week 05 | 06 October 2024
--

Assignment due: none

Tuesday:
 - L09
   - What causes kernel code to run, exceptions, traps, interrupts <https://kdlp.underground.software/course/fall2023/lectures/L08.md>

Thursday:
 - L10
   - tracing kernel code with ftrace <https://kdlp.underground.software/course/fall2023/lectures/L07.md> *TBA: design specific demo for this*


Week 06 | 13 October 2024
--

the studies according to Monday on Tuesday

Discuss and assign: [Midpoint Presentation](assignments/mid_pres_guide.md)
Assignment due: none

Thursday:
 - L11
   - What do we want for presentations in this class, no exams means we have high expectations <https://kdlp.underground.software/course/fall2023/assignments/mid_pres_guide.md>


Week 07 | 20 October 2024
--

Assignment due: [E1](assignments/E1.md)

Tuesday:
 - L12
   - Write simple hello world assembly program, begin tracing how syscall instruction works <https://kdlp.underground.software/course/fall2023/lectures/L10.md> <https://kdlp.underground.software/course/fall2023/lectures/L08.md>

Thursday:
 - L13
   - Finish syscalls end to end, how do we get back to syscall in our userspace program <https://kdlp.underground.software/course/fall2023/lectures/L09.md>


Week 08 | 27 October 2024
--

Assignment due: [P1](assignments/P1.md)

Tuesday:
 - L14
   - playing around with /proc /sys /dev *TBA: design specific demo*  <https://kdlp.underground.software/course/fall2023/lectures/L13.md> <https://youtu.be/RwvSsDOGWXE>

Thursday:
 - L15
   - writing a hello world module <https://kdlp.underground.software/course/fall2023/lectures/L12.md> <https://youtu.be/Phh05oG4WHg>

Week 09 | 03 November 2024
--

Assignment due: [E2](assignments/E2.md)

Tuesday:
 - L16
   - kernel data types `struct file`, `struct inode`, `struct file_operations`, `dev_t`, etc <https://kdlp.underground.software/course/fall2023/lectures/L15.md> <https://youtu.be/KiKGc67107Y> <https://kdlp.underground.software/course/fall2023/lectures/L16.md> <https://youtu.be/L1VJ9kEZ2zY>
Thursday:
 - L17
   - advanced char dev operations, ioctl, lseek, etc *TBA: do we have stuff from an even earlier semester?*

Week 10 | 10 November 2024
--

Assignment due: [P2](assignments/P2.md)

Tuesday:
 - L18
   - Debug/fix userspace program with race conditions *TBA: need to design this demo*

Thursday:
 - L19
   - Add locking to P2 code


Week 11 | 17 November 2024
--

Assignment due: [F0](assignments/F0.md)

Tuesday:
 - L20
   - Convert P2 code to use a misc-device

Thursday:
 - L21
   - Live debugging tic tac toe driver 

Week 12 | 24 November 2024
--

Assignment due: none

Discuss and assign: [Final Presentation](assignments/final_pres_guide.md)|

Tuesday:
 - L22
   - Cont. Live debugging tic tac toe driver

Thanksgiving: no class on Thursday


Week 13 | 24 November 2024
--

Assignment due: [F1](assignments/F1.md)

Tuesday:
 - L23
   - Guest lecture:
     - Topic: ????

Thursday:
 - L24
   - Final presentations:
     - 3:30 - 3:53
     - 3:56 - 4:19
     - 4:22 - 4:45

Week 14 | 08 December 2024
--

Assignment due: none

Tuesday:
 - L25
   - Final presentations:
     - 3:30 - 3:53
     - 3:56 - 4:19
     - 4:22 - 4:45

Thursday:
 - L26
   - Final presentations:
     - 3:30 - 3:53
     - 3:56 - 4:19
     - 4:22 - 4:45

Week 15 | 15 December 2024
--

Assignment due: have a nice holiday

Exact weekdays unknown at this time

Someday:
 - L27
   - Final presentations:
     - Section I:
       - 3:30 - 3:53
       - 3:56 - 4:19
       - 4:22 - 4:45
       - 4:48 - 5:11
     - Section II:
       - 6:30 - 6:53
       - 6:56 - 7:19
       - 7:22 - 7:45
       - 7:48 - 8:11

Someday:
 - L28
   - Final presentations:
     - Section I:
       - 3:30 - 3:53
       - 3:56 - 4:19
       - 4:22 - 4:45
       - 4:48 - 5:11
     - Section II:
       - 6:30 - 6:53
       - 6:56 - 7:19
       - 7:22 - 7:45
       - 7:48 - 8:11
