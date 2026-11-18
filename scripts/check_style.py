#!/usr/bin/env python3
"""Check Markdown course materials against the KDLP style guide.

The rules come from styleguide.md on the master branch and the whitespace
rules in the commit guidelines (faq/commits.md):

Errors (fail the check):
  whitespace       trailing whitespace, whitespace-only lines, missing final
                   newline, extra blank lines at the end of the file
  blank-lines      more than one blank line in a row ("avoid unnecessary
                   blank lines")
  heading-level    a `#` heading (top-level headings must be at most `##`),
                   or a heading that skips a level (`##` straight to `####`)
  bullet-spacing   list items not separated by a blank line
  bullet-cap       list item does not start with a capital letter
  bullet-period    list item ends with a period

Warnings (reported, never fail the check unless --strict):
  heading-case     heading does not look like Title Case
  bullet-sentences list item has more than one sentence
  small-number     number under 20 written as digits in prose

Usage:
  check_style.py [--diff-base REF] [--strict] [FILE ...]

With no files, every tracked *.md file is checked. With --diff-base, only
problems on lines added or changed since REF are reported, so existing
content does not block new contributions.
"""

import argparse
import os
import re
import subprocess
import sys

SMALL_WORDS = {
    "a", "an", "and", "as", "at", "but", "by", "for", "from", "if", "in",
    "into", "nor", "of", "off", "on", "or", "out", "over", "per", "so",
    "than", "the", "to", "up", "via", "vs", "vs.", "with", "yet",
}
ABBREVIATIONS = ("etc.", "e.g.", "i.e.", "vs.", "al.", "...")

FENCE_RE = re.compile(r"^\s*(```|~~~)")
HEADING_RE = re.compile(r"^(#{1,6})\s+(.*?)\s*#*\s*$")
LIST_RE = re.compile(r"^(\s*)([*+-]|\d+[.)])\s+(.*)$")
INLINE_CODE_RE = re.compile(r"`+[^`]*`+")
LINK_RE = re.compile(r"!?\[([^\]]*)\]\([^)]*\)|\[([^\]]*)\]\[[^\]]*\]")
URL_RE = re.compile(r"<?https?://\S+>?")
HTML_TAG_RE = re.compile(r"<[^>]+>")
SENTENCE_BREAK_RE = re.compile(r"[A-Za-z0-9)\"'][.!?]\s+[A-Z]")
ABBREVIATION_RE = re.compile(r"\b(?:e\.g|i\.e|etc|vs|cf)\.?(?=\s)", re.I)
# Numbers used as names or measurements rather than counts stay as digits:
# "Patch 1", "inode 0", "exit code 0", "version 5", "8 bytes", "10 ± 1".
NUMBER_LABEL_RE = re.compile(
    r"(?:patch|inode|sector|index|code|errno|pid|version|step|slide|plan|"
    r"lecture|chapter|section|page|width of|level|column|line|signal|"
    r"slot|±)\s+$", re.I)
NUMBER_UNIT_RE = re.compile(
    r"^\s*(?:bytes?|bits?|[KMGT]i?B\b|columns?|±|%)", re.I)
SMALL_NUMBER_RE = re.compile(r"(?<![\w.$/#:-])(1?\d)(?![\w./%:-])")


class Report:
    def __init__(self, changed):
        self.changed = changed  # {path: set(lines)} or None for everything
        self.errors = 0
        self.warnings = 0
        self.github = os.environ.get("GITHUB_ACTIONS") == "true"

    def add(self, path, line, rule, msg, warning=False):
        if self.changed is not None and line not in self.changed.get(path, ()):
            return
        level = "warning" if warning else "error"
        if warning:
            self.warnings += 1
        else:
            self.errors += 1
        if self.github:
            print(f"::{level} file={path},line={line},title={rule}::{msg}")
        else:
            print(f"{path}:{line}: {level}: [{rule}] {msg}")


def strip_markup(text):
    """Remove inline code, URLs and HTML; keep link text."""
    text = INLINE_CODE_RE.sub("CODE", text)
    text = LINK_RE.sub(lambda m: m.group(1) or m.group(2) or "", text)
    text = URL_RE.sub("URL", text)
    return HTML_TAG_RE.sub("", text)


def classify(lines):
    """Return a list of kinds per line: 'code', 'blank', 'heading',
    'item', 'text' or 'other' (tables, HTML, link definitions)."""
    kinds = []
    in_fence = None
    content_indent = 0  # indent of the text of the innermost list item
    prev = "blank"
    for raw in lines:
        line = raw.rstrip("\n")
        m = FENCE_RE.match(line)
        if in_fence:
            kinds.append("code")
            if m and m.group(1) == in_fence:
                in_fence = None
            prev = "code"
            continue
        if m:
            in_fence = m.group(1)
            kinds.append("code")
            prev = "code"
            continue
        if not line.strip():
            kinds.append("blank")
            prev = "blank"
            continue
        indent = len(line) - len(line.lstrip(" "))
        if line.startswith("\t"):
            indent = 8
        if prev in ("blank", "code") and indent >= content_indent + 4:
            kinds.append("code")
            prev = "code"
            continue
        stripped = line.strip()
        if HEADING_RE.match(line):
            kind = "heading"
            content_indent = 0
        elif LIST_RE.match(line):
            lm = LIST_RE.match(line)
            kind = "item"
            content_indent = len(lm.group(1)) + len(lm.group(2)) + 1
        elif (stripped.startswith(("|", "<", "[TOC]", "---", "***", ">"))
              or re.match(r"^\[[^\]]+\]:\s", stripped)):
            kind = "other"
        else:
            kind = "text"
            if prev == "blank" and indent < content_indent:
                content_indent = indent
        kinds.append(kind)
        prev = kind
    return kinds


def check_whitespace(path, lines, report):
    for n, raw in enumerate(lines, 1):
        body = raw.rstrip("\n")
        if body and not body.strip():
            report.add(path, n, "whitespace", "line contains only whitespace")
        elif body != body.rstrip():
            report.add(path, n, "whitespace", "trailing whitespace")
    if lines and not lines[-1].endswith("\n"):
        report.add(path, len(lines), "whitespace", "no newline at end of file")
    if lines and not lines[-1].strip():
        report.add(path, len(lines), "whitespace",
                   "blank line(s) at end of file")


def check_blank_lines(path, lines, kinds, report):
    for n in range(1, len(lines)):
        if kinds[n] == "blank" and kinds[n - 1] == "blank" and n + 1 < len(lines):
            report.add(path, n + 1, "blank-lines",
                       "more than one consecutive blank line")


def check_headings(path, lines, kinds, report):
    last_level = 1
    for n, (raw, kind) in enumerate(zip(lines, kinds), 1):
        if kind != "heading":
            continue
        m = HEADING_RE.match(raw)
        level, title = len(m.group(1)), m.group(2)
        if level == 1:
            report.add(path, n, "heading-level",
                       "use `##` or lower; `#` is reserved for the page title")
        elif level > last_level + 1:
            report.add(path, n, "heading-level",
                       f"heading jumps from level {last_level} to {level}")
        last_level = level
        check_title_case(path, n, title, report)


def check_title_case(path, n, title, report):
    words = strip_markup(title).rstrip(":").split()
    words = [w for w in words if w != "CODE"]
    bad = []
    for i, word in enumerate(words):
        core = word.strip("\"'()*_,")
        if not core or not core[0].isalpha():
            continue
        if "." in core or "/" in core or any(c.isupper() for c in core[1:]):
            continue  # file names, paths, identifiers like ioctl or eBPF
        edge = i == 0 or i == len(words) - 1
        if core[0].islower() and (edge or core.lower() not in SMALL_WORDS):
            bad.append(core)
        elif core[0].isupper() and not edge and core.lower() in SMALL_WORDS:
            bad.append(core)
    if bad:
        report.add(path, n, "heading-case",
                   "heading should use Title Case (check: "
                   + ", ".join(bad) + ")", warning=True)


def list_items(lines, kinds):
    """Yield (line number, marker indent, full text, last line number)."""
    n = 0
    while n < len(lines):
        if kinds[n] != "item":
            n += 1
            continue
        m = LIST_RE.match(lines[n].rstrip("\n"))
        start = n
        text = [m.group(3)]
        n += 1
        while n < len(lines) and kinds[n] == "text":
            text.append(lines[n].strip())
            n += 1
        yield start + 1, len(m.group(1)), " ".join(text), n


def check_lists(path, lines, kinds, report):
    for n, kind in enumerate(kinds):
        if kind == "item" and n > 0 and kinds[n - 1] in ("item", "text"):
            # Walk back to see whether the previous block is a list item.
            k = n - 1
            while k > 0 and kinds[k] == "text":
                k -= 1
            if kinds[k] == "item":
                report.add(path, n + 1, "bullet-spacing",
                           "separate list items with a blank line")

    for line_no, _indent, text, last in list_items(lines, kinds):
        plain = strip_markup(text).strip()
        lead = plain.lstrip("*_\"'( ")
        if (lead and lead[0].isalpha() and lead[0].islower()
                and not text.lstrip().startswith("`")
                and not re.match(r"^[a-z]+[A-Z]", lead)):  # e.g. iPhone
            report.add(path, line_no, "bullet-cap",
                       "list item should start with a capital letter")
        tail = text.rstrip().rstrip("*_")
        if (tail.endswith(".") and not tail.lower().endswith(ABBREVIATIONS)
                and not tail.endswith("`")):
            report.add(path, last, "bullet-period",
                       "list item should not end with a period")
        if SENTENCE_BREAK_RE.search(ABBREVIATION_RE.sub("abbr", plain).rstrip(".")):
            report.add(path, line_no, "bullet-sentences",
                       "list item should be at most one sentence; split it "
                       "into separate items or a paragraph", warning=True)


def check_numbers(path, lines, kinds, report):
    for n, (raw, kind) in enumerate(zip(lines, kinds), 1):
        if kind not in ("text", "item"):
            continue
        body = raw
        lm = LIST_RE.match(raw.rstrip("\n"))
        if lm:
            body = lm.group(3)
        plain = strip_markup(body)
        for m in SMALL_NUMBER_RE.finditer(plain):
            if (NUMBER_LABEL_RE.search(plain[:m.start()])
                    or NUMBER_UNIT_RE.match(plain[m.end():])):
                continue
            report.add(path, n, "small-number",
                       f"spell out numbers under 20 in prose ({m.group(1)})",
                       warning=True)
            break


def check_file(path, report):
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()
    kinds = classify(lines)
    check_whitespace(path, lines, report)
    check_blank_lines(path, lines, kinds, report)
    check_headings(path, lines, kinds, report)
    check_lists(path, lines, kinds, report)
    check_numbers(path, lines, kinds, report)


def changed_lines(base):
    out = subprocess.run(
        ["git", "diff", "-U0", "--no-color", "--diff-filter=AM",
         f"{base}...HEAD", "--", "*.md"],
        check=True, capture_output=True, text=True).stdout
    changed, path = {}, None
    for line in out.splitlines():
        if line.startswith("+++ "):
            path = line[6:] if line.startswith("+++ b/") else None
        elif line.startswith("@@") and path:
            m = re.search(r"\+(\d+)(?:,(\d+))?", line)
            start, count = int(m.group(1)), int(m.group(2) or 1)
            # A count of zero is a pure deletion; still check the line it
            # left behind so removals cannot create double blank lines.
            span = range(start, start + max(count, 1))
            changed.setdefault(path, set()).update(span)
    return changed


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("files", nargs="*")
    ap.add_argument("--diff-base", metavar="REF",
                    help="only report problems on lines changed since REF")
    ap.add_argument("--strict", action="store_true",
                    help="treat warnings as errors")
    args = ap.parse_args()

    changed = changed_lines(args.diff_base) if args.diff_base else None
    files = args.files
    if not files:
        files = subprocess.run(["git", "ls-files", "*.md"], check=True,
                               capture_output=True, text=True).stdout.split()
    if changed is not None:
        files = [f for f in files if f in changed]

    report = Report(changed)
    for path in files:
        check_file(path, report)

    print(f"\n{len(files)} file(s) checked: {report.errors} error(s), "
          f"{report.warnings} warning(s)")
    failed = report.errors or (args.strict and report.warnings)
    if failed:
        print("See styleguide.md on the master branch for the rules.")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
