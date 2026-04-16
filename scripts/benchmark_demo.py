#!/usr/bin/env python3
"""
Benchmark the dedicated ScopeTimer benchmark app with ScopeTimer enabled and disabled.

The script alternates disabled/enabled runs against the same optimized Benchmark
binary, writes enabled logs into a temporary directory, and reports the added
wall-clock cost plus a rough per-record estimate.
"""

from __future__ import annotations

import argparse
import json
import os
import statistics
import subprocess
import tempfile
import time
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, help="Path to the benchmark executable")
    parser.add_argument("--iterations", type=int, default=10, help="Benchmark --iterations value")
    parser.add_argument("--runs", type=int, default=12, help="Number of alternating disabled/enabled runs")
    parser.add_argument(
        "--scenario",
        default="hotpath-bench",
        help="Benchmark scenario to run, for example 'hotpath-bench'",
    )
    parser.add_argument(
        "--env",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="Additional environment override passed to the Demo process",
    )
    parser.add_argument("--json", action="store_true", help="Emit machine-readable JSON instead of text")
    return parser.parse_args()


def parse_extra_env(raw_items: list[str]) -> dict[str, str]:
    parsed: dict[str, str] = {}
    for item in raw_items:
        if "=" not in item:
            raise SystemExit(f"Invalid --env entry: {item!r} (expected KEY=VALUE)")
        key, value = item.split("=", 1)
        if not key:
            raise SystemExit(f"Invalid --env entry: {item!r} (empty key)")
        parsed[key] = value
    return parsed


def run_once(
    binary: Path,
    iterations: int,
    scenario: str,
    enabled: bool,
    log_dir: Path,
    extra_env: dict[str, str],
) -> dict[str, float | int | bool]:
    log_path = log_dir / "ScopeTimer.log"
    try:
        log_path.unlink()
    except FileNotFoundError:
        pass

    env = os.environ.copy()
    env["SCOPE_TIMER"] = "1" if enabled else "0"
    env["SCOPE_TIMER_DIR"] = str(log_dir)
    env.update(extra_env)

    start = time.perf_counter()
    completed = subprocess.run(
        [str(binary), f"--iterations={iterations}", f"--scenario={scenario}"],
        env=env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    elapsed = time.perf_counter() - start
    if completed.returncode != 0:
            raise RuntimeError(f"Benchmark exited with {completed.returncode} (enabled={enabled})")

    log_exists = log_path.exists()
    log_lines = 0
    log_bytes = 0
    if log_exists:
        log_bytes = log_path.stat().st_size
        with log_path.open("r", encoding="utf-8", errors="replace") as handle:
            log_lines = sum(1 for _ in handle)

    return {
        "seconds": elapsed,
        "log_exists": log_exists,
        "log_lines": log_lines,
        "log_bytes": log_bytes,
    }


def build_report(binary: Path, iterations: int, runs: int, scenario: str, extra_env: dict[str, str]) -> dict[str, object]:
    with tempfile.TemporaryDirectory(prefix="scopetimer-demo-bench-") as tmpdir:
        log_dir = Path(tmpdir)

        for enabled in (False, True):
            run_once(binary, iterations, scenario, enabled, log_dir, extra_env)

        disabled_runs: list[dict[str, float | int | bool]] = []
        enabled_runs: list[dict[str, float | int | bool]] = []
        for _ in range(runs):
            disabled_runs.append(run_once(binary, iterations, scenario, False, log_dir, extra_env))
            enabled_runs.append(run_once(binary, iterations, scenario, True, log_dir, extra_env))

    disabled_times = [entry["seconds"] for entry in disabled_runs]
    enabled_times = [entry["seconds"] for entry in enabled_runs]
    deltas = [enabled - disabled for disabled, enabled in zip(disabled_times, enabled_times)]
    overhead_pct = [((enabled / disabled) - 1.0) * 100.0 for disabled, enabled in zip(disabled_times, enabled_times)]

    final_enabled = enabled_runs[-1]
    line_count = int(final_enabled["log_lines"])
    per_record_us = None
    if line_count > 0:
        per_record_us = (statistics.mean(deltas) / line_count) * 1_000_000.0

    return {
        "binary": str(binary),
        "scenario": scenario,
        "iterations": iterations,
        "runs": runs,
        "env": dict(sorted(extra_env.items())),
        "disabled_mean_s": statistics.mean(disabled_times),
        "disabled_median_s": statistics.median(disabled_times),
        "enabled_mean_s": statistics.mean(enabled_times),
        "enabled_median_s": statistics.median(enabled_times),
        "delta_mean_s": statistics.mean(deltas),
        "delta_median_s": statistics.median(deltas),
        "overhead_mean_pct": statistics.mean(overhead_pct),
        "overhead_median_pct": statistics.median(overhead_pct),
        "disabled_min_s": min(disabled_times),
        "disabled_max_s": max(disabled_times),
        "enabled_min_s": min(enabled_times),
        "enabled_max_s": max(enabled_times),
        "enabled_log_lines": line_count,
        "enabled_log_bytes": int(final_enabled["log_bytes"]),
        "disabled_log_exists": bool(disabled_runs[-1]["log_exists"]),
        "approx_per_record_us": per_record_us,
    }


def print_text_report(report: dict[str, object]) -> None:
    print("ScopeTimer Benchmark")
    print(f"binary:               {report['binary']}")
    print(f"scenario:             {report['scenario']}")
    print(f"iterations per run:   {report['iterations']}")
    print(f"alternating runs:     {report['runs']}")
    if report["env"]:
        print(f"env overrides:        {report['env']}")
    print(f"disabled mean:        {report['disabled_mean_s']:.6f}s")
    print(f"enabled mean:         {report['enabled_mean_s']:.6f}s")
    print(f"mean overhead:        {report['delta_mean_s']:.6f}s ({report['overhead_mean_pct']:.3f}%)")
    print(f"disabled median:      {report['disabled_median_s']:.6f}s")
    print(f"enabled median:       {report['enabled_median_s']:.6f}s")
    print(f"median overhead:      {report['delta_median_s']:.6f}s ({report['overhead_median_pct']:.3f}%)")
    print(
        "enabled log output:   "
        f"{report['enabled_log_lines']} lines, {report['enabled_log_bytes']} bytes"
    )
    if report["approx_per_record_us"] is not None:
        print(f"approx per record:    {report['approx_per_record_us']:.3f}us")
    print(f"disabled emits log:   {'yes' if report['disabled_log_exists'] else 'no'}")


def main() -> int:
    args = parse_args()
    binary = Path(args.binary)
    if not binary.is_file():
        raise SystemExit(f"Benchmark binary not found: {binary}")

    report = build_report(
        binary,
        max(1, args.iterations),
        max(1, args.runs),
        args.scenario,
        parse_extra_env(args.env),
    )
    if args.json:
        print(json.dumps(report, indent=2))
    else:
        print_text_report(report)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
