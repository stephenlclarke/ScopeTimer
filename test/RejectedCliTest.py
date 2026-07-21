#!/usr/bin/env python3
"""Assert that a CLI rejects invalid input with an actionable diagnostic."""

from __future__ import annotations

import argparse
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expected", required=True, help="Required text in combined output")
    parser.add_argument("command", nargs=argparse.REMAINDER, help="Command after --")
    args = parser.parse_args()

    command = list(args.command)
    if command[:1] == ["--"]:
        command.pop(0)
    if not command:
        parser.error("a command is required after --")

    completed = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    output = completed.stdout + completed.stderr
    if completed.returncode == 0:
        print("expected a nonzero exit status", file=sys.stderr)
        return 1
    if args.expected not in output:
        print(
            f"expected diagnostic {args.expected!r}, got:\n{output}",
            file=sys.stderr,
        )
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
