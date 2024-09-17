## Introduction to Linux Kernel Development: University of Massachusetts Lowell - Fall 2024

### Course Summary and Purpose

We aim to introduce students to the concepts, tools, and framework necessary for independent work on the Linux kernel,
and more broadly to the skills necessary for work as a professional open source software engineer.

### Course Description

Linux is the most widely used operating system in the world.
It powers the world's top supercomputers, every Android phone,
and the servers that host the internet's most popular websites.
At the core of Linux is a software component called the kernel.
It performs critical roles including managing communication with
hardware, sharing system resources equitably, and enforcing
security constraints.
This course will introduce students to Linux kernel development
through hands-on experience working with internal Linux kernel
APIs and providing an overview of some of the core features
and components of the kernel.
Students will gain an invaluable perspective on how their computers
work behind the scenes. By grasping the inner workings of the operating
system and how to make changes to it, they will add a new layer of
understanding to their mental model of computation that will pay
dividends throughout their future in software engineering.

### Course Objectives

Students will:

* Stretch their C programming skills to the limit

* Develop the ability to autonomously participate in the open source software community

* Learn how to give and receive feedback on code patches

* Demonstrate the ability to explain your code and your knowledge of Linux in your own words

* Create, develop, and test interesting and creative Linux kernel modules

* Extend their Linux kernel internals knowledge

### Required Materials

* A computer that can run a virtual machine to give you access to a Linux environment

* Patience, determination, and an ability to focus on a potentially frustrating task for sustained periods of time

* The ability to join a live video call with camera/microphone from that computer
  (i.e. the necessary equipment and a stable internet connection)

### Coursework Overview

#### Coding Assignments

* Students will complete a coding assignment about once a week

* Assignment submissions are git patches formatted for email

* Assignments fall into three categories: exercises, programs, and the final project

* Exercises are about practicing skills and learning by doing

    * Students will follow a series of instructions (e.g. to execute certain
    commands), and answer questions about what they did

    * A submission will be proof that the student completed the steps
    correctly, and their answers to the questions

    * As a general rule exercises will not involve much programming,
    and are weighted lesser in the overall course grade

* Programs are about getting your hands dirty and actually writing some code

    * Students will have specific requirements their code has to meet, and a way
    to test that what they have written completes the requirements successfully

    * A submission will consists of proof that the program works correctly, and its source code

    * Since programs involve more actual coding, they will be weighted more in the overall course grade

* The final project is broken into two parts

    * The students will have freedom to engineer a creative project

    * However, there is an emphasis on designing rigorous specifications and tests to clarify and enforce that vision

    * The student will implement a peers specification instead of their own

* As a part of every assignment (except the very final one) students will perform peer review

    * Students will be assigned two others' submissions to review

    * Part of the grade on each assignment will be determined by the quality and thoroughness of the review

#### Presentation Assignments

* Students will be required to complete two presentations throughout the course

    * Students must deliver their presentations over a live video call (with a camera)

    * Presentations must include original visual content, such as slides

    * Students are required to include live demonstrations of their code by sharing their screen during the presentation

### Grading Details

#### Category Weights

| Category | Percentage |
|--|--|
| Programs                              | 30% |
| Exercises                             | 20% |
| Midpoint Presentation                 | 15% |
| Final Assignment + Final Presentation | 30% |
| Participation                         |  5% |

#### Letter Grade Conversion

|Percentage range|Letter grade|Grade points|
|--|--|--|
|90 ~ 100  |A |4.0|
|85 ~ 89.99|A-|3.7|
|80 ~ 84.99|B+|3.3|
|75 ~ 79.99|B |3.0|
|70 ~ 74.99|B-|2.7|
|65 ~ 69.99|C+|2.3|
|60 ~ 64.99|C |2.0|
|55 ~ 59.99|C-|1.7|
|50 ~ 54.99|D+|1.3|
|40 ~ 49.99|D |1.0|
|below 40  |F |0.0|

## Course Policies

### Attendance Policy

* Attendance is required in person or via the class video call

* The instructors will be joining remotely

* Students are welcome (and encouraged) to join the meeting as well so they can contribute more easily

### Automatic Course Failure

If a student fails to complete a task by a deadline with no prior notice,
an instructor will reach out to you via email and direct message on matrix.
If the student fails to respond to this message within seven days of its receipt,
the instructors reserve the right to give the student a final course grade of F.

### Due date Policies

During a week where an assignment is due,
the students must make two submissions and complete two peer reviews in three stages as follows:

1. By 23:59 Tuesday, a student must make at least one submission.
This is the "initial submission" stage.
The student will not be graded on the content of this submission,
however the student will receive a 0 for the assignment if they do not submit anything at all.
Students may make as many resubmissions as they please,
and only the last submission made before the initial submission deadline
will be distributed as the target of peer review.
Students will review each other's initial submissions in the following stage.

2. By 23:59 Wednesday, a student must reply by email to two other students' initial submissions
providing feedback on anything wrong with the patchset
and any suggestions for the other student to improve their final submission.
This is the "peer review" stage.
Triggered by the event of the initial submission deadline, automation will make all students'
submissions visible to the whole class and assign each student two others to peer review.
Each peer review will comprise 10% of a students' assignment grade, for 20% of the total.

3. By 23:59 Thursday, a student must make at least one more submission for the assignment.
This is the "final submission" stage.
The wise students will take their peer's feedback and self-reflection into account and revise
their work before the final submission, however they are not required to do so.
Students can re-submit as many times as they please
and they will only be graded on the content
of last submission made before the deadline,
provided they made at least one initial submission.
The grade students receive on the content of the final submission will comprise 80% of their assignment grade.
Even if you are satisfied with your initial submission, you must make at least one final submission.
Failure to make at least one final submission will result in a grade of zero.

<br>

* Instructors reserve the right to change due dates within reason

    * In such a case, we will provide advance notice to students

    * If the due date is being pushed forward, the notification will occur at least one week before the new due date

* The instructors will not accept late work

    * Students receive a zero for assignments not submitted on time

* One exception (a once per semester get out of jail free card):

    * Students may choose to opt out of the peer review process for a given coding assignment

    * By skipping the process---both giving and receiving---students can use the two days that the process
    would normally entail to take more time to work on their final (and only) submission

    * Skipping peer review means forgoing the safety net of receiving early feedback from peers,
    so it is the sole responsibility of the student who chooses this path to ensure that their
    work is correct lest any defects in the submission negatively impact their final grade

Further details can be found on the [policies and procedures page](/procedures.md)

### Course Communication

* The primary location for course-related communication will be on the instructor-hosted matrix server

    * Students will be provided access credentials at the start of the course

    * These will be the same credentials students will use to access the course email patch submission system for assignments

    * Students who have general questions about the course material or any other non-private course-related information will
    post their questions in the #questions channel on matrix where discussion and answers will be available to all course participants

    * Students who have private questions about the course will reach out to a provided email address with their concerns

## Course Overview

|Week #|Start Date|Topic|Tuesday Lecture| Thursday Lecture|Assignment Due|
|--|--|--|--|--|--|
|00| 01 Sep|syllabus, course format, what is linux?	|                      |[L00](coming_soon.md)|                             |
|01| 08 Sep|commandline bootcamp, git intro		|[L01](coming_soon.md)|[L02](coming_soon.md)|                             |
|02| 15 Sep|makefiles, C compilation deep dive		|[L03](coming_soon.md)|[L04](coming_soon.md)|[setup](assignments/setup.md)|
|03| 22 Sep|file descriptors, processes, syscalls	|[L05](coming_soon.md)|[L06](coming_soon.md)|[E0](assignments/E0.md)      |
|04| 29 Sep|CPU priv., reentrency, kernel vs user code	|[L07](coming_soon.md)|[L08](coming_soon.md)|[P0](assignments/P0.md)      |
|05| 06 Oct|tracing how/when OS code runs on the CPU	|[L09](coming_soon.md)|[L10](coming_soon.md)|                             |
|06| 13 Oct|midpoint pres, what a good pres. looks like	|monday schedule       |[L11](coming_soon.md)|[Midpoint Presentation](coming_soon.md)|
|07| 20 Oct|assembly refresher, syscalls: end to end	|[L12](coming_soon.md)|[L13](coming_soon.md)|[E1](coming_soon.md)      |
|08| 27 Oct|kernel pseudo filesystems; kernel modules	|[L14](coming_soon.md)|[L15](coming_soon.md)|[P1](coming_soon.md)      |
|09| 03 Nov|character devices, ioctl, file position	|[L16](coming_soon.md)|[L17](coming_soon.md)|[E2](coming_soon.md)      |
|10| 10 Nov|kernel locking support			|[L18](coming_soon.md)|[L19](coming_soon.md)|[P2](coming_soon.md)      |
|11| 17 Nov|misc. devices for simple chardevs		|[L20](coming_soon.md)|[L21](coming_soon.md)|[F0](coming_soon.md)      |
|12| 24 Nov|live debugging: broken char dev module	|[L22](coming_soon.md)|thanksgiving          |                             |
|13| 01 Dec|guest lecture; final presentations		|[L23](coming_soon.md)|[L24](coming_soon.md)|[F1](coming_soon.md)      |
|14| 08 Dec|final presentations				|[L25](coming_soon.md)|[L26](coming_soon.md)|                             |
|15| 15 Dec|final presentations; end of class		|[L27](coming_soon.md)|[L28](coming_soon.md)|[Final Presentation](coming_soon.md)|

For more information, please refer to our [detailed procedures and policies](procedures.md).
