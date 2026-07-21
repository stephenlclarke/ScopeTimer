#!/usr/bin/env python3
"""Verify ScopeTimer works as an add_subdirectory CMake dependency."""

from __future__ import annotations

import subprocess
import sys
import tempfile
from pathlib import Path


def run(command: list[str], cwd: Path) -> None:
    completed = subprocess.run(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(f"command failed: {' '.join(command)}\n{completed.stdout}")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: CMakeConsumerTest.py <ScopeTimer repo root>", file=sys.stderr)
        return 2
    repo = Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="scopetimer-consumer-") as temporary:
        root = Path(temporary)
        (root / "CMakeLists.txt").write_text(
            "\n".join(
                [
                    "cmake_minimum_required(VERSION 3.16)",
                    "project(ScopeTimerConsumer LANGUAGES CXX)",
                    f'add_subdirectory("{repo.as_posix()}" scopetimer)',
                    "if(TARGET Demo OR TARGET Benchmark OR TARGET scopetimer_tests)",
                    '  message(FATAL_ERROR "ScopeTimer consumer unexpectedly received development targets")',
                    "endif()",
                    "add_executable(consumer main.cpp)",
                    "target_link_libraries(consumer PRIVATE ScopeTimer::ScopeTimer)",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        (root / "main.cpp").write_text(
            '#include "ScopeTimer.hpp"\nint main() { SCOPE_TIMER(); return 0; }\n',
            encoding="utf-8",
        )
        build_dir = root / "build"
        run(["cmake", "-S", str(root), "-B", str(build_dir)], root)
        run(["cmake", "--build", str(build_dir), "--target", "consumer"], root)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as error:
        print(error, file=sys.stderr)
        raise SystemExit(1)
