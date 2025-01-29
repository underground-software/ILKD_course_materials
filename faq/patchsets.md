## Patchset Guidelines

The specific assignment document will specify which files to edit, and how many commits
you should be making which will inform the overall structure of your patchset, but every
patchset in this class must follow these general guidelines:

* Each [commit](commits.md) gets its own [patch](patches.md) with a title and body

* The patch series is introduced with one additional patch, the [cover letter](coverletters.md)

Fortunately, git format-patch can generate the appropriate files for you:

```shell
$ git format-patch -3 --cover-letter --rfc -v1
```

This command generates git email patches from a base repository. The arguments mean the following:

`-3` specifies that the three most recent commits should be included, and therefore 3 email patch files will be generated. Change the number as needed.

`--cover-letter` specifies that a [cover letter](coverletters.md) email template file is generated as "patch 0" of the patchset. You must always use this.

Use `-v<n>`, in this case `-v1` to specify the version number of this patchset.
Increase this number each time you resubmit an assignment. Use `--rfc` to denote
on each patch that the changes are a draft posted for review. Use this when generating
initial submissison patchsets, but not when generating the final submission patchsets.

All of the patches must follow the
[patch guidelines](patches.md).
Commits, from which the patches are generated, should follow the
[commit guidelines](commits.md)
and the cover letter must follow the
[cover letter guidelines](coverletters.md)

You will receive an automatic zero on the assignment if any of the patches
in your patchset are corrupt. A patch is considered corrupt if it does not apply.

Always test to see if your generated patchset applies cleanly before submission.
Generating corrupt patches shouldn't be possible if you use `git format-patch`, but
if you edit the files manually they might get corrupted. You have been warned!
The correct way to edit patches is to edit the underlying commits and then regenerate the patchset.

Note that version numbers are maintained between RFC and non-RFC patchsets--in the best case,
your RFC would be v1 and your final submission would be v2.
Your cover letter shoud include a summary of changes since the previous version.
To edit previous commits, see `man git-rebase` and the `--amend` option from `man git-commit`.
