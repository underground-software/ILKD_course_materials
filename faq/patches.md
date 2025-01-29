## Patch Guidelines

Assignments must be submitted in the format of git email text patches grouped into
[patchsets](patchsets.md).

Binary patches are forbidden because all work in this class is in plaintext.

---

Every patch in the patch series (including the cover letter) must end with
a "Signed-off-by" line, called the DCO (Developer Certificate of Origin).
The line must exactly match this format:

```
Signed-off-by: $FIRSTNAME $LASTNAME <$USERNAME@spring2025-utsa.kdlp.underground.software>
```

The DCO line must be the final line of the email body right before the start of the patch diff,
or in the case of the cover letter, before the start of the patchset summary and diffstat output.

Fortunately, you can make git add this line automatically for you when you author a commit:

```
$ git commit -s
```

This will add the DCO line in the commit message automatically from information
in the current user's git config, or if present the repository's `.git/config` file.

You will need to remember to add your DCO to the cover letter manually.
