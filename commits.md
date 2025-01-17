## Commit Guidelines

Within the submissions repository for this course each student will create a directory matching
their username. If any starter files are needed, the assignment descriptions will specify the
necessary details. You will do the assignment and turn your work into commits using `git commit -s`.

When you author a commit the first line(s) you type into your editor will become the title,
and by hitting enter twice and leaving a blank line the subsequent text will become the full
commit message. The `git format-patch` utility will automatically put the title and message
of a commit into the respective title and body of the corresponding generated email patch file.

The commit title should include the assignment prefix followed by a colon and a space,
then a short summary of the changes in this commit in the imperative tense.
e.g. `setup: Add username/setup.txt`. Any further details should be included in the commit message.

You should make sure that the changes you are including in your commits are tidy.
This means that code should follow the
[kernel code style guidelines](https://www.kernel.org/doc/html/latest/process/coding-style.html),
(tabs for indentation, tab width of 8, no lines exceeding 100 columns, etc).

You must also avoid whitespace errors. These include whitespace at the end of a line,
lines with only whitespace on them, extra blank lines at the end of a file, forgetting
the newline on the last line of the file, etc. A good editor will highlight and/or automatically
fix these for you, but git will also detect these when formatting and applying patches.

You can check for the other whitespace errors by using `git am <email patch file>` to attempt
to apply your patch to the local tree. If `git am` prints a warning like this when you apply the patch:

```
warning: 2 lines add whitespace errors.
```

You should adjust the indicated lines and fix your patch.

Your patches also must apply cleanly to the `HEAD` commit on the `master` branch of the
upstream repository. You can verify this by checking out that branch and trying to apply
your patches. We should NOT need to apply your previous versions of the patchset in order
for the latest version of your patchset to apply.

Sample workflow to check that your patchset applies cleanly:

* Generate your patches and put them in a known location and take note of the filenames

* Make sure your local git worktree is up to date
(you should do this each time you begin work within any git repository)

  * Use `git remote update` to update all of your local copies of remote trees

* Create and checkout a local branch based on the upstream `origin/master` branch by using:

  * `git checkout -b <branch name> origin/master` (branch name can be anything convenient)

* Apply your patchset to this branch using `git am <patch1> <patch2> ... <patchN>`

* If there are no errors that appear, congratulations, your patchset applies cleanly!

  * If there are whitespace errors or corrupt patches, revise as needed by amending or rebasing your commits

