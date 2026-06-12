#!/usr/bin/env python3
"""
Run the reproducible ScopeTimer benchmark matrix and update repo benchmark reports.

The machine-readable history is stored in JSON so benchmark drift can be tracked
alongside code changes. A human-readable Markdown report is also refreshed as a
snapshot of the latest benchmark run plus its delta from the last benchmark
state checked in to main.
"""

from __future__ import annotations

import argparse
import json
import os
import platform
import plistlib
import shlex
import shutil
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
ASYNC_SINK_BYTES = "65536"
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
        "name": "standard_null_sink",
        "label": "Standard timer, null sink",
        "description": (
            "Standard timer routed to a no-op sink so the benchmark measures "
            "ScopeTimer framework overhead without output I/O."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "NULL",
            "SCOPE_TIMER_WALLTIME": "0",
        },
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
            "background writer instead of the calling thread, using a 64 KiB "
            "handoff size to reduce enqueue frequency."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "ASYNC",
            "SCOPE_TIMER_BENCH_SINK_BYTES": ASYNC_SINK_BYTES,
            "SCOPE_TIMER_BENCH_THREADS": THREADS_PLACEHOLDER,
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
    {
        "name": "hotpath_async_threaded",
        "label": "Hot-path timer, async sink",
        "description": (
            "Lowest-overhead profile: hot-path timer format plus the async sink, "
            "measured under the threaded stress workload with a 64 KiB async "
            "handoff size."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "ASYNC",
            "SCOPE_TIMER_BENCH_SINK_BYTES": ASYNC_SINK_BYTES,
            "SCOPE_TIMER_BENCH_THREADS": THREADS_PLACEHOLDER,
            "SCOPE_TIMER_BENCH_TIMER": "HOTPATH",
            "SCOPE_TIMER_WALLTIME": "0",
        },
    },
    {
        "name": "hotpath_null_sink",
        "label": "Hot-path timer, null sink",
        "description": (
            "Hot-path timer routed to the no-op sink so the benchmark shows "
            "the floor for ScopeTimer's own bookkeeping without output I/O."
        ),
        "env": {
            "SCOPE_TIMER_BENCH_SINK": "NULL",
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
        default="-O3",
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


def run_command(args: list[str]) -> str:
    completed = subprocess.run(
        args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        return ""
    return completed.stdout.strip()


def int_or_none(value: str | None) -> int | None:
    if value is None:
        return None
    try:
        return int(value.strip())
    except ValueError:
        return None


def human_bytes(value: int | None) -> str:
    if value is None:
        return "n/a"
    units = ("B", "KiB", "MiB", "GiB", "TiB", "PiB")
    amount = float(value)
    unit = units[0]
    for unit in units:
        if abs(amount) < 1024.0 or unit == units[-1]:
            break
        amount /= 1024.0
    if unit == "B":
        return f"{int(amount)} {unit}"
    return f"{amount:.2f} {unit}"


def sysctl_value(name: str) -> str | None:
    value = run_command(["sysctl", "-n", name])
    return value if value else None


def total_memory_bytes() -> int | None:
    memsize = int_or_none(sysctl_value("hw.memsize"))
    if memsize is not None:
        return memsize

    if hasattr(os, "sysconf"):
        try:
            pages = os.sysconf("SC_PHYS_PAGES")
            page_size = os.sysconf("SC_PAGE_SIZE")
        except (OSError, ValueError):
            return None
        if isinstance(pages, int) and isinstance(page_size, int):
            return pages * page_size
    return None


def diskutil_metadata(mount_point: str | None) -> dict[str, Any]:
    if platform.system() != "Darwin" or not mount_point:
        return {}

    completed = subprocess.run(
        ["diskutil", "info", "-plist", mount_point],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if completed.returncode != 0 or not completed.stdout:
        return {}

    try:
        raw = plistlib.loads(completed.stdout)
    except (plistlib.InvalidFileException, ValueError):
        return {}

    selected_keys = {
        "APFSContainerReference": "apfs_container",
        "BusProtocol": "bus_protocol",
        "DeviceIdentifier": "device_identifier",
        "DeviceNode": "device_node",
        "FilesystemName": "filesystem_name",
        "FilesystemType": "filesystem_type",
        "Internal": "internal",
        "MediaName": "media_name",
        "MediaType": "media_type",
        "MountPoint": "mount_point",
        "SMARTStatus": "smart_status",
        "SolidState": "solid_state",
        "VolumeName": "volume_name",
    }
    return {
        target_key: raw[source_key]
        for source_key, target_key in selected_keys.items()
        if source_key in raw
    }


def filesystem_metadata(path: Path) -> dict[str, Any]:
    metadata: dict[str, Any] = {"path": str(path)}
    usage = shutil.disk_usage(path)
    metadata.update(
        {
            "total_bytes": usage.total,
            "used_bytes": usage.used,
            "free_bytes": usage.free,
            "total": human_bytes(usage.total),
            "used": human_bytes(usage.used),
            "free": human_bytes(usage.free),
        }
    )

    df_output = run_command(["df", "-kP", str(path)])
    lines = df_output.splitlines()
    if len(lines) >= 2:
        parts = lines[1].split(maxsplit=5)
        if len(parts) >= 6:
            metadata.update(
                {
                    "filesystem": parts[0],
                    "blocks_1024": int_or_none(parts[1]),
                    "used_1024": int_or_none(parts[2]),
                    "available_1024": int_or_none(parts[3]),
                    "capacity": parts[4],
                    "mount_point": parts[5],
                }
            )

    metadata.update(diskutil_metadata(metadata.get("mount_point")))
    return metadata


def machine_metadata(repo_root: Path) -> dict[str, Any]:
    physical_cores = int_or_none(sysctl_value("hw.physicalcpu"))
    logical_cores = int_or_none(sysctl_value("hw.logicalcpu")) or os.cpu_count()
    memory_bytes = total_memory_bytes()
    cpu_brand = sysctl_value("machdep.cpu.brand_string") or platform.processor() or "unknown"

    return {
        "system": {
            "platform": platform.platform(),
            "system": platform.system(),
            "release": platform.release(),
            "version": platform.version(),
            "machine": platform.machine(),
            "python": platform.python_version(),
        },
        "cpu": {
            "brand": cpu_brand,
            "physical_cores": physical_cores,
            "logical_cores": logical_cores,
            "architecture": platform.machine(),
        },
        "memory": {
            "total_bytes": memory_bytes,
            "total": human_bytes(memory_bytes),
        },
        "disk": filesystem_metadata(repo_root),
    }


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


def git_ref_exists(repo_root: Path, ref: str) -> bool:
    return bool(run_git(["rev-parse", "--verify", "--quiet", ref], repo_root))


def repo_relative_path(path: Path, repo_root: Path) -> str | None:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return None


def load_history_from_git_ref(path: Path, repo_root: Path, ref: str) -> dict[str, Any] | None:
    repo_path = repo_relative_path(path, repo_root)
    if not repo_path:
        return None

    completed = subprocess.run(
        ["git", "show", f"{ref}:{repo_path}"],
        cwd=repo_root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        return None

    raw = completed.stdout.strip()
    if not raw:
        return {"schema_version": SCHEMA_VERSION, "history": []}

    data = json.loads(raw)
    if data.get("schema_version") != SCHEMA_VERSION:
        raise SystemExit(
            f"Unsupported benchmark history schema in {path} at {ref}: {data.get('schema_version')!r}"
        )
    if not isinstance(data.get("history"), list):
        raise SystemExit(f"Invalid benchmark history format in {path} at {ref}")
    return data


def resolve_main_baseline_ref(repo_root: Path, branch_name: str) -> str | None:
    if branch_name == "main":
        return "HEAD"

    for candidate in ("origin/main", "main"):
        if git_ref_exists(repo_root, candidate):
            return candidate
    return None


def resolve_main_baseline_entry(
    repo_root: Path,
    history_path: Path,
    branch_name: str,
) -> tuple[dict[str, Any] | None, str | None]:
    baseline_ref = resolve_main_baseline_ref(repo_root, branch_name)
    if not baseline_ref:
        return None, None

    baseline_history = load_history_from_git_ref(history_path, repo_root, baseline_ref)
    if not baseline_history:
        return None, baseline_ref

    baseline_entries = baseline_history.get("history", [])
    if not baseline_entries:
        return None, baseline_ref

    return baseline_entries[-1], baseline_ref


def comparison_reference_metadata(
    baseline_entry: dict[str, Any] | None,
    baseline_ref: str | None,
) -> dict[str, Any] | None:
    if not baseline_entry:
        return None

    baseline_git = baseline_entry.get("git", {})
    return {
        "kind": "last_checked_in_main",
        "resolved_ref": baseline_ref,
        "recorded_at_utc": baseline_entry.get("recorded_at_utc"),
        "commit": baseline_git.get("commit"),
        "short_commit": baseline_git.get("short_commit"),
        "branch": baseline_git.get("branch"),
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
            f"and a default `sink_bytes={config.get('sink_bytes', DEFAULT_REPORT_CONFIG['sink_bytes'])}` "
            "for profiles that do not override it below."
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


def format_ns_from_us(value: float | None) -> str:
    if value is None:
        return "n/a"
    return f"{value * 1000.0:.3f}ns"


def format_env_summary(env: dict[str, str] | None) -> str:
    if not env:
        return "default"
    return ", ".join(f"`{key}={value}`" for key, value in sorted(env.items()))


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


def speed_profile_summary(profile: dict[str, Any]) -> dict[str, Any]:
    per_record_us = profile.get("approx_per_record_us")
    per_record_ns = None
    if per_record_us is not None:
        per_record_ns = float(per_record_us) * 1000.0

    return {
        "name": profile.get("name"),
        "label": profile.get("label", profile.get("name", "unknown")),
        "env": profile.get("env", {}),
        "approx_per_record_us": per_record_us,
        "approx_per_record_ns": per_record_ns,
        "delta_mean_s": profile.get("delta_mean_s"),
        "enabled_mean_s": profile.get("enabled_mean_s"),
        "enabled_log_lines": profile.get("enabled_log_lines", 0),
    }


def build_speed_summary(results: list[dict[str, Any]]) -> dict[str, Any]:
    profile_summaries = [speed_profile_summary(profile) for profile in results]
    per_record_profiles = [
        profile for profile in profile_summaries
        if profile.get("approx_per_record_us") is not None
    ]
    fastest = None
    if per_record_profiles:
        fastest = min(per_record_profiles, key=lambda profile: float(profile["approx_per_record_us"]))

    return {
        "fastest_profile": fastest,
        "profiles": profile_summaries,
    }


def render_speed_breakdown_lines(summary: dict[str, Any]) -> list[str]:
    fastest = summary.get("fastest_profile")
    lines = ["", "## Current speed breakdown", ""]
    if fastest:
        per_record_us = fastest.get("approx_per_record_us")
        lines.extend(
            [
                (
                    "- Fastest measured configuration: "
                    f"{fastest.get('label', 'unknown')} at "
                    f"`{format_us(per_record_us)}/record` "
                    f"(`{format_ns_from_us(per_record_us)}/record`)."
                ),
                f"- Fastest configuration settings: {format_env_summary(fastest.get('env', {}))}.",
                "",
            ]
        )
    else:
        lines.extend(["- Fastest measured configuration: unavailable.", ""])

    lines.extend(
        [
            "| Configuration | Per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |",
            "| --- | --- | --- | --- | --- | --- |",
        ]
    )
    for profile in summary.get("profiles", []):
        per_record_us = profile.get("approx_per_record_us")
        lines.append(
            "| "
            f"{profile.get('label', 'unknown')} | "
            f"`{format_us(per_record_us)}` | "
            f"`{format_ns_from_us(per_record_us)}` | "
            f"`{format_seconds(profile.get('delta_mean_s'))}` | "
            f"`{format_seconds(profile.get('enabled_mean_s'))}` | "
            f"{format_env_summary(profile.get('env', {}))} |"
        )
    return lines


def render_machine_lines(machine: dict[str, Any]) -> list[str]:
    system = machine.get("system", {})
    cpu = machine.get("cpu", {})
    memory = machine.get("memory", {})
    disk = machine.get("disk", {})

    disk_detail_parts = []
    for key in ("filesystem", "filesystem_type", "device_node", "mount_point", "capacity"):
        if disk.get(key) not in (None, ""):
            disk_detail_parts.append(f"{key}=`{disk[key]}`")
    for key in ("solid_state", "internal", "smart_status", "bus_protocol"):
        if disk.get(key) not in (None, ""):
            disk_detail_parts.append(f"{key}=`{disk[key]}`")

    return [
        "",
        "## Benchmark host",
        "",
        (
            "- System: "
            f"`{system.get('platform', 'unknown')}` "
            f"({system.get('machine', 'unknown')}), Python `{system.get('python', 'unknown')}`."
        ),
        (
            "- CPU: "
            f"`{cpu.get('brand', 'unknown')}`, "
            f"physical cores `{cpu.get('physical_cores', 'unknown')}`, "
            f"logical cores `{cpu.get('logical_cores', 'unknown')}`."
        ),
        (
            "- Memory: "
            f"`{memory.get('total', 'n/a')}` "
            f"({memory.get('total_bytes', 'n/a')} bytes)."
        ),
        (
            "- Disk: "
            f"`{disk.get('total', 'n/a')}` total, "
            f"`{disk.get('free', 'n/a')}` free, "
            f"`{disk.get('used', 'n/a')}` used."
        ),
        f"- Disk details: {', '.join(disk_detail_parts) if disk_detail_parts else 'n/a'}.",
    ]


def render_report(
    history: dict[str, Any],
    repo_root: Path,
    history_path: Path,
) -> str:
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

    git = latest.get("git", {})
    config = latest.get("benchmark_config", {})
    comparison_reference = latest.get("comparison_reference")
    if not comparison_reference:
        baseline_entry, baseline_ref = resolve_main_baseline_entry(
            repo_root,
            history_path,
            git.get("branch", "unknown"),
        )
        comparison_reference = comparison_reference_metadata(baseline_entry, baseline_ref)

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

    if comparison_reference:
        lines.extend(
            [
                (
                    "- Comparison baseline: last benchmark checked in to `main`: "
                    f"`{comparison_reference.get('recorded_at_utc', 'unknown')}` "
                    f"on `{comparison_reference.get('short_commit', 'unknown')}`"
                ),
                "- Delta source: per-record overhead when available, otherwise mean overhead.",
            ]
        )
    else:
        lines.append("- Comparison baseline: no benchmark recorded on `main` yet.")

    machine = latest.get("machine")
    if machine:
        lines.extend(render_machine_lines(machine))

    speed_summary = latest.get("speed_summary") or build_speed_summary(latest.get("results", []))
    if speed_summary:
        lines.extend(render_speed_breakdown_lines(speed_summary))

    lines.extend(
        [
            "",
            "## Profile results",
            "",
            "| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |",
            "| --- | --- | --- | --- | --- | --- | --- |",
        ]
    )

    for profile in latest.get("results", []):
        comparison = profile.get("comparison_to_main_baseline", profile.get("comparison_to_previous", {}))
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


def save_report(
    path: Path,
    history: dict[str, Any],
    repo_root: Path,
    history_path: Path,
) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        handle.write(render_report(history, repo_root, history_path).rstrip())
        handle.write("\n")


def comparison_for_profile(
    current_report: dict[str, Any],
    baseline_entry: dict[str, Any] | None,
    profile_name: str,
) -> dict[str, Any]:
    if not baseline_entry:
        return {
            "indicator": "baseline",
            "status": "baseline",
            "summary": "main baseline unavailable",
        }

    previous_profiles = {
        profile.get("name"): profile for profile in baseline_entry.get("results", [])
    }
    previous_profile = previous_profiles.get(profile_name)
    if not previous_profile:
        return {
            "indicator": "baseline",
            "status": "baseline",
            "summary": "main baseline unavailable for this profile",
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
        f"{status} vs {baseline_entry['git']['short_commit']} "
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
        "previous_recorded_at_utc": baseline_entry.get("recorded_at_utc"),
        "previous_commit": baseline_entry.get("git", {}).get("commit"),
        "previous_short_commit": baseline_entry.get("git", {}).get("short_commit"),
        "tolerance_pct": NOISE_TOLERANCE_PCT,
    }


def print_profile_result(profile: dict[str, Any], report: dict[str, Any], comparison: dict[str, Any]) -> None:
    print(f"=== {profile['label']} ===")
    benchmark_demo.print_text_report(report)
    print(f"comparison:           {comparison['summary']}")


def main() -> None:
    args = parse_args()
    repo_root = Path(__file__).resolve().parent.parent
    history_path = Path(args.history_file)
    report_path = Path(args.report_file)

    if args.refresh_report_only:
        history = load_history(history_path)
        save_report(report_path, history, repo_root, history_path)
        print(f"Saved benchmark report: {report_path}")
        return

    if not args.binary:
        raise SystemExit("--binary is required unless --refresh-report-only is used")

    binary = Path(args.binary)
    if not binary.is_file():
        raise SystemExit(f"Benchmark binary not found: {binary}")

    history = load_history(history_path)
    current_git = git_metadata(repo_root)
    baseline_entry, baseline_ref = resolve_main_baseline_entry(
        repo_root,
        history_path,
        current_git.get("branch", "unknown"),
    )

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
        comparison = comparison_for_profile(report, baseline_entry, profile["name"])
        print_profile_result(profile, report, comparison)
        results.append(
            {
                "name": profile["name"],
                "label": profile["label"],
                "env": env,
                **report,
                "comparison_to_main_baseline": comparison,
            }
        )

    entry = {
        "recorded_at_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat(),
        "git": current_git,
        "comparison_reference": comparison_reference_metadata(baseline_entry, baseline_ref),
        "machine": machine_metadata(repo_root),
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
        "speed_summary": build_speed_summary(results),
        "results": results,
    }
    history["history"].append(entry)
    save_history(history_path, history)
    save_report(report_path, history, repo_root, history_path)

    print(f"Saved benchmark history: {history_path}")
    print(f"Saved benchmark report: {report_path}")


if __name__ == "__main__":
    main()
