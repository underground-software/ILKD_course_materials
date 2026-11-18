## Style Guide

### Paragraphs

* Correct grammar and punctuation

### Bullet-Point Lists

* Initial cap

* At most one sentence

* No period at the end

* More than one sentence should be more than one bullet, or a paragraph

### Headings

* Title capitalization

* Top level headings within a document should be at most `##` and decrease from there

### Markdown Rules

* Separate bullets by a blank line

* When splitting single paragraphs or bullets across lines, try to find
a natural breaking point, but prefer avoiding jagged edges
to breaking strictly on clauses

* Spell out a number if it is less than twenty

* Spell out larger numbers if they are one word (i.e. sixty but 65)

* Avoid unnecessary blank lines

### Commit Messages

* Add a `Signed-off-by` line matching the commit author with `git commit -s` to certify the [Developer Certificate of Origin](https://developercertificate.org)

* Write a title of at most 72 characters, optionally prefixed with an area such as `setup: `

* Start the title with a capital letter in the imperative mood ("Fix typo", not "Fixed typo")

* Do not end the title with a period

* Leave a blank line between the title and the body

* Wrap the body at 75 characters, except for URLs and indented code or logs

* Start sentences and list items in the body with a capital letter

* Use correct spelling and grammar

### Automated Checks

Pull requests and pushes run these checks in CI:

* `scripts/check_commits.py` checks every new commit message, using `codespell` for spelling

* `scripts/check_style.py` checks the Markdown lines a change touches

* `git diff --check` catches whitespace errors

Run them locally before sending a change:

```
pip install codespell
python3 scripts/check_commits.py origin/2026_fall_uml..HEAD
python3 scripts/check_style.py --diff-base origin/2026_fall_uml
```
