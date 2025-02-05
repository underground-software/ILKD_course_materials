## Style Guide

### Title

  * Each document's first line is a `## Title` so that radius can grab it for the page's HTML `<title>` tag

### Paragraphs

  * Correct grammar and punctuation

### Bullet-point lists

  * Initial cap

  * <= 1 sentance

  * No period at the end

  * More than one sentance should be more than one bullet, or a paragraph

### Headings

  * Title capitalization

### Markdown rules

  * separate bullets by a blank line

  * when splitting single paragraphs/bullets try to find

  * a natural breaking point, but prefer avoiding jagged

  * edges to breaking strictly on clauses or something

  * spell out a number if it less than 20

  * larger numbers potentially if they are one word (i.e. sixty but 65)

  * top level headings within document should be at most `##` and decrease from there

  * avoid unnecessary blank lines

### Abbreviations

  * Avoid abbreviations as much as possible

  * KDLP is approved to general use but it is preferrable to spell out Kernel Devlopment Learning Pipeline at least once per page

  * ILKD is acceptable but better spell out Introduction to Linux Kernel Development

  * USI should only be used internally and Underground Software or Underground Software Incorporated should be preferred in any public document

### Links

  * Use relative links to a file in the same directory

  * Use absolute links in any other case

  * Images and other static content should be stored in and loaded from kdlp.underground.software

### Other

  * Prefer "Matrix" over "matrix" when referring to the Matrix protocol

  * Text used as a placeholder to be substituted should be formatted like shell variable dereference syntax, i.e. `$PLACEHOLDER`

  * Exception to previous rule: within command descriptions `<description>` should be preferred to describe parameters

  * Oopsie is a proper noun

  * Use makefile in prose and `Makefile` in filenames
