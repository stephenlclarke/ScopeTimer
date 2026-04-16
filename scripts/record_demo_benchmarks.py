#!/usr/bin/env python3
"""
Run the reproducible ScopeTimer benchmark matrix and update repo benchmark reports.

The machine-readable history is stored in JSON so benchmark drift can be tracked
alongside code changes. A human-readable Markdown report is also refreshed as a
snapshot of the latest benchmark run plus its delta from the previous one.
"""

from __future__ import annotations

import argparse
import json
import shlex
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

import benchmark_demo

REPO_ROOT = Path(__file__).resolve().parent.parent
SCHEMA_VERSION = 1
NOISE_TOLERANCE_PCT = 2.0
SINK_BYTES_PLACEHOLDER = "{sink_bytes}"
THREADS_PLACEHOLDER = "{threads}"
DEFAULT_REPORT_CONFIG: dict[str, Any] = {
    "binary": "./build-bench/Benchmark",
    "scenario": "hotpath-bench",
    "iterations": 5,
    "runs": 8,
    "threads": 4,
    "sink_bytes": 4096,
}

PROFILE_DEFS: list[dict[str, Any]] = [
    {
        "name": "standard_default",
        "label": "Standard timer, default sink",
        "description": (
            "Baseline ScopeTimer cost with the normal synchronous sink and "
            "wall-clock timestamps enabled."
        ),
        "env": {},
    },
    {
        "name": "standard_walltime_off",
        "label": "Standard timer, wall time disabled",
        "description": (
            "Standard timer with `SCOPE_TIMER_WALLTIME=0` so the report shows "
            "the cost of dropping `start=` and `end=` timestamp formatting."
        ),
        "env": {"SCOPE_TIMER_WALLTIME": "0"},
    },
    {
        "name": "buffered_single_thread",
        "label": "Standard timer, buffered sink",
        "description": (
            "Single-thread run with the thread-buffered sink enabled to show "
            "how much caller-thread write overhead falls when flushes are batched."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "BUFFERED",
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
    {
        "name": "buffered_threaded",
        "label": "Standard timer, buffered sink (threaded stress)",
        "description": (
            "Multi-threaded buffered run that stresses contention and cross-thread "
            "flush behavior under the standard timer format."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "BUFFERED",
            "SCOPE_TIMER_BENCH_SINK_BYTES": SINK_BYTES_PLACEHOLDER,
            "SCOPE_TIMER_BENCH_THREADS": THREADS_PLACEHOLDER,
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
    {
        "name": "async_threaded",
        "label": "Standard timer, async sink",
        "description": (
            "Multi-threaded run with the async sink so flush work moves to the "
            "background writer instead of the calling thread."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "ASYNC",
            "SCOPE_TIMER_BENCH_SINK_BYTES": SINK_BYTES_PLACEHOLDER,
            "SCOPE_TIMER_BENCH_THREADS": THREADS_PLACEHOLDER,
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
    {
        "name": "hotpath_async_threaded",
        "label": "Hot-path timer, async sink",
        "description": (
            "Lowest-overhead profile: hot-path timer format plus the async sink, "
            "measured under the threaded stress workload."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "ASYNC",
            "SCOPE_TIMER_BENCH_SINK_BYTES": SINK_BYTES_PLACEHOLDER,
            "SCOPE_TIMER_BENCH_THREADS": THREADS_PLACEHOLDER,
            "SCOPE_TIMER_BENCH_TIMER": "HOTPATH",
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
]


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parent.parent

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", help="Path to the benchmark executable")
    parser.add_argument("--scenario", default="hotpath-bench", help="Benchmark scenario to benchmark")
    parser.add_argument("--iterations", type=int, default=5, help="Benchmark --iterations value")
    parser.add_argument("--runs", type=int, default=8, help="Number of alternating disabled/enabled runs")
    parser.add_argument("--threads", type=int, default=4, help="Worker thread count for threaded profiles")
    parser.add_argument("--sink-bytes", type=int, default=4096, help="Flush threshold for buffered/async profiles")
    parser.add_argument(
        "--history-file",
        default=str(repo_root / "benchmarks" / "demo_benchmark_history.json"),
        help="JSON history file to update",
    )
    parser.add_argument(
        "--report-file",
        default=str(repo_root / "BENCHMARK.md"),
        help="Markdown benchmark report to refresh",
    )
    parser.add_argument(
        "--build-dir",
        default="build-bench",
        help="Benchmark build directory description saved into the history entry",
    )
    parser.add_argument(
        "--cxx-flags",
        default="-O2",
        help="Compiler flags description saved into the history entry",
    )
    parser.add_argument(
        "--refresh-report-only",
        action="store_true",
        help="Regenerate the Markdown benchmark report from existing history without running benchmarks",
    )
    return parser.parse_args()


def run_git(args: list[str], repo_root: Path) -> str:
    completed = subprocess.run(
        ["git", *args],
        cwd=repo_root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        return ""
    return completed.stdout.strip()


def git_metadata(repo_root: Path) -> dict[str, Any]:
    status = run_git(["status", "--short"], repo_root)
    return {
        "branch": run_git(["rev-parse", "--abbrev-ref", "HEAD"], repo_root) or "unknown",
        "commit": run_git(["rev-parse", "HEAD"], repo_root) or "unknown",
        "short_commit": run_git(["rev-parse", "--short", "HEAD"], repo_root) or "unknown",
        "subject": run_git(["log", "-1", "--pretty=%s"], repo_root) or "unknown",
        "message": run_git(["log", "-1", "--pretty=%B"], repo_root) or "unknown",
        "dirty": bool(status),
    }


def format_profile_env(template_env: dict[str, str], threads: int, sink_bytes: int) -> dict[str, str]:
    return {
        key: value.format(threads=threads, sink_bytes=sink_bytes)
        for key, value in template_env.items()
    }


def display_path(path_value: str) -> str:
    if not path_value:
        return path_value

    path = Path(path_value).expanduser()
    try:
        return f"./{path.resolve().relative_to(REPO_ROOT)}"
    except (OSError, ValueError):
        return path_value


def profile_doc_config(entry: dict[str, Any] | None) -> dict[str, Any]:
    config = dict(DEFAULT_REPORT_CONFIG)
    if entry is None:
        return config

    config.update(entry.get("benchmark_config", {}))
    config["binary"] = display_path(str(config.get("binary", DEFAULT_REPORT_CONFIG["binary"])))
    return config


def profile_benchmark_command(profile_def: dict[str, Any], config: dict[str, Any]) -> str:
    env = format_profile_env(
        profile_def["env"],
        int(config.get("threads", DEFAULT_REPORT_CONFIG["threads"])),
        int(config.get("sink_bytes", DEFAULT_REPORT_CONFIG["sink_bytes"])),
    )
    command = [
        "python3",
        "scripts/benchmark_demo.py",
        "--binary",
        str(config.get("binary", DEFAULT_REPORT_CONFIG["binary"])),
        "--scenario",
        str(config.get("scenario", DEFAULT_REPORT_CONFIG["scenario"])),
        "--iterations",
        str(config.get("iterations", DEFAULT_REPORT_CONFIG["iterations"])),
        "--runs",
        str(config.get("runs", DEFAULT_REPORT_CONFIG["runs"])),
    ]
    for key, value in env.items():
        command.extend(["--env", f"{key}={value}"])
    return shlex.join(command)


def profile_reference_lines(config: dict[str, Any]) -> list[str]:
    lines = [
        "## Profile reference",
        "",
        "The commands below rerun one profile at a time through",
        "`scripts/benchmark_demo.py`. The harness handles the alternating",
        "`SCOPE_TIMER=0` and `SCOPE_TIMER=1` passes for you; the `--env` flags",
        "shown here are only the profile-specific knobs.",
        "",
        (
            "These examples use "
            f"`binary={config.get('binary', DEFAULT_REPORT_CONFIG['binary'])}`, "
            f"`scenario={config.get('scenario', DEFAULT_REPORT_CONFIG['scenario'])}`, "
            f"`iterations={config.get('iterations', DEFAULT_REPORT_CONFIG['iterations'])}`, "
            f"`runs={config.get('runs', DEFAULT_REPORT_CONFIG['runs'])}`, "
            f"`threads={config.get('threads', DEFAULT_REPORT_CONFIG['threads'])}`, "
            f"and `sink_bytes={config.get('sink_bytes', DEFAULT_REPORT_CONFIG['sink_bytes'])}`."
        ),
        "",
    ]

    for profile_def in PROFILE_DEFS:
        lines.extend(
            [
                f"### {profile_def['label']}",
                "",
                profile_def["description"],
                "",
                "```bash",
                profile_benchmark_command(profile_def, config),
                "```",
                "",
            ]
        )

    return lines


def load_history(path: Path) -> dict[str, Any]:
    if not path.exists():
        return {"schema_version": SCHEMA_VERSION, "history": []}

    with path.open("r", encoding="utf-8") as handle:
        raw = handle.read().strip()
    if not raw:
        return {"schema_version": SCHEMA_VERSION, "history": []}
    data = json.loads(raw)

    if data.get("schema_version") != SCHEMA_VERSION:
        raise SystemExit(
            f"Unsupported benchmark history schema in {path}: {data.get('schema_version')!r}"
        )
    if not isinstance(data.get("history"), list):
        raise SystemExit(f"Invalid benchmark history format in {path}")
    return data


def save_history(path: Path, history: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        json.dump(history, handle, indent=2)
        handle.write("\n")


def format_seconds(value: float | None) -> str:
    if value is None:
        return "n/a"
    return f"{value:.6f}s"


def format_percent(value: float | None) -> str:
    if value is None:
        return "n/a"
    return f"{value:.3f}%"


def format_us(value: float | None) -> str:
    if value is None:
        return "n/a"
    return f"{value:.3f}us"


def format_yes_no(value: bool) -> str:
    return "yes" if value else "no"


def format_metric_value(metric_name: str | None, value: float | None, *, signed: bool = False) -> str:
    if value is None:
        return "n/a"
    if metric_name == "approx_per_record_us":
        return f"{value:+.3f}us" if signed else f"{value:.3f}us"
    if metric_name == "delta_mean_s":
        return f"{value:+.6f}s" if signed else f"{value:.6f}s"
    return f"{value:+.6f}" if signed else f"{value:.6f}"


def comparison_delta_text(comparison: dict[str, Any]) -> str:
    status = comparison.get("status", "unknown")
    if status == "baseline":
        return "baseline"
    if status == "unknown":
        return "comparison unavailable"

    metric_name = comparison.get("metric")
    delta_value = comparison.get("delta_value")
    delta_pct = comparison.get("delta_pct")
    return (
        f"{format_metric_value(metric_name, delta_value, signed=True)} "
        f"({delta_pct:+.1f}%)"
    )


def render_report(history: dict[str, Any]) -> str:
    entries = history.get("history", [])
    latest = entries[-1] if entries else None
    doc_config = profile_doc_config(latest)
    lines = [
        "<!-- markdownlint-disable MD013 -->",
        "",
        "# Benchmark Results",
        "",
        "Human-readable ScopeTimer benchmark reporting. The machine-readable",
        "history lives in `benchmarks/demo_benchmark_history.json`.",
        "",
        "## Reproducing the benchmarks",
        "",
        "```bash",
        "cmake -S . -B build-review",
        "cmake --build build-review --target demo_benchmark",
        "cmake --build build-review --target demo_benchmark_matrix",
        "```",
        "",
        "`demo_benchmark` runs the direct enabled-vs-disabled benchmark for",
        "`example/Benchmark.cpp`. `demo_benchmark_matrix` runs the full profile",
        "matrix, appends `benchmarks/demo_benchmark_history.json`, and refreshes",
        "this file with the latest snapshot.",
    ]

    if not entries:
        lines.extend(
            [
                "",
                "## Current benchmark snapshot",
                "",
                "No benchmark history has been recorded yet.",
                "",
            ]
        )
        lines.extend(
            [
                "Full history will be written to",
                "`benchmarks/demo_benchmark_history.json` after the first",
                "`demo_benchmark_matrix` run.",
            ]
        )
        lines.extend(["", *profile_reference_lines(doc_config)])
        return "\n".join(lines)

    previous = entries[-2] if len(entries) > 1 else None
    git = latest.get("git", {})
    config = latest.get("benchmark_config", {})

    lines.extend(
        [
            "",
            "## Current benchmark snapshot",
            "",
            f"- Recorded at: `{latest.get('recorded_at_utc', 'unknown')}`",
            f"- Commit: `{git.get('short_commit', 'unknown')}`",
            f"- Subject: {git.get('subject', 'unknown')}",
            f"- Branch: `{git.get('branch', 'unknown')}`",
            f"- Dirty worktree: {format_yes_no(bool(git.get('dirty', False)))}",
            (
                "- Config: "
                f"`binary={config.get('binary', 'n/a')}`, "
                f"`build_dir={config.get('build_dir', 'n/a')}`, "
                f"`scenario={config.get('scenario', 'n/a')}`, "
                f"`iterations={config.get('iterations', 'n/a')}`, "
                f"`runs={config.get('runs', 'n/a')}`, "
                f"`threads={config.get('threads', 'n/a')}`, "
                f"`sink_bytes={config.get('sink_bytes', 'n/a')}`, "
                f"`cxx_flags={config.get('cxx_flags', 'n/a')}`"
            ),
        ]
    )

    if previous:
        prev_git = previous.get("git", {})
        lines.extend(
            [
                (
                    "- Previous benchmark: "
                    f"`{previous.get('recorded_at_utc', 'unknown')}` "
                    f"on `{prev_git.get('short_commit', 'unknown')}`"
                ),
                "- Delta source: per-record overhead when available, otherwise mean overhead.",
            ]
        )
    else:
        lines.append("- Previous benchmark: none recorded yet.")

    lines.extend(
        [
            "",
            "| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs previous | Status |",
            "| --- | --- | --- | --- | --- | --- | --- |",
        ]
    )

    for profile in latest.get("results", []):
        comparison = profile.get("comparison_to_previous", {})
        lines.append(
            "| "
            f"{profile.get('label', profile.get('name', 'unknown'))} | "
            f"`{format_us(profile.get('approx_per_record_us'))}` | "
            f"`{format_seconds(profile.get('delta_mean_s'))}` "
            f"({format_percent(profile.get('overhead_mean_pct'))}) | "
            f"`{format_seconds(profile.get('enabled_mean_s'))}` | "
            f"`{profile.get('enabled_log_lines', 0)}` | "
            f"{comparison_delta_text(comparison)} | "
            f"{comparison.get('status', 'unknown')} |"
        )

    lines.extend(
        [
            "",
            "Full historical results remain in",
            "`benchmarks/demo_benchmark_history.json`.",
        ]
    )
    lines.extend(["", *profile_reference_lines(doc_config)])

    return "\n".join(lines)


def save_report(path: Path, history: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        handle.write(render_report(history).rstrip())
        handle.write("\n")


def comparison_for_profile(
    current_report: dict[str, Any],
    previous_entry: dict[str, Any] | None,
    profile_name: str,
) -> dict[str, Any]:
    if not previous_entry:
        return {
            "indicator": "baseline",
            "status": "baseline",
            "summary": "baseline run",
        }

    previous_profiles = {
        profile.get("name"): profile for profile in previous_entry.get("results", [])
    }
    previous_profile = previous_profiles.get(profile_name)
    if not previous_profile:
        return {
            "indicator": "baseline",
            "status": "baseline",
            "summary": "baseline run for this profile",
        }

    metric_name = "approx_per_record_us"
    current_value = current_report.get(metric_name)
    previous_value = previous_profile.get(metric_name)
    if current_value is None or previous_value is None:
        metric_name = "delta_mean_s"
        current_value = current_report.get(metric_name)
        previous_value = previous_profile.get(metric_name)

    if current_value is None or previous_value in (None, 0):
        return {
            "indicator": "unknown",
            "status": "unknown",
            "summary": "comparison unavailable",
        }

    delta_value = float(current_value) - float(previous_value)
    delta_pct = (delta_value / float(previous_value)) * 100.0
    abs_delta_pct = abs(delta_pct)

    if abs_delta_pct <= NOISE_TOLERANCE_PCT:
        status = "unchanged"
        indicator = "="
    elif delta_value < 0:
        status = "faster"
        indicator = "faster"
    else:
        status = "slower"
        indicator = "slower"

    unit = "us/record" if metric_name == "approx_per_record_us" else "s mean overhead"
    summary = (
        f"{status} vs {previous_entry['git']['short_commit']} "
        f"({previous_value:.3f} -> {current_value:.3f} {unit}, {delta_pct:+.1f}%)"
    )

    return {
        "indicator": indicator,
        "status": status,
        "metric": metric_name,
        "unit": unit,
        "previous_value": previous_value,
        "current_value": current_value,
        "delta_value": delta_value,
        "delta_pct": delta_pct,
        "summary": summary,
        "previous_recorded_at_utc": previous_entry.get("recorded_at_utc"),
        "previous_commit": previous_entry.get("git", {}).get("commit"),
        "previous_short_commit": previous_entry.get("git", {}).get("short_commit"),
        "tolerance_pct": NOISE_TOLERANCE_PCT,
    }


def print_profile_result(profile: dict[str, Any], report: dict[str, Any], comparison: dict[str, Any]) -> None:
    print(f"=== {profile['label']} ===")
    benchmark_demo.print_text_report(report)
    print(f"comparison:           {comparison['summary']}")


def main() -> int:
    args = parse_args()
    repo_root = Path(__file__).resolve().parent.parent
    history_path = Path(args.history_file)
    report_path = Path(args.report_file)

    if args.refresh_report_only:
        history = load_history(history_path)
        save_report(report_path, history)
        print(f"Saved benchmark report: {report_path}")
        return 0

    if not args.binary:
        raise SystemExit("--binary is required unless --refresh-report-only is used")

    binary = Path(args.binary)
    if not binary.is_file():
        raise SystemExit(f"Benchmark binary not found: {binary}")

    history = load_history(history_path)
    previous_entry = history["history"][-1] if history["history"] else None

    results: list[dict[str, Any]] = []
    for profile in PROFILE_DEFS:
        env = format_profile_env(profile["env"], args.threads, args.sink_bytes)
        report = benchmark_demo.build_report(
            binary=binary,
            iterations=max(1, args.iterations),
            runs=max(1, args.runs),
            scenario=args.scenario,
            extra_env=env,
        )
        comparison = comparison_for_profile(report, previous_entry, profile["name"])
        print_profile_result(profile, report, comparison)
        results.append(
            {
                "name": profile["name"],
                "label": profile["label"],
                "env": env,
                **report,
                "comparison_to_previous": comparison,
            }
        )

    entry = {
        "recorded_at_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat(),
        "git": git_metadata(repo_root),
        "benchmark_config": {
            "binary": str(binary),
            "build_dir": args.build_dir,
            "scenario": args.scenario,
            "iterations": max(1, args.iterations),
            "runs": max(1, args.runs),
            "threads": max(1, args.threads),
            "sink_bytes": max(1, args.sink_bytes),
            "cxx_flags": args.cxx_flags,
        },
        "results": results,
    }
    history["history"].append(entry)
    save_history(history_path, history)
    save_report(report_path, history)

    print(f"Saved benchmark history: {history_path}")
    print(f"Saved benchmark report: {report_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
