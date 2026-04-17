#!/usr/bin/env python3
"""
Regenerate BUILD.md and TESTS.md from live build artifacts.

BENCHMARK.md is refreshed by the dedicated benchmark targets rather than the
default build so local builds stay fast.
"""

from __future__ import annotations

import argparse
import os
import subprocess
import tempfile
import time
from pathlib import Path


BUILD_HEAD_LINES = 120
BUILD_TAIL_LINES = 60
TEST_LOG_HEAD_LINES = 24
BUILD_REVIEW_CONFIGURE_CMD = "cmake -S . -B build-review"
BASH_FENCE = "```bash"

FORMAT_SECTIONS = [
    ("Testing with default elapsed time formatting", None),
    ("Testing with elapsed time formatted in seconds", "SECONDS"),
    ("Testing with elapsed time formatted in millis", "MILLIS"),
    ("Testing with elapsed time formatted in micros", "MICROS"),
    ("Testing with elapsed time formatted in nanos", "NANOS"),
]


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", default=str(repo_root))
    parser.add_argument("--docs-build-dir", required=True)
    parser.add_argument("--build-doc", required=True)
    parser.add_argument("--tests-doc", required=True)
    return parser.parse_args()


def run_command(
    cmd: list[str],
    *,
    cwd: Path,
    env: dict[str, str] | None = None,
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
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
    if check and completed.returncode != 0:
        raise RuntimeError(
            f"Command failed ({completed.returncode}): {' '.join(cmd)}\n{completed.stdout}"
        )
    return completed


def relative_display_path(path: Path, repo_root: Path) -> str:
    try:
        rel = path.relative_to(repo_root)
        return f"./{rel.as_posix()}"
    except ValueError:
        return str(path)


def remove_tree(path: Path) -> None:
    if not path.exists():
        return

    for attempt in range(5):
        rm = subprocess.run(
            ["cmake", "-E", "rm", "-rf", str(path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
        if not path.exists():
            return
        if attempt == 4:
            raise RuntimeError(
                f"Failed to remove directory {path}\n{rm.stdout}"
            )
        time.sleep(0.2 * (attempt + 1))


def refresh_build_doc(
    repo_root: Path,
    docs_build_dir: Path,
    build_doc: Path,
) -> None:
    remove_tree(docs_build_dir)

    configure_cmd = [
        "cmake",
        "-S",
        ".",
        "-B",
        relative_display_path(docs_build_dir, repo_root),
        "-DAUTO_REFRESH_DOCS=OFF",
    ]
    actual_configure_cmd = [
        "cmake",
        "-S",
        str(repo_root),
        "-B",
        str(docs_build_dir),
        "-DAUTO_REFRESH_DOCS=OFF",
    ]
    build_cmd = ["cmake", "--build", relative_display_path(docs_build_dir, repo_root), "-j"]
    actual_build_cmd = ["cmake", "--build", str(docs_build_dir), "-j"]
    test_cmd = [
        "ctest",
        "--test-dir",
        relative_display_path(docs_build_dir, repo_root),
        "--output-on-failure",
    ]
    actual_test_cmd = ["ctest", "--test-dir", str(docs_build_dir), "--output-on-failure"]
    sonar_cmd = [
        "cmake",
        "--build",
        relative_display_path(docs_build_dir, repo_root),
        "--target",
        "sonar_scan",
    ]
    actual_sonar_cmd = ["cmake", "--build", str(docs_build_dir), "--target", "sonar_scan"]

    log_parts = []
    log_parts.append(run_command(actual_configure_cmd, cwd=repo_root).stdout)
    log_parts.append(run_command(actual_build_cmd, cwd=repo_root).stdout)
    log_parts.append(run_command(actual_test_cmd, cwd=repo_root).stdout)

    include_sonar = bool(os.environ.get("SONAR_TOKEN"))
    if include_sonar:
        sonar_run = run_command(actual_sonar_cmd, cwd=repo_root, check=False)
        log_parts.append(sonar_run.stdout)
    log_text = "".join(log_parts).strip()
    log_lines = log_text.splitlines()
    head = "\n".join(log_lines[:BUILD_HEAD_LINES])
    tail = "\n".join(log_lines[-BUILD_TAIL_LINES:]) if len(log_lines) > BUILD_HEAD_LINES else ""

    composite_cmd = [
        f"> rm -rf {relative_display_path(docs_build_dir, repo_root)} ./build-docs.log",
        f"> {{ {' '.join(configure_cmd)} && \\",
        f"  {' '.join(build_cmd)} && \\",
        f"  {' '.join(test_cmd)}"
        + (" && \\" if include_sonar else ";")
    ]
    if include_sonar:
        composite_cmd.append(f"  {' '.join(sonar_cmd)}; }} > ./build-docs.log 2>&1")
    else:
        composite_cmd[-1] = composite_cmd[-1][:-1]
        composite_cmd.append("} > ./build-docs.log 2>&1")

    lines = [
        "<!-- Generated automatically by scripts/refresh_docs.py -->",
        "",
        "# Build",
        "",
        "Build, coverage, Sonar, and benchmark-target usage for this repo lives",
        "here so the main README can stay focused on the library API.",
        "",
        "See also:",
        "",
        "- [README.md](README.md) for library usage",
        "- [TESTS.md](TESTS.md) for log-format examples and summary output",
        "- [BENCHMARK.md](BENCHMARK.md) for the latest benchmark snapshot",
        "",
        "`coverage` and `sonar_scan` require `gcovr`. `sonar_scan` also needs",
        "`SONAR_TOKEN` plus access to your SonarCloud or SonarQube server, and",
        "it passes the active CMake build directory through to the scanner so",
        "out-of-tree builds analyze the right artifacts.",
        "",
        "`leak_check` runs `scopetimer_tests` under the native leak detector",
        "for the current platform: `leaks` on macOS and `valgrind` on Linux.",
        "Run it with:",
        "",
        BASH_FENCE,
        BUILD_REVIEW_CONFIGURE_CMD,
        "cmake --build build-review --target leak_check",
        "```",
        "",
        "`scopetimer_header_coverage` is the header-only coverage gate for",
        "`include/ScopeTimer.hpp`. It rebuilds `test/ScopeTimerTest.cpp` with",
        "clang source-based coverage and enforces the configured line-coverage",
        "threshold (default `80%`). Run it with:",
        "",
        BASH_FENCE,
        BUILD_REVIEW_CONFIGURE_CMD,
        "cmake --build build-review --target scopetimer_header_coverage",
        "```",
        "",
        "`Demo` is the educational example app. The dedicated overhead workload",
        "now lives in `example/Benchmark.cpp`.",
        "",
        "Benchmarks are intentionally excluded from the default",
        "`cmake --build` path so local builds stay fast. Run them explicitly",
        "with:",
        "",
        BASH_FENCE,
        BUILD_REVIEW_CONFIGURE_CMD,
        "cmake --build build-review --target demo_benchmark",
        "cmake --build build-review --target demo_benchmark_matrix",
        "```",
        "",
        "These targets configure a dedicated `build-bench` tree with coverage",
        "disabled and maximum benchmark-only optimization flags enabled",
        "(default `-O3` on GCC/Clang and `/O2` on MSVC) without defining",
        "`NDEBUG`, then build and benchmark the `Benchmark` executable with",
        "`SCOPE_TIMER=0` and `SCOPE_TIMER=1` against the",
        "CPU-bound `hotpath-bench` scenario.",
        "",
        "Pull request CI runs `demo_benchmark_matrix` automatically for PR",
        "open and update events, then uploads the refreshed benchmark report",
        "as an artifact.",
        "",
        "The human-readable benchmark results now live in",
        "[`BENCHMARK.md`](BENCHMARK.md). That file is refreshed automatically by",
        "`demo_benchmark_matrix`, and the full history remains in",
        "`benchmarks/demo_benchmark_history.json`.",
        "",
        "<!-- markdownlint-disable MD013 -->",
        "",
        BASH_FENCE,
        *composite_cmd,
        "> sed -n '1,120p' ./build-docs.log",
        head,
    ]
    if tail:
        lines.extend(
            [
                "> tail -n 60 ./build-docs.log",
                tail,
            ]
        )
    lines.extend(
        [
            "```",
            "",
            "<!-- markdownlint-enable MD013 -->",
            "",
        ]
    )
    build_doc.write_text("\n".join(lines), encoding="utf-8")


def run_demo_log(
    repo_root: Path,
    demo_binary: Path,
    *,
    fmt: str | None = None,
) -> list[str]:
    env = {"SCOPE_TIMER_DIR": "."}
    if fmt:
        env["SCOPE_TIMER_FORMAT"] = fmt
    with tempfile.TemporaryDirectory() as tmp_dir_str:
        tmp_dir = Path(tmp_dir_str)
        env["SCOPE_TIMER_DIR"] = str(tmp_dir)
        run_command(
            [str(demo_binary), "--iterations=1"],
            cwd=repo_root,
            env=env,
        )
        log_file = tmp_dir / "ScopeTimer.log"
        if not log_file.exists():
            raise RuntimeError(f"Expected log file was not written: {log_file}")
        return log_file.read_text(encoding="utf-8").splitlines()[:TEST_LOG_HEAD_LINES]


def run_summary(repo_root: Path, demo_binary: Path) -> str:
    with tempfile.TemporaryDirectory() as tmp_dir_str:
        tmp_dir = Path(tmp_dir_str)
        run_command(
            [str(demo_binary), "--iterations=1"],
            cwd=repo_root,
            env={
                "SCOPE_TIMER_DIR": str(tmp_dir),
                "SCOPE_TIMER_FORMAT": "NANOS",
            },
        )
        log_file = tmp_dir / "ScopeTimer.log"
        process_cmd = (
            f"scripts/process_scope_times.sh {log_file} | scripts/summarize_scope_times.sh"
        )
        summary = run_command(
            ["bash", "-lc", process_cmd],
            cwd=repo_root,
        ).stdout.rstrip()
        return summary


def refresh_tests_doc(
    repo_root: Path,
    docs_build_dir: Path,
    tests_doc: Path,
) -> None:
    demo_binary = docs_build_dir / "Demo"
    demo_display = relative_display_path(demo_binary, repo_root)

    lines = [
        "<!-- Generated automatically by scripts/refresh_docs.py -->",
        "",
        "# Tests",
        "",
        "Formatting and log-processing examples live here so the main README",
        "stays focused on how to use the library.",
        "",
        "See also:",
        "",
        "- [README.md](README.md) for the library overview and API examples",
        "- [BUILD.md](BUILD.md) for build, coverage, Sonar, and benchmark-target usage",
        "",
        "<!-- markdownlint-disable MD013 -->",
        "",
    ]

    for heading, fmt in FORMAT_SECTIONS:
        lines.append(f"## {heading}")
        lines.append("")
        lines.append(BASH_FENCE)
        if fmt:
            cmd = (
                f"> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. "
                f"SCOPE_TIMER_FORMAT={fmt} {demo_display} --iterations=1 "
                ">/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log"
            )
        else:
            cmd = (
                f"> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. "
                f"{demo_display} --iterations=1 >/dev/null 2>&1; "
                "sed -n '1,24p' ./ScopeTimer.log"
            )
        lines.append(cmd)
        lines.extend(run_demo_log(repo_root, demo_binary, fmt=fmt))
        lines.append("```")
        lines.append("")

    lines.extend(
        [
            "## Summarise the logging",
            "",
            BASH_FENCE,
            f"> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS "
            f"{demo_display} --iterations=1 >/dev/null 2>&1; "
            "scripts/process_scope_times.sh ./ScopeTimer.log | "
            "scripts/summarize_scope_times.sh",
            run_summary(repo_root, demo_binary),
            "```",
            "",
            "<!-- markdownlint-enable MD013 -->",
            "",
        ]
    )

    tests_doc.write_text("\n".join(lines), encoding="utf-8")

def main() -> int:
    args = parse_args()
    repo_root = Path(args.repo_root).resolve()
    docs_build_dir = Path(args.docs_build_dir).resolve()
    build_doc = Path(args.build_doc).resolve()
    tests_doc = Path(args.tests_doc).resolve()

    refresh_build_doc(repo_root, docs_build_dir, build_doc)
    refresh_tests_doc(repo_root, docs_build_dir, tests_doc)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
