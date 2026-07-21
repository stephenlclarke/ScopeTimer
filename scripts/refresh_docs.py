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
MANAGED_BUILD_DIR_NAME = "docs-refresh"
MANAGED_BUILD_MARKER = ".scopetimer-docs-build"
MANAGED_BUILD_MARKER_CONTENT = "ScopeTimer managed docs build directory\n"

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
    parser.add_argument("--managed-root", required=True)
    parser.add_argument("--docs-build-dir", required=True)
    parser.add_argument("--build-doc", required=True)
    parser.add_argument("--tests-doc", required=True)
    return parser.parse_args()


def run_command(
    cmd: list[str],
    *,
    cwd: Path,
    env: dict[str, str] | None = None,
    input_text: str | None = None,
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
        input=input_text,
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


def prepare_managed_build_dir(
    path: Path,
    managed_root: Path,
    repo_root: Path,
) -> None:
    """Reset only the marker-owned docs build directory under a safe build root."""
    resolved_path = path.resolve()
    resolved_root = managed_root.resolve()
    resolved_repo = repo_root.resolve()
    filesystem_root = Path(resolved_root.anchor)

    if resolved_root in (filesystem_root, Path.home().resolve(), resolved_repo):
        raise RuntimeError(f"Refusing unsafe managed build root: {resolved_root}")
    if path.is_symlink():
        raise RuntimeError(f"Refusing symlinked docs build directory: {path}")

    expected_path = (resolved_root / MANAGED_BUILD_DIR_NAME).resolve()
    if resolved_path != expected_path:
        raise RuntimeError(
            "Docs build directory must be the managed child "
            f"{expected_path}, got {resolved_path}"
        )

    marker = resolved_path / MANAGED_BUILD_MARKER
    if resolved_path.exists():
        if not resolved_path.is_dir():
            raise RuntimeError(f"Docs build path is not a directory: {resolved_path}")
        if marker.is_symlink() or not marker.is_file():
            raise RuntimeError(
                f"Refusing to remove unowned docs build directory: {resolved_path}"
            )
        if marker.read_text(encoding="utf-8") != MANAGED_BUILD_MARKER_CONTENT:
            raise RuntimeError(
                f"Refusing to remove docs build directory with invalid marker: {resolved_path}"
            )
        remove_tree(resolved_path)

    resolved_path.mkdir(parents=True, exist_ok=False)
    (resolved_path / MANAGED_BUILD_MARKER).write_text(
        MANAGED_BUILD_MARKER_CONTENT,
        encoding="utf-8",
    )


def refresh_build_doc(
    repo_root: Path,
    docs_build_dir: Path,
    build_doc: Path,
) -> None:
    actual_configure_cmd = [
        "cmake",
        "-S",
        str(repo_root),
        "-B",
        str(docs_build_dir),
        "-DAUTO_REFRESH_DOCS=OFF",
        "-DENABLE_COVERAGE=OFF",
        "-DENABLE_SONAR=OFF",
    ]
    actual_build_cmd = ["cmake", "--build", str(docs_build_dir), "-j"]
    actual_test_cmd = ["ctest", "--test-dir", str(docs_build_dir), "--output-on-failure"]
    log_parts = []
    log_parts.append(run_command(actual_configure_cmd, cwd=repo_root).stdout)
    log_parts.append(run_command(actual_build_cmd, cwd=repo_root).stdout)
    log_parts.append(run_command(actual_test_cmd, cwd=repo_root).stdout)

    log_text = "".join(log_parts).replace(str(repo_root), ".").strip()
    log_lines = log_text.splitlines()
    head = "\n".join(log_lines[:BUILD_HEAD_LINES])
    tail_start = max(BUILD_HEAD_LINES, len(log_lines) - BUILD_TAIL_LINES)
    tail = "\n".join(log_lines[tail_start:])

    managed_refresh_cmd = (
        "> cmake --build "
        f"{relative_display_path(docs_build_dir.parent, repo_root)} "
        "--target docs_refresh"
    )

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
        "`coverage` and `sonar_scan` require `gcovr` and a build configured",
        "with `-DENABLE_COVERAGE=ON`. `sonar_scan` also needs",
        "`SONAR_TOKEN`, access to your SonarCloud or SonarQube server, and a",
        "build configured with `-DENABLE_SONAR=ON`. It passes the active CMake",
        "build directory through to the scanner so out-of-tree builds analyze",
        "the right artifacts. To stay within the free-tier branch limit, both",
        "the local target and GitHub Actions restrict Sonar scans to `main`.",
        "",
        "`leak_check` runs `scopetimer_tests` under the native leak detector",
        "for the current platform: `leaks` on macOS and `valgrind` on Linux.",
        "That means MacBook developer runs use `leaks`, while Linux hosts and",
        "the GitHub Actions build use Valgrind for leak detection.",
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
        "These targets configure a dedicated `<build-dir>/benchmark-build` tree with coverage",
        "disabled and maximum benchmark-only optimization flags enabled",
        "(default `-O3` on GCC/Clang and `/O2` on MSVC) without defining",
        "`NDEBUG`, then build and benchmark the `Benchmark` executable with",
        "`SCOPE_TIMER=0` and `SCOPE_TIMER=1` against the",
        "CPU-bound `hotpath-bench` scenario.",
        "",
        "Benchmarks are intentionally local-only for this repo and are not run",
        "in GitHub Actions. Run `demo_benchmark_matrix` on the MacBook before",
        "pushing changes that could affect performance.",
        "",
        "The human-readable benchmark results now live in",
        "[`BENCHMARK.md`](BENCHMARK.md). That file is refreshed automatically by",
        "`demo_benchmark_matrix`, and the full history remains in",
        "`benchmarks/demo_benchmark_history.json`.",
        "",
        "The generated transcript below is captured by the managed",
        "`docs_refresh` target. Use that target so the ownership marker is",
        "created before its nested build directory is replaced.",
        "",
        "<!-- markdownlint-disable MD013 -->",
        "",
        BASH_FENCE,
        managed_refresh_cmd,
        "> # Captured nested configure/build/test output:",
        head,
    ]
    if tail:
        lines.extend(
            [
                "> # Final 60 lines of the captured output:",
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
        processed = run_command(
            [str(repo_root / "scripts" / "process_scope_times.sh"), str(log_file)],
            cwd=repo_root,
        ).stdout
        summary = run_command(
            [str(repo_root / "scripts" / "summarize_scope_times.sh")],
            cwd=repo_root,
            input_text=processed,
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
    managed_root = Path(args.managed_root).resolve()
    docs_build_dir_arg = Path(args.docs_build_dir).expanduser()
    build_doc = Path(args.build_doc).resolve()
    tests_doc = Path(args.tests_doc).resolve()

    prepare_managed_build_dir(docs_build_dir_arg, managed_root, repo_root)
    docs_build_dir = docs_build_dir_arg.resolve()
    refresh_build_doc(repo_root, docs_build_dir, build_doc)
    refresh_tests_doc(repo_root, docs_build_dir, tests_doc)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
