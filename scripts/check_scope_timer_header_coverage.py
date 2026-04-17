#!/usr/bin/env python3
"""
Compile the unit tests with clang source-based coverage and assert that
include/ScopeTimer.hpp stays above a minimum line-coverage threshold.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", default=str(repo_root))
    parser.add_argument(
        "--build-dir",
        default=str(repo_root / "build-header-coverage"),
        help="Directory used for the temporary coverage binary and reports.",
    )
    parser.add_argument(
        "--threshold",
        type=float,
        default=80.0,
        help="Minimum required line coverage percentage for ScopeTimer.hpp.",
    )
    return parser.parse_args()


def find_tool(name: str) -> str | None:
    direct = shutil.which(name)
    if direct:
        return direct

    xcrun = shutil.which("xcrun")
    if not xcrun:
        return None

    completed = subprocess.run(
        [xcrun, "--find", name],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        return None
    path = completed.stdout.strip()
    return path or None


def run_command(
    cmd: list[str],
    *,
    cwd: Path,
    env: dict[str, str] | None = None,
) -> str:
    merged_env = os.environ.copy()
    if env:
        merged_env.update(env)

    completed = subprocess.run(
        cmd,
        cwd=cwd,
        env=merged_env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        joined = " ".join(cmd)
        raise RuntimeError(
            f"Command failed ({completed.returncode}): {joined}\n{completed.stdout}"
        )
    return completed.stdout


def remove_if_exists(path: Path) -> None:
    if path.exists():
        path.unlink()


def main() -> int:
    args = parse_args()
    repo_root = Path(args.repo_root).resolve()
    build_dir = Path(args.build_dir).resolve()
    source = repo_root / "test" / "ScopeTimerTest.cpp"
    header = repo_root / "include" / "ScopeTimer.hpp"
    include_dir = repo_root / "include"

    if not source.exists():
        raise FileNotFoundError(f"Missing test source: {source}")
    if not header.exists():
        raise FileNotFoundError(f"Missing header: {header}")

    clangxx = find_tool("clang++")
    llvm_cov = find_tool("llvm-cov")
    llvm_profdata = find_tool("llvm-profdata")
    missing = [
        name
        for name, path in (
            ("clang++", clangxx),
            ("llvm-cov", llvm_cov),
            ("llvm-profdata", llvm_profdata),
        )
        if not path
    ]
    if missing:
        raise RuntimeError(
            "Missing required coverage tools: "
            + ", ".join(missing)
            + ". Install Xcode Command Line Tools or LLVM."
        )

    build_dir.mkdir(parents=True, exist_ok=True)
    binary = build_dir / "scopetimer_header_coverage_tests"
    profraw = build_dir / "scopetimer_header_coverage.profraw"
    profdata = build_dir / "scopetimer_header_coverage.profdata"
    report_path = build_dir / "coverage-report.txt"
    summary_path = build_dir / "coverage-summary.json"
    test_log_path = build_dir / "test-output.log"

    for artifact in (binary, profraw, profdata, report_path, summary_path, test_log_path):
        remove_if_exists(artifact)

    compile_cmd = [
        clangxx,
        "-std=c++17",
        "-O0",
        "-g",
        "-fprofile-instr-generate",
        "-fcoverage-mapping",
        "-I",
        str(include_dir),
        f'-DSCOPETIMER_SOURCE_DIR="{repo_root}"',
        str(source),
        "-o",
        str(binary),
    ]
    run_command(compile_cmd, cwd=repo_root)

    test_output = run_command(
        [str(binary)],
        cwd=repo_root,
        env={"LLVM_PROFILE_FILE": str(profraw)},
    )
    test_log_path.write_text(test_output, encoding="utf-8")

    run_command(
        [
            llvm_profdata,
            "merge",
            "-sparse",
            str(profraw),
            "-o",
            str(profdata),
        ],
        cwd=repo_root,
    )

    report_output = run_command(
        [
            llvm_cov,
            "report",
            str(binary),
            f"-instr-profile={profdata}",
            str(header),
        ],
        cwd=repo_root,
    )
    report_path.write_text(report_output, encoding="utf-8")

    summary_output = run_command(
        [
            llvm_cov,
            "export",
            "-summary-only",
            str(binary),
            f"-instr-profile={profdata}",
            str(header),
        ],
        cwd=repo_root,
    )
    summary_path.write_text(summary_output, encoding="utf-8")

    payload = json.loads(summary_output)
    totals = payload["data"][0]["totals"]
    lines = totals["lines"]
    functions = totals["functions"]
    branches = totals["branches"]

    print(report_output.rstrip())
    print()
    print(
        "ScopeTimer.hpp summary: "
        f"lines {lines['percent']:.2f}% ({lines['covered']}/{lines['count']}), "
        f"functions {functions['percent']:.2f}% ({functions['covered']}/{functions['count']}), "
        f"branches {branches['percent']:.2f}% ({branches['covered']}/{branches['count']})"
    )
    print(f"Coverage artifacts written to: {build_dir}")

    if float(lines["percent"]) < args.threshold:
        print(
            f"ERROR: ScopeTimer.hpp line coverage {lines['percent']:.2f}% "
            f"is below the required {args.threshold:.2f}%",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
