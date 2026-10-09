# Assignment Guidelines

### Sections
* [Commit Guidelines](#commit-guidelines)
    * [Editing Commits](#editing-commits)
    * [Quick checklist for commits](#quick-checklist-for-commits)
* [Patchset Guidelines](#patchset-guidelines)
* [Patch Guidelines](#patch-guidelines)
    * [Checking your Patches](#checking-your-patches)
* [Cover Letter Guidelines](#cover-letter-guidelines)
    * [Quick checklist for cover letter](#quick-checklist-for-cover-letter)
* [Peer review Guidelines](#peer-review-guidelines)
    * [Peer review grading](#peer-review-grading)
* [Submitting Guidelines](#submitting-guidelines)
* [Why did I get a zero?](#why-did-i-get-a-zero)



### Commit Guidelines

Always commit using `git commit -s`. This will make sure the "Signed-of-by:" is automatically included. 

When you author a commit the first line(s) you type into your editor will become the title, and by hitting enter twice and leaving a blank line the subsequent text will become the full commit message. The commit title should include the assignment name followed by a colon and a space, then a short summary of the changes in this commit in the imperative tense. For example if the assignment is `setup` then one valid title would be `setup: Add setup.txt`. You should also include any further details about the commit message.

You should make sure that the changes you are including in your commits are tidy.
This means that code should follow the
[kernel code style guidelines](https://www.kernel.org/doc/html/latest/process/coding-style.html),
e.g. tabs for indentation, tab width of 8, and no lines exceeding 100 columns.

You must also avoid whitespace errors. These include whitespace at the end of a line,
lines with only whitespace on them, extra blank lines at the end of a file,
and forgetting the newline on the last line of the file.
A good editor will highlight and/or automatically
fix these for you, but git will also detect these when formatting and applying patches.

One way to find them is before you commit you run the command `git diff --staged --check`. This will check if there are any whitespace errors in your staged changes. Now let's suppose you have already committed like 5 times, and you want to make sure there are no whitespace errors before making the patches you can easily run the command `git diff --check HEAD~NUM_OF_COMMITS` to check for whitespace errors. `NUM_OF_COMMITS` here means number of recent commits to check backwards from the head. For example, using 5 would mean check the last 5 commits for any whitespace errors.

##### Editing Commits
To edit previous `N` commits, you can use `git rebase` and `git commit` in the following manner.

First, use the command ```git rebase -i HEAD~N``` 

This will start a rebase in interactive mode for previous `N` commits. It will open a file in text editor, in that file you will see `N` lines of `pick COMMIT_HASH COMMIT_TITLE`. There are instructions in the file for more detailed usage. But for our purpose the following should be enough. Replace `pick` with `e` if you want to edit files in that commit. Replace `pick` with `r` if you just want to change the commit title and body. Keep the `pick` if you don't want to change anything about that commit. After choosing what you want to do save the file and close it.

* If you chose `e` short for edit, you will be edit the commit. This means you can edit, add, or remove any files. When you are satisfied with the changes, stage them using `git add FILE_NAME`. After staging all changes, use the `git commit --amend`. This will amend the changes to the commit. After you are done with the current commit move to the next commit using the command `git rebase --continue`. Keep in mind `pick` commits will be skipped.
* If you chose `r` short for reword, you will be able to edit the commit title and message.

##### Quick checklist for commits

*   Must have a commit title
    * They should be in the format of `ASSIGNMENT_NAME: IMPERATIVE_ TENSE_SHORT_DESCRIPTION `
    * For example if the assignment is "setup" then `setup: Add setup.txt`
*   Must have a commit message
*   Must include a "Signed-off-by: ...." line at the end, use `git commit -s` to automatically include it
*   No white space errors
    * check by running `git diff --check HEAD~NUM_OF_COMMITS`


### Patchset Guidelines

Every patchset in this class must follow these general guidelines:

* Each gets its own patch with a title and body

* The patch series is introduced with one additional patch, the [cover letter](#cover-letter-guidelines)

Fortunately, git format-patch can generate the appropriate files for you---as an example:

```shell
$ git format-patch -NUM_OF_COMMITS --cover-letter --rfc -vN
```

This command generates git email patches from a base repository. The arguments mean the following:

`NUM_OF_COMMITS` specifies that the `NUM_OF_COMMITS` most recent commits should be included, and therefore `NUM_OF_COMMITS` email patch files will be generated. Change the number as needed for the individual assignment.

`--cover-letter` specifies that a [cover letter](#cover-letter-guidelines) email template file is generated as "patch 0" of the patchset. You must always use this.

Use `-vN`, where `N` is the version number of the patchest. For an initial submission you would use `-v1`. Increase the version number each time you resubmit an assignment. Note that version numbers are maintained between RFC and non-RFC patchsets--in the best case,
your RFC would be v1 and your final submission would be v2.

Use `--rfc` to denote on each patch that the changes are a draft posted for review. Use this when generating
initial submissison patchsets, but not when generating the final submission patchsets.

Always test to see if your generated patchset applies cleanly before submission. ([checking patches](#checking-your-patches))
Generating corrupt patches shouldn't be possible if you use `git format-patch`, but
if you edit the files manually they might get corrupted. You have been warned!
The correct way to edit patches is to edit the underlying commits and then regenerate the patchset. ([Editting Commits](#editing-commits))

### Patch Guidelines

Assignments must be submitted in the format of git email text patches grouped into patchsets. 

Patches with binary content are forbidden because all work in this class is expressible as plaintext.

Every patch in the patch series (including the cover letter) must end with a "Signed-off-by" line, called the DCO (Developer Certificate of Origin).

The line must exactly match this format:

```
Signed-off-by: $FIRSTNAME $LASTNAME <$USERNAME@fall2026-uml.kdlp.underground.software>
```
The DCO line must be the final line of the email body right before the start of the patch diff, or in the case of the cover letter, before the start of the patchset summary and diffstat output. If you made commits using `git commit -s` then DCO will automatically be included for commit patches. But you will need to remember to add your DCO to the cover letter manually.
 
You can check for the other whitespace errors by using `git am <email patch file>` to attempt
to apply your patch to the local tree. If `git am` prints a warning like this when you apply the patch:

```
warning: 2 lines add whitespace errors.
```

You must adjust the indicated lines in the original file, fix your commit ([editing commits](#editing-commits)), and regenerate the patches ([making patches](#patchset-guidelines)).

Your patches also must apply cleanly to the `HEAD` commit on the `master` branch of the [upstream submissions repository](https://fall2026-uml.kdlp.underground.software/cgit/ILKD_Submissions/). You can verify this by updating and checking out the `master` branch yourself and trying to apply your patches. ([applying patches](#checking-your-patches))

#### Checking your patches
Sample workflow to check that your patches applies cleanly:

0. Generate your patches and put them in a known location and take note of the filenames

0. Make sure your local git worktree is up to date

    * You should do this each time you begin work within any git repository

    * Use `git remote update` to update all of your local copies of remote trees

0. Create and checkout a local branch based on the upstream `origin/master` branch by using:

    * `git checkout -b <branch name> origin/master` (branch name can be anything convenient)

0. Apply your patchset to this branch using `git am <patch1> <patch2> ... <patchN>`

0. If no errors appear: congratulations, your patchset applies cleanly!

    * If there are whitespace errors or corrupt patches, revise as needed by rebasing and amending your commits then regenerate the patches. ([editing commits](#editing-commits), [generate patches](#patchset-guidelines))

### Cover Letter Guidelines

When creating a [patchset](patchsets.md), you will generate a cover letter template file.

When you open the cover letter file generated by `git format-patch` in your editor,
you will see a summary of all the changes made in the subsequent patches at the bottom
and two filler lines indicating where you can add the title `(*** SUBJECT HERE ***)`
and message `(*** BLURB HERE ***)` to the email.

You must:

* Replace the subject filler text with the assignment name and your username (whoami, you can see by scrolling all the way down on any page when logged in) separated by a dash `'-'`.
For example: `ASSIGNMENT_NAME - WHOAMI`

* Replace the blurb filler text with your write-up

Failure to remove these filler lines including the asterisks will result in lost points.

The first line of your cover letter write up should state what you think your degree of success for the assignment was (from 0% to 100%), formatted as follows:

```
Completed: 100%
```

Following this, your cover letter write-up should include a short discussion that includes the following:

* An estimate of how much time you spent working on this assignment

* How you approached the assignment

* A detailed description of any problems that you were not able to resolve before submitting


If your self-assessment of success is not 100%, please document all known issues. Be honest with your self-assessment. If obvious errors are discovered despite high self-assessment, then we reserve the right to give you a zero. We also reserve the right to dock points for grammatical mistakes and spelling errors.

Make sure to add a Signed-off-by line to the cover letter. It goes right at the end before the automatically generated summary of the patchset. If you want to be sure you have it in the right place, you can add it first putting it in place of the `*** BLURB HERE***` filler text and then write
the rest of your cover letter above it.

If you resubmit, you must regenerate and resend all the patch files and cover letter as a new version of the entire patch series, incrementing the version number in the subject line appropriately.

You must also document what changed since the last submission in your write-up, include a section with a title like "changes since vN" where N is the number of your last submission and you explain what changed.

##### Quick checklist for cover letter

* No `(*** SUBJECT HERE ***)` or `(*** BLURB HERE ***)` in the cover letter
* Replace subject filler with `ASSIGNMENT_NAME - WHOAMI`
* First line of the cover letter blurb (body) must be `Completed: COMPLETED_PERCENT`
* Cover letter blurb must include how much time you spent working on the assignment
* Cover letter blurb must include a detailed description of any problems that you were not able to resolve before submitting. If all problems were resolved still include a statement about it. The following should work "I was able to resolve all problems during development and have zero unresolved problems to report."
* If the patch version isn't one then you must include a section with a title like "changes since vN" where N is the number of your last submissions, and explain what changed. 
* Last line before auto generated summary about the patchset must be "Signed-off-by" line.

### Peer Review Guidelines

Each student is assigned two other students' work to review on their dashboard. For each student to whom you are assigned to perform peer review, you will do the following:

* Make a new branch off of master and switch over to that branch.

* Apply the student's latest patchset submission to your local tree

* Make sure each patch applies cleanly IN ORDER (a.k.a no corrupt patches, whitespace errors, etc.)

* Make sure the code compiles without warnings or errors

* Sanity check that any programs run without immediately crashing

* Make sure that the output looks reasonable

* Scan through the rubric, as each assignment's peer review requirements will be different

* If there are any problems with the submission, report them in your review

* If you determine that there are no issues with the submission, inform the recipient.

* If it turns out that there were issues with the submission that you missed, points will be deducted from your overall assignment grade

* In parallel, other students have been assigned the student's submission
and the student should receive feedback from two other students

If the student approves of a submission, then the student will reply to the cover letter
of the patchset with a single line containing the following:

```
Acked-by: $FIRSTNAME $LASTNAME <$USERNAME@fall2026-uml.kdlp.underground.software>
```

If the student finds issues with a submission, then the student will reply to the cover letter
of the patchset with detailed feedback about their concerns and conclude the email
with a single line containing the following:

```
Nacked-by: $FIRSTNAME $LASTNAME <$USERNAME@fall2026-uml.kdlp.underground.software>
```



#### Peer review grading: 

* These reviews are due 24 hours after the initial submission deadline

* A late or missing submission yields a zero for the review section of the assignment

* If a student does not complete peer review their maximum assignment grade is 80%

* Reviews are graded based on how many issues a student missed. The student receives 20% off for each unique issue not spotted with max penalty of 100%

* You can resubmit a peer review by making a new reply to the original cover letter with a new version of your complete review.

### Submitting guidelines

Your patches should be sent to the address for the specific assignment. Each assignment will list the appropriate email and the correct command will look something like:

```git send-email --to=ASSIGNMENT_NAME@fall2026-uml.kdlp.underground.software vN*.patch```

This command attempts to send any file in the current directory starting with v followed by version number of your patches and ending with .patch to the mailing list. For a first submission to an assignment N would be 1, keep in mind when resubmitting you have to increase version number and version numbers are maintained between initial and final submission. `v1*.patch` is an example of glob expansion in the shell. See `man 7 glob` for more information.


## Why did I get a zero?

The following could be a reason of why you get a zero on an assignment:

* Failure to cover estimate of time spent on assignment or how assignment was approached or description of problems that you weren't able to resolve (if don't have any problems to report then say so) will result in a zero grade on your assignment.

* If obvious errors are discovered despite high self-assessment of the assignment, then we reserve the right to give you a zero.

* We should NOT need to apply your previous versions of the patchset in order for the latest version of your patchset to apply. If this is the case, your patchset will be considered corrupt and you will receive a zero.

* Patches with binary content are forbidden because all work in this class is expressible as plaintext.

* If the initial submission is late, the student will get a zero on the entire assignment

* You will receive an automatic zero on the assignment if any of the patches
in your patchset are corrupt. A patch is considered corrupt if it does not apply.

