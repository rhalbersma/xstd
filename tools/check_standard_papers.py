#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
"""Validate xstd's deterministic standards-paper references and tracking table."""

from pathlib import Path
import datetime
import re
import sys

FILES = (Path("README.md"), Path("CONTRIBUTING.md"), Path("doc/design.md"))
ANY_LINK = re.compile(r"https?://wg21\.link/([^\s)]+)", re.IGNORECASE)
ROW = re.compile(
    r"^\|\s*(P\d{4}R\d+)\s*\|.*?https://wg21\.link/(P\d{4}R\d+)\).*?"
    r"https://github\.com/cplusplus/papers/issues/(\d+)\).*?\|\s*(\d{4}-\d{2}-\d{2})\s*\|\s*(.+?)\s*\|$",
    re.IGNORECASE,
)

def error(path: Path, line: int, message: str) -> None:
    print(f"{path}:{line}: {message}", file=sys.stderr)


def main() -> int:
    failures = 0
    for path in FILES:
        for number, text in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            for match in ANY_LINK.finditer(text):
                target = match.group(1)
                if not re.fullmatch(r"P\d{4}R\d+", target, re.IGNORECASE):
                    error(path, number, f"WG21 link must name a pinned P####R# revision: {target}")
                    failures += 1

    path = Path("CONTRIBUTING.md")
    lines = path.read_text(encoding="utf-8").splitlines()
    begin = [i for i, line in enumerate(lines) if line == "<!-- standard-papers:begin -->"]
    end = [i for i, line in enumerate(lines) if line == "<!-- standard-papers:end -->"]
    if len(begin) != 1 or len(end) != 1 or begin[0] >= end[0]:
        error(path, 1, "expected one well-formed standard-papers tracking table")
        return 1

    rows = 0
    for index in range(begin[0] + 1, end[0]):
        text = lines[index]
        if not re.match(r"^\|\s*P\d{4}", text, re.IGNORECASE):
            continue
        rows += 1
        match = ROW.match(text)
        if not match:
            error(path, index + 1, "malformed tracking row; require pinned wording, issue, date, and relationship")
            failures += 1
            continue
        paper, reviewed, _issue, date, relationship = match.groups()
        if paper.upper() != reviewed.upper():
            error(path, index + 1, "paper and reviewed revision differ")
            failures += 1
        try:
            datetime.date.fromisoformat(date)
        except ValueError:
            error(path, index + 1, "last-reviewed date is not a valid ISO date")
            failures += 1
        if not relationship.strip():
            error(path, index + 1, "relationship statement is empty")
            failures += 1
    if rows == 0:
        error(path, begin[0] + 1, "tracking table has no paper rows")
        failures += 1
    return bool(failures)

if __name__ == "__main__":
    raise SystemExit(main())
