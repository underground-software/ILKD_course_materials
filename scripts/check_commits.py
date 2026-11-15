#!/usr/bin/env python3
"""Check commit messages for DCO sign-off, formatting, spelling and grammar.

The rules follow faq/commits.md and the "Commit Messages" section of
styleguide.md:

  dco          a "Signed-off-by: Name <email>" line matching the commit
               author (Developer Certificate of Origin, `git commit -s`)
  title        a title of at most 72 characters, optionally prefixed with
               an area such as "setup: ", starting with a capital letter,
               written in the imperative mood ("Fix", not "Fixed" or
               "Fixes") and not ending with a period
  blank-line   a blank line between the title and the body
  line-length  body lines of at most 75 characters (URLs, trailers and
               indented code or logs are exempt)
  whitespace   no trailing whitespace
  capital      body paragraphs and list items start with a capital letter
  repeat       no accidentally repeated words ("the the")
  spelling     no misspellings found by codespell

Merge commits are skipped.

Usage:
  check_commits.py [--require-spelling] RANGE

RANGE is anything `git rev-list` accepts, e.g. origin/2026_fall_uml..HEAD.
"""

import argparse
import os
import re
import shutil
import subprocess
import sys

TITLE_MAX = 72
BODY_MAX = 75

PREFIX_RE = re.compile(r"^[\w./-]+(?:\s*\([\w./-]+\))?:\s+")
TRAILER_RE = re.compile(r"^[A-Za-z][\w-]*:\s+\S")
SIGNOFF_RE = re.compile(r"^Signed-off-by:\s*(.+?)\s*<([^>]+)>\s*$", re.M)
LIST_RE = re.compile(r"^\s*(?:[-*+]|\d+[.)])\s+")
REPEAT_RE = re.compile(r"\b([A-Za-z]+)\s+\1\b", re.I)
URL_RE = re.compile(r"https?://")

# Imperative-looking words that end like past or progressive tense.
NOT_TENSED = {
    "bring", "embed", "feed", "need", "proceed", "seed", "shed", "speed",
    "spring", "string", "succeed", "exceed",
}
# Common commit verbs; "Adds"/"Added"/"Adding" all point back to "Add".
VERBS = {
    "add", "adjust", "allow", "apply", "bump", "change", "clarify",
    "clean", "convert", "correct", "create", "define", "delete", "describe",
    "document", "drop", "enable", "disable", "ensure", "explain", "expand",
    "extend", "fix", "format", "handle", "implement", "improve", "include",
    "introduce", "link", "make", "merge", "move", "port", "refactor",
    "remove", "rename", "reorder", "replace", "restructure", "revert",
    "rewrite", "simplify", "specify", "split", "support", "tidy", "update",
    "use", "write",
}
# Irregular past tenses of common commit verbs.
IRREGULAR = {"made": "make", "wrote": "write", "rewrote": "rewrite",
             "split": None, "took": "take", "gave": "give", "found": "find",
             "built": "build", "kept": "keep", "left": "leave",
             "brought": "bring", "began": "begin", "chose": "choose"}
# Words that look like third person verbs but are fine to start a title.
NOT_THIRD_PERSON = {"address", "process", "access", "pass", "focus", "bias"}


class Report:
    def __init__(self):
        self.errors = 0
        self.github = os.environ.get("GITHUB_ACTIONS") == "true"

    def add(self, commit, rule, msg):
        self.errors += 1
        short = commit["sha"][:12]
        if self.github:
            print(f"::error title=commit {short} ({rule})::"
                  f"{commit['title']!r}: {msg}")
        else:
            print(f"{short} {commit['title']!r}: [{rule}] {msg}")


def git(*args):
    return subprocess.run(["git", *args], check=True, capture_output=True,
                          text=True).stdout


def commits(rev_range):
    shas = git("rev-list", "--no-merges", "--reverse", rev_range).split()
    for sha in shas:
        out = git("show", "-s", "--format=%an%x00%ae%x00%B", sha)
        name, email, body = out.split("\0", 2)
        body = body.rstrip("\n")
        yield {"sha": sha, "name": name, "email": email, "message": body,
               "title": body.split("\n", 1)[0]}


def base_form(word):
    """Return the likely base verb if `word` is a tensed form of one."""
    w = word.lower()
    if w in IRREGULAR:
        return IRREGULAR[w]
    if w in NOT_TENSED or w in NOT_THIRD_PERSON:
        return None
    for suffix, repl in (("ied", "y"), ("ies", "y"), ("ed", ""), ("ed", "e"),
                         ("ing", ""), ("ing", "e"), ("es", ""), ("s", "")):
        if w.endswith(suffix):
            stem = w[: -len(suffix)] + repl
            if stem in VERBS:
                return stem
            if len(stem) > 2 and stem[-1] == stem[-2] and stem[:-1] in VERBS:
                return stem[:-1]  # "dropped" -> "drop"
    if (w.endswith("ed") or w.endswith("ing")) and len(w) > 4:
        return w  # unknown verb, but clearly not imperative
    return None


def check_title(commit, report):
    title = commit["title"]
    if not title.strip():
        report.add(commit, "title", "the commit has no title")
        return
    if len(title) > TITLE_MAX:
        report.add(commit, "title",
                   f"title is {len(title)} characters; keep it to "
                   f"{TITLE_MAX} or fewer")
    if title.rstrip().endswith("."):
        report.add(commit, "title", "title should not end with a period")
    if LIST_RE.match(title):
        report.add(commit, "title",
                   "title should be a one-line summary, not a list item")
    summary = PREFIX_RE.sub("", title, count=1)
    if summary[:1].islower():
        report.add(commit, "title",
                   "title should start with a capital letter after any "
                   "\"area: \" prefix")
    first = re.match(r"[A-Za-z]+", summary.lstrip("-*• "))
    if first:
        stem = base_form(first.group(0))
        if stem:
            report.add(commit, "title",
                       f"use the imperative mood (\"{stem.capitalize()}\", "
                       f"not \"{first.group(0)}\")")


def check_body(commit, report):
    lines = commit["message"].split("\n")
    if len(lines) > 1 and lines[1].strip():
        report.add(commit, "blank-line",
                   "leave a blank line between the title and the body")
    in_trailers = False
    prev_blank = True
    for n, line in enumerate(lines, 1):
        if line != line.rstrip():
            report.add(commit, "whitespace", f"line {n} has trailing whitespace")
        if n == 1:
            continue
        stripped = line.strip()
        if not stripped:
            prev_blank = True
            continue
        if TRAILER_RE.match(line) and (prev_blank or in_trailers):
            in_trailers = True
            prev_blank = False
            continue
        in_trailers = False
        indented = line.startswith(("    ", "\t"))
        if (len(line) > BODY_MAX and not URL_RE.search(line)
                and not indented):
            report.add(commit, "line-length",
                       f"line {n} is {len(line)} characters; wrap the body "
                       f"at {BODY_MAX}")
        starts_item = LIST_RE.match(line)
        if (prev_blank or starts_item) and not indented:
            text = LIST_RE.sub("", line)
            if text.startswith(("'", '"', "`")):
                prev_blank = False
                continue  # quoted code or identifiers keep their own case
            text = text.lstrip("(")
            word = text.split(" ", 1)[0]
            # File names, paths and identifiers keep their own case.
            identifier = re.search(r"[/._()]|[a-z][A-Z]", word)
            if (text[:1].islower() and not text.startswith("`")
                    and not identifier):
                report.add(commit, "capital",
                           f"line {n}: start sentences and list items with "
                           "a capital letter")
        prev_blank = False


def check_dco(commit, report):
    signoffs = SIGNOFF_RE.findall(commit["message"])
    if not signoffs:
        report.add(commit, "dco",
                   "missing \"Signed-off-by:\" line; amend with "
                   "`git commit --amend -s` (see "
                   "https://developercertificate.org)")
        return
    if not any(e.strip().lower() == commit["email"].lower()
               for _, e in signoffs):
        report.add(commit, "dco",
                   f"no Signed-off-by matches the author "
                   f"{commit['name']} <{commit['email']}>")


def strip_for_spelling(message):
    """Drop trailers, URLs, indented blocks and `code` before spell checks."""
    keep = []
    for line in message.split("\n"):
        if TRAILER_RE.match(line) or line.startswith(("    ", "\t")):
            continue
        line = re.sub(r"`[^`]*`", "CODE", line)
        line = re.sub(r"\S*https?://\S*", "URL", line)
        keep.append(line)
    return "\n".join(keep)


def check_language(commit, report):
    text = strip_for_spelling(commit["message"])
    for m in REPEAT_RE.finditer(text):
        report.add(commit, "repeat", f"repeated word \"{m.group(0)}\"")


def check_spelling(commit_list, report, required):
    exe = shutil.which("codespell")
    if not exe:
        msg = "codespell is not installed; skipping the spelling check"
        if required:
            print(f"error: {msg}", file=sys.stderr)
            report.errors += 1
        else:
            print(f"note: {msg} (pip install codespell)", file=sys.stderr)
        return
    for commit in commit_list:
        text = strip_for_spelling(commit["message"]) + "\n"
        out = subprocess.run([exe, "-"], input=text, capture_output=True,
                             text=True).stdout
        # codespell prints "<word> ==> <fix>" for each misspelling.
        for m in re.finditer(r"(\S+)\s*==>\s*(.+)", out):
            report.add(commit, "spelling", f"\"{m.group(1)}\" should be "
                       f"\"{m.group(2).strip()}\"")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("range", help="commits to check, e.g. origin/main..HEAD")
    ap.add_argument("--require-spelling", action="store_true",
                    help="fail if codespell is not installed")
    args = ap.parse_args()

    report = Report()
    commit_list = list(commits(args.range))
    for commit in commit_list:
        check_dco(commit, report)
        check_title(commit, report)
        check_body(commit, report)
        check_language(commit, report)
    check_spelling(commit_list, report, args.require_spelling)

    print(f"\n{len(commit_list)} commit(s) checked: {report.errors} error(s)")
    if report.errors:
        print("See \"Commit Messages\" in styleguide.md for the rules.")
    return 1 if report.errors else 0


if __name__ == "__main__":
    sys.exit(main())
