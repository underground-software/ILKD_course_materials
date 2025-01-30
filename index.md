## Introduction to Linux Kernel Development: University of Texas San Antonio Spring 2025

| Week # | Class dates | Useful information |
| -- | -- | -- |
| 0 | 21/23 Jan | syllabus/policies/setup [L00](lectures/L00.md) |
| 1 | 28/30 Jan | [setup due](setup.md) barebones Linux [L01](lectures/L01.md) [L02](lectures/L02.md)|
| 2 | 04/06 Feb | [new_syscall due](new_syscall.md)|
| 3 | 11/13 Feb | |
| 4 | 18/20 Feb | |
| 5 | 25/27 Feb | |
| 6 | 04/06 Mar |
| 7 | 11/13 Mar | SPRING BREAK: no class |
| 8 | 18/19 Mar | |
| 9 | 25/27 Mar | |
| 10 | 01/03 Apr | |
| 11 | 08/10 Apr | |
| 12 | 15/17 Apr | |
| 13 | 22/24 Apr | |
| 14 | 29/01 Apr | |
| 15 | 06/08 May | |

#### [Frequently Asked Questions](faq.md)

### Course Summary and Purpose

We guide students into the depths of the Linux kernel, introducing the concepts, tools, and frameworks
necessary to become independent contributors to the Linux kernel in particular and open source software more broadly.

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

* Develop Linux kernel internals knowledge, by:

    * Building up an understanding up of various kernel subsystems

    * Creating, developing, and testing interesting and creative Linux kernel modules

    * Stretching their C programming skills to the limit

* Develop the ability to autonomously participate in the open source software community

* Demonstrate the ability to explain technical topics in your own words

* Learn how to give and receive feedback on code patches

### Required Materials

* A computer that can run [`podman`](https://podman.io/)

* Patience, determination, and an ability to focus on a potentially frustrating task for sustained periods of time

* The ability to join a live video call with camera/microphone from that computer
  (i.e. the necessary equipment and a stable internet connection)

### Attendance Policy

Attendance is required.

### Late Work Policy

Late work will not be accepted.

### Automatic Course Failure

If a student fails to complete a task by a deadline or fails to attend class with no prior notice,
an instructor will reach out to you via direct message on matrix.

If we do not hear from the student within one week of an instructor reaching out,
we reserve the right to give the student a final course grade of F.

### Grading Breakdown and Letter Grade Conversion

| Category | Percentage |
|--|--|
| Assignments                           | 50% |
| Midpoint Project                      | 15% |
| Final Project                         | 35% |

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
