## Introduction to Linux Kernel Development Fall 2024

### Course Summary and Purpose

We aim to introduce students to the concepts, tools, and framework necessary for independent work on the Linux kernel.

### Course Description

Linux is the most widely used operating system in the world.
The core software component of the Linux operating system is the kernel.
A couple of its roles include managing hardware interactions,
virtualizing system resources, and enforcing security constraints.
In effect, Linux kernel powers almost all of the world’s top supercomputers,
android phones, and an innumerable variety of other computers.
This course will introduce students to Linux kernel development
by focusing on device driver development.
This will give students hands-on experience working with internal Linux kernel APIs
and provide an overview of some of the core features and components of the kernel.
Gaining an understanding of the inner workings of the operating system
and how to make changes to it will give students an invaluable perspective
on how their computers work behind the scenes that will reveal
a new layer of understanding to apply to any future software engineering practice.

### Course Objectives

Students will:

* Stretch their C programing skills to the limit

* Create, develop, and test interesting and creative Linux kernel modules

* Develop the ability to autonomously participate in the open source software community

* Learn how to give and recieve feedback on code patches

* Demonstrate the ability to explain your code and your knowledge of Linux in your own words

### Required Materials

* A computer that can run a virtual machine to give you access to a Linux environment.

* Patience, determination, and an ability to focus on a potentially frustrating task for sustained periods of time.

### Coursework Overview

TODO: tense stuff

**Coding Assignments**

* Students will complete a coding assignment about once a week

* Assignment submissions are git patches formatted for email. 

* Assignments fall into three categories: exercises, programs, and the final project

* Exercises are about practicing skills and learning by doing.

  * Students will follow a series of instructions to execute certain
    commands, and answer questions about what you did.

  * A submission will be proof that you completed the steps
    correctly, and your answers to the questions.

  * As a general rule exercises will not involve much programming,
    and are weighted lesser in your overall grade.

* Programs are about getting your hands dirty and actually writing some code.

  * Students will have specific requirements your code has to meet,
    and a way to test that what you have written completes the requirements it successfully.

  * Your submission will consists of proof that your program works correctly, and its source code.

  * Since programs involve more actual coding, they will be weighted more in your overall grade.

* Students will review two other's submissions as a part of every assignment

**Presentation Assignments**

* Students will be required to complete two (2) presentations throughout the course

* Students must deliver their presentations over a live video call

* Presentations must include original visual content, such as slides

* Students are required to include live demonstrations of their code by sharing their text editor and/or terminal during the presentation

### Grading scheme:

Student grades will be weighted according to the following:

| Category | Percentage |
|--|--|
| Programs | 30% |
| Exercises | 20% |
| Midpoint Presentation | 15% |
| Final Assignment + Final Presentation | 30% |
| Participation | 5% |


Students will be given a letter grade according to the following this scheme:

|Percentage range|Letter grade|Grade points|
|--|--|--|
|90 ~ 100|A |4.0|
|85 ~ 89.99|A-|3.7|
|80 ~ 84.99|B+|3.3|
|75 ~ 79.99|B|3.0|
|70 ~ 74.99|B-|2.7|
|65 ~ 69.99|C+|2.3|
|60 ~ 64.99|C|2.0|
|55 ~ 59.99|C-|1.7|
|50 ~ 54.99|D+|1.3|
|40 ~ 49.99|D|1.0|
|below 40|F|0.0|


## Course Policies

### Attendance Policy

TODO
* Attendance is required.

### Automatic Course Failure (Grade of F)

* In the case that a student fails to complete a task by a deadline with no prior notice,
an instructor will reach out to you via email and/or direct message.
If the student fails to respond to this message within seven (7) days of it’s receipt,
the instructors reserve the right to give the student a failing grade (F) for the course.


### Assignment Policies

* Instructors reserve the right to change due dates within reason
  * In such a case, we will provide sufficient advance notice to students
* There is no late work!
  * Students receive a zero for assignments not submitted on time.
* One exception (a once per semester get out of jail free card):
  * Students may choose to opt out of the peer review process.
  * By bypassing the ack requirement (both giving and receiving) students receive the two days that would be used for that process as an extension before they make their first and only final submission.
  * This means that you do you not get a chance to receive early feedback from your peers and so you get what you get for a final grade.


**Peer Review Procedure (only for coding assignments)**

* Students must participate in the class mailing list for submissions by giving other students feedback on their work.
* This is a part of each assignment grade and the tripartite scheme is as follows:
  * The review process spans 2 days.
  * Part 1: Initial submission
    * The student makes their submission to the mailing list using git-send-email.
    * This submission is due on the listed assignment "due date", see the [course schedule](course_spring2023.html).
    * If the initial submission is late, the student will get a zero on the entire assignment.
  * Part 2: Peer review
    * A student is given two other student names (call them student A and student B).
    * The student is assigned to review student A's and student B's submissions.
    * If the student approves of a submission, then the student will reply to the approved email with what we call an "ack".
      * An "ack" consists of a single line containing the following: Acked-by: Firstname Lastname <email@domain.tld\>.
    * If the student sees problems with a submission, the student will reply to the problematic email with their feedback.
    * In parallel, other students have been assigned the student's submission and the student should recieve feedback from two other students.
    * These reviews are due one day past the initial submission, a late or no submission results in a zero for the review section of the assignment.
    * Reviews are graded based on how many issues a student missed.
      * The student receives 20% off for each unique issue not spotted with max penalty of 100%.
  * Part 3: Final submission
    * The student, if canny, will act on the feedback from the received reviews.
    * Regardless of whether the student made changes to their initial submission, they must make a final submission.
    * This is due two days after the initial submission, and due one day after receiving the reviews.
  * Overall assignment grading scheme (percentages are relative to the total assignment grade):
    * The initial submission is purely for the peer-review process. It is crucial that the initial submission is made on time, otherwise, as stated above, the student will recieve a zero on the entire assignment.
    * Review 1: 10%
    * Review 2: 10%
    * Final Submission: 80%


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
provided they made at lease one initial submission.
The grade students receive on the content of the final submission will comprise 80% of their assignment grade.

### Assignment Submission Guidelines

TODO:

    * Submissions must conform to all Linux kernel coding standards that are sane in a particular context.
    * Submissions must not contain whitespace errors
    * Submissions must not container binary blobs


  * Assignments must be submitted in the format of plaintext unless explicitly specified.
  * The instructors will provide detailed guidelines for automated checking of these requirements, and will gladly explain them upon request to any interested student
    * Therefore the instructors reserve the right to refuse to grade any submission that does not conform to these standards.
  * Any code submission that fails to compile with zero warnings and zero errors will receive a zero.
All homework submissions must be submitted in the form of an email patchset [0] generated by git format-patch [1] from commits [2] made in your local copy of [this repository](http://prod-01.kdlp.underground.software/cgit/KDLP_assignments.git/) with a cover letter [3] describing your work, and sent by git send-email [4] to our mailing list.

As part of the peer-review process for coding assignments in this class (described [here](course_policies.md)) each assignment will require you to submit each patchset at least twice [5].

Try to start your assignments early. If you run into issues and get stuck it gives you time to ask questions and get help before the due date so you can submit something on time and get credit for the assignment, and if you finish early you can submit and potentially get feedback from us or other students that you can incorporate into a resubmission before the deadline in accordance with the resubmission policy [6].

#### [0] Patchset Guidelines

The specific assignment document will specify which files to edit, and how many commits you should be making which will inform the overall structure of your patchset, but every patchset in this class must follow these general guidelines:

* Each commit gets its own patch with a title and body

* The patch series is introduced with one additional patch, the cover letter

Fortunately, git format-patch can generate the appropriate files for you automatically:

$ git format-patch -3 --cover-letter

$ # generates patches from the three most recent commits (change -3 to whatever you need for the amount of commits you have) and a cover letter template.

All of the patches must follow the patch guidelines [1]. Those generated from a commit should follow the commit guidelines [2] and the cover letter must follow the cover letter guidelines [3].

You will recieve an automatic zero on the assignment if any of the patches in your patchset are corrupt. This shouldn't be possible if you generate your patches with `git format-patch`, but if you edit the files manually they might get corrupted. You have been warned! The correct way to edit the patches is to edit the underlying commits (see `man git-rebase` and the `--amend` option from `man git-commit`) and then regenerate the patches. 

#### [1] Patch Guidelines

Every patch in the patch series (including the cover letter) must end with a “Signed-off-by” line, called the DCO (Developer Certificate of Origin). The line must exactly match this format:

Signed-off-by: Firstname Lastname

The DCO line must be the final line of the email body right before the start of the patch diff.

Fortunately, you can make git add this line automatically for you when you author a commit:

$ git commit -s

$ # include a DCO Signed-off-by line in the commit message automatically

You will need to remember to add your DCO to the cover letter manually.

You should use the checkpatch.pl script in the scripts/ directory of Linus’ kernel tree to make sure your patch is as close to upstream Linux kernel patch standards as possible.

#### [2] Commit Guidelines

Within the repository for this class there is a directory for each assignment (e.g. for A0, look in the A0 directory, for Mid 1, look in Mid1, etc.) and within that directory there should be everything you need to get started after you have read the specific document for the assignment you are working on.

You will make the necessary changes or additions and turn them into commits using git commit.

When you author a commit the first line(s) you type into your editor will become the title, and by hitting enter twice and leaving a blank line the subsequent text will become the full commit message. The git format-patch utility will automatically put the title and message of a commit into the title and body respectively of the corresponding patch email it generates.

Your commits should have a title that is a short summary of the changes in this commit and you should include any further details in the commit message.

You should make sure that the changes you are including in your commits are tidy. This means that code should follow the [kernel code style guidelines](https://www.kernel.org/doc/html/latest/process/coding-style.html), (tabs for indentation, tab width of 8, no lines exceeding 80 columns, etc).

You must also avoid whitespace errors. These include whitespace at the end of a line, lines with only whitespace on them, extra blank lines at the end of a file, forgetting the newline on the last line of the file, etc. A good editor will highlight and/or automatically fix these for you, but git will also detect these when formatting and applying patches.

When you format a patch, if you forgot the newline at the end of the file git will put this line at the end of the diff \ No newline at end of file. If you see this, you should adjust the file contents and fix your patch. You can check for the other whitespace errors by running git am to try and apply your patch. If git am prints a warning like this when you apply the patch: warning: 2 lines add whitespace errors. You should adjust the indicated lines and fix your patch.

Your patches also must apply cleanly to HEAD commit on the master branch of the upstream repository. You can verify this by checking out that branch and trying to apply your patches. We should NOT need to apply your previous versions of the patch in order for the latest version of your patchset to apply.

Sample workflow to check that your patchset applies cleanly:

* Generate your patches and put them in a known location and take note of the filenames
* Make sure your git tree is up to date. You should do this each time you begin work within any git repository.
  * Use `git remote update` to update all of your local copies of remote trees.
* Create and checkout a local branch based on the upstream `origin/master` branch by using:
  * `git checkout -b <branch name> origin/master` (branch name can be anything convenient)
* Apply your patchset to this branch using `git am <patch1> <patch2> ... <patchN>`
* If there are no errors that appear, congratulations, your patchset applies cleanly!
  * If there are whitespace errors or corrupt patches, revise as needed by amending or rebasing your commits.

#### [3] Cover Letter Guidelines

When you open the cover letter file generated by git format-patch in your editor, it will contain a summary of all the changes made in the subsequent patches at the bottom and two filler lines indicating where you can add the title (\*\*\* SUBJECT HERE \*\*\*) and message (\*\*\* BLURB HERE \*\*\*) to the email.

* You must replace the subject filler With your name and the assignment title. For example Charles Mirabile - Assignment 3

* You must replace the blurb filler with your writeup

* Failure to remove these filler lines including the asterisks will result in lost points

Your cover letter writeup should include a short write-up that specifies the following:

* What you think your degree of success with this assignment is (from 0% to 100%)

* A brief discussion of your approach to the assignment

* A detailed description of any problems that you were not able to resolve before submitting.

Failure to specifically provide this information will result in a 0 grade on your assignment. Further, If you do not disclose problems in your write-up and problems are discovered when your work is reviewed, you will receive a grade of 0. Note: These writeup guidelines are adapted from those of [Prof. Bill Moloney](https://cs.uml.edu/~bill/)

#### [4] Submitting to the Mailing List

Your patches should be sent to the address for the specific assignment. Each assignment will list the appropriate email to use, but in general e0 -> exercise0@kdlp.underground.software, p0 -> programming0@kdlp.underground.software f0 -> final0@kdlp.underground.software and likewise for the subsequent numbers.

$ git send-email --to=whatever_assignment@kdlp.underground.software *.patch

$ # send the emails to the mailing list. The list of patch files are passed as arguments

$ # The *.patch wildcard expands to all of the files whose names end with .patch in the current directory

#### [5] Peer Review Submission Process

The patch format should be as follows:
* Send rfc patches for the submissions that are to be peer review (add the `--rfc` option when you format your patchset).
* Send non-rfc patches for the final submission (omit the `--rfc` option).

#### [6] Late Assignment Submissions

Late submissions will result in an automatic grade of zero. The only exception is the one-time per semester oopsie (refer [here](course_policies.md)).

#### [7] Resubmissions

If you notice a mistake in your submission (or have it pointed out to you by one of the instructors or another student) before the due date, you are welcome to resubmit as many times as you'd like until said deadline. If you choose to resubmit, you must regenerate the patches from your (potentially altered) commit(s), and send the full patch series as your resubmission (i.e. a cover letter and one email for each commit even if some of the emails are identical to what you sent before).

Further, you must make clear that it is a resubmission by adding a version number after PATCH in the square brackets (i.e. Subject: [PATCH x/n] My title here would become Subject: [PATCH v2 x/n] My title here) incrementing the number as needed for however many resubmissions you make (you can use the `-v` option with an argument, denoting the version number, when reformatting your patchset).

You must also document what changed since the last submission in your writeup, include a section with a title like “changes since vN” where N is the number of your last submission and you explain what changed.

### How to submit a code review

* You will be assigned two students to peer review shortly after you complete your own initial submission.
* For each student, which you are assigned, you will do the following:
  * Locate their submissions and download all of the patches in the patchset (besides the cover letter).
  * Make a new branch off of master/main and switch over to that branch to apply the patches.
  * Make sure each patch applies cleanly IN ORDER (a.k.a no corrupt patches, whitespace errors, etc.)
    * You can run checkpatch.pl on the patches, ignore any errors pertaining to maintainers.
  * Make sure the code compiles without warnings or errors.
  * Make sure the program runs without crashing.
  * Make sure that the output is correct (passes all tests, if applicable).
  * If there are any problems with the submission, document said issues and report them in your email reply to the student who you are reviewing.
  * If you determine that there are no issues with the submission, inform the recipient.
    * If it turns out that there were issues with the submission that you missed, points will be deducted from your overall assignment grade.
  * Refer [here](course_policies.md) for the grading policy regarding peer reviewing.

### Presentation Guidelines 🎯 🏹

Duration: Tentative till further notice. Midterm will be a bit shorter, final a bit longer, neither longer than 20m

<br></br>

Content:

What have you learned so far by doing the assignments?

- How did the assignments relate to what we learned in class in your own words

What challenges have you faced and overcome working on these tasks?

- Your assignment write ups should be useful here

Assignments you think you did an exceptional job on

- Why do you think so? Maybe demonstrate your competence/understanding

What assignments did you enjoy or not enjoy and any feedback

<br></br>

Suggestions on what to prepare:

- Visual demonstrations if you think they are helpful

Live example of how your project functions

- Could do something in the terminal live

Maybe show your program’s inner workings

- Explain your thought process

<br></br>

The presentation should demonstrate that you understand your work, you are able to explain what you did and why it matters to others, and you are able to show your thought process when you are faced with a problem.

An example of a previous presentation can be found here.

These are general guidelines for what we are looking for in a presentation. You do not need to follow it strictly but you should try to cover all of the areas at least somewhat.

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
   - Worked example, what are processes, what are syscalls, what are file descriptors *TBA: design this content*

Thursday:
 - L06
   - Basically the OS assignment where you run grep in a child process <https://cs.uml.edu/~bill/cs308/Assignment_2.pdf>
   - open read write fork exec dup pipe wait close exit

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


L08 import TODO

E0 add modify linux_banner in init/version-timestamp.c
