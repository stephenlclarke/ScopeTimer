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
import re
import statistics
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from pathlib import Path


MAX_ITERATIONS = 100
MAX_RUNS = 1000
MAX_BENCHMARK_THREADS = 256
MAX_BENCHMARK_SINK_BYTES = 64 * 1024 * 1024
HOTPATH_RECORDS_PER_ROUND = 256
HOTPATH_ROUNDS_PER_ITERATION = 12
INSTRUMENTATION_PROBE_ARGUMENT = "--instrumentation-status"
INSTRUMENTATION_PROBE_ENABLED = "ScopeTimerBenchmark protocol=1 instrumentation=enabled"
INSTRUMENTATION_PROBE_DISABLED = "ScopeTimerBenchmark protocol=1 instrumentation=disabled"
INSTRUMENTATION_PROBE_TIMEOUT_SECONDS = 5.0
RESERVED_ENV_KEYS = frozenset({"SCOPE_TIMER", "SCOPE_TIMER_DIR"})
PROFILE_ENV_KEYS = frozenset(
    {
        "SCOPE_TIMER_FLUSH_N",
        "SCOPE_TIMER_FORMAT",
        "SCOPE_TIMER_WALLTIME",
        "SCOPE_TIMER_BENCH_SINK",
        "SCOPE_TIMER_BENCH_SINK_BYTES",
        "SCOPE_TIMER_BENCH_THREADS",
        "SCOPE_TIMER_BENCH_TIMER",
    }
)
CONTROLLED_ENV_KEYS = RESERVED_ENV_KEYS | PROFILE_ENV_KEYS
ENV_KEY_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*\Z")


class BenchmarkConfigurationError(ValueError):
    """Raised when benchmark arguments cannot describe a safe, comparable run."""


class BenchmarkInvariantError(RuntimeError):
    """Raised when the benchmark executable violates the harness contract."""


@dataclass(frozen=True)
class BenchmarkConfig:
    binary: Path
    iterations: int
    runs: int
    scenario: str
    extra_env: dict[str, str]
    emit_json: bool


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
        help="Additional environment override passed to the Benchmark process",
    )
    parser.add_argument("--json", action="store_true", help="Emit machine-readable JSON instead of text")
    return parser.parse_args()


def parse_extra_env(raw_items: list[str]) -> dict[str, str]:
    parsed: dict[str, str] = {}
    seen_keys: set[str] = set()
    for item in raw_items:
        if "=" not in item:
            raise BenchmarkConfigurationError(
                f"invalid --env entry {item!r}: expected KEY=VALUE"
            )
        key, value = item.split("=", 1)
        if not key:
            raise BenchmarkConfigurationError(f"invalid --env entry {item!r}: empty key")
        uppercase_key = key.upper()
        canonical_key = uppercase_key if uppercase_key in CONTROLLED_ENV_KEYS else key
        if canonical_key in seen_keys:
            raise BenchmarkConfigurationError(
                f"environment variable {canonical_key} may only be specified once"
            )
        seen_keys.add(canonical_key)
        parsed[key] = value
    return normalize_extra_env(parsed)


def bounded_positive(value: int, name: str, maximum: int) -> int:
    if value < 1 or value > maximum:
        raise BenchmarkConfigurationError(
            f"{name} must be between 1 and {maximum}; got {value}"
        )
    return value


def parse_bounded_decimal(value: str, name: str, maximum: int) -> int:
    if not value or not value.isascii() or not value.isdecimal():
        raise BenchmarkConfigurationError(
            f"{name} must be a whole decimal integer; got {value!r}"
        )
    return bounded_positive(int(value), name, maximum)


def normalize_extra_env(extra_env: dict[str, str]) -> dict[str, str]:
    normalized: dict[str, str] = {}
    for key, value in extra_env.items():
        if not isinstance(key, str) or not ENV_KEY_RE.fullmatch(key):
            raise BenchmarkConfigurationError(f"invalid environment variable name: {key!r}")
        uppercase_key = key.upper()
        if uppercase_key in RESERVED_ENV_KEYS:
            raise BenchmarkConfigurationError(
                f"{key} is controlled by the benchmark harness and cannot be overridden"
            )
        if not isinstance(value, str) or "\0" in value:
            raise BenchmarkConfigurationError(f"invalid value for environment variable {key}")
        canonical_key = uppercase_key if uppercase_key in PROFILE_ENV_KEYS else key
        if canonical_key in normalized:
            raise BenchmarkConfigurationError(
                f"environment variable {canonical_key} may only be specified once"
            )
        normalized[canonical_key] = value

    if "SCOPE_TIMER_BENCH_THREADS" in normalized:
        normalized["SCOPE_TIMER_BENCH_THREADS"] = str(
            parse_bounded_decimal(
                normalized["SCOPE_TIMER_BENCH_THREADS"],
                "SCOPE_TIMER_BENCH_THREADS",
                MAX_BENCHMARK_THREADS,
            )
        )
    if "SCOPE_TIMER_BENCH_SINK_BYTES" in normalized:
        normalized["SCOPE_TIMER_BENCH_SINK_BYTES"] = str(
            parse_bounded_decimal(
                normalized["SCOPE_TIMER_BENCH_SINK_BYTES"],
                "SCOPE_TIMER_BENCH_SINK_BYTES",
                MAX_BENCHMARK_SINK_BYTES,
            )
        )
    if "SCOPE_TIMER_BENCH_SINK" in normalized:
        sink = normalized["SCOPE_TIMER_BENCH_SINK"].upper()
        if sink not in {"DEFAULT", "BUFFERED", "ASYNC", "NULL", "NOOP"}:
            raise BenchmarkConfigurationError(
                "SCOPE_TIMER_BENCH_SINK must be DEFAULT, BUFFERED, ASYNC, NULL, or NOOP"
            )
        normalized["SCOPE_TIMER_BENCH_SINK"] = sink
    if "SCOPE_TIMER_BENCH_TIMER" in normalized:
        timer = normalized["SCOPE_TIMER_BENCH_TIMER"].upper()
        if timer not in {"DEFAULT", "STANDARD", "HOTPATH", "FAST"}:
            raise BenchmarkConfigurationError(
                "SCOPE_TIMER_BENCH_TIMER must be DEFAULT, STANDARD, HOTPATH, or FAST"
            )
        normalized["SCOPE_TIMER_BENCH_TIMER"] = timer
    return normalized


def normalize_args(args: argparse.Namespace) -> BenchmarkConfig:
    scenario = str(args.scenario)
    if scenario != "hotpath-bench":
        raise BenchmarkConfigurationError(
            f"unsupported benchmark scenario {scenario!r}; expected 'hotpath-bench'"
        )
    return BenchmarkConfig(
        binary=Path(args.binary).expanduser().resolve(),
        iterations=bounded_positive(args.iterations, "iterations", MAX_ITERATIONS),
        runs=bounded_positive(args.runs, "runs", MAX_RUNS),
        scenario=scenario,
        extra_env=parse_extra_env(args.env),
        emit_json=bool(args.json),
    )


def expected_record_count(iterations: int, extra_env: dict[str, str]) -> int:
    thread_count = int(extra_env.get("SCOPE_TIMER_BENCH_THREADS", "1"))
    record_timers = (
        iterations
        * iterations
        * HOTPATH_ROUNDS_PER_ITERATION
        * HOTPATH_RECORDS_PER_ROUND
        * thread_count
    )
    # One parse timer and one main timer per process, plus one benchmark timer
    # for each outer iteration.
    return record_timers + iterations + 2


def profile_emits_output(extra_env: dict[str, str]) -> bool:
    return extra_env.get("SCOPE_TIMER_BENCH_SINK", "DEFAULT") not in {"NULL", "NOOP"}


def validate_run_result(
    result: dict[str, float | int | bool],
    *,
    enabled: bool,
    emits_output: bool,
    expected_records: int,
    description: str,
) -> None:
    log_exists = bool(result["log_exists"])
    log_lines = int(result["log_lines"])
    log_bytes = int(result["log_bytes"])
    if not enabled:
        if log_exists or log_lines != 0 or log_bytes != 0:
            raise BenchmarkInvariantError(
                f"{description}: disabled run unexpectedly emitted ScopeTimer output"
            )
        return

    if emits_output:
        if not log_exists:
            raise BenchmarkInvariantError(
                f"{description}: enabled output-producing run did not create ScopeTimer.log; "
                "the binary may have been compiled with NDEBUG"
            )
        if log_lines != expected_records:
            raise BenchmarkInvariantError(
                f"{description}: enabled run emitted {log_lines} records; "
                f"expected {expected_records}"
            )
    elif log_exists or log_lines != 0 or log_bytes != 0:
        raise BenchmarkInvariantError(
            f"{description}: null-sink run unexpectedly emitted file output"
        )


def probe_benchmark_binary(binary: Path) -> dict[str, object]:
    try:
        completed = subprocess.run(
            [str(binary), INSTRUMENTATION_PROBE_ARGUMENT],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
            timeout=INSTRUMENTATION_PROBE_TIMEOUT_SECONDS,
        )
    except subprocess.TimeoutExpired as error:
        raise BenchmarkInvariantError(
            f"benchmark identity probe timed out after {INSTRUMENTATION_PROBE_TIMEOUT_SECONDS:.0f}s"
        ) from error
    except OSError as error:
        raise BenchmarkInvariantError(
            f"could not execute benchmark identity probe: {error}"
        ) from error

    status = completed.stdout.strip()
    stderr = completed.stderr.strip()
    if completed.returncode != 0:
        raise BenchmarkInvariantError(
            f"benchmark identity probe exited with {completed.returncode}"
        )
    if stderr:
        raise BenchmarkInvariantError(
            f"benchmark identity probe wrote unexpected stderr: {stderr!r}"
        )
    if status == INSTRUMENTATION_PROBE_DISABLED:
        raise BenchmarkInvariantError(
            "benchmark identifies as ScopeTimer Benchmark but instrumentation is disabled by NDEBUG"
        )
    if status != INSTRUMENTATION_PROBE_ENABLED:
        raise BenchmarkInvariantError(
            "binary did not return the ScopeTimer Benchmark instrumentation protocol response"
        )
    return {
        "protocol": 1,
        "identity": "ScopeTimerBenchmark",
        "instrumentation": "enabled",
    }


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
    for key in list(env):
        if key.upper() in CONTROLLED_ENV_KEYS:
            env.pop(key)
    env["SCOPE_TIMER"] = "1" if enabled else "0"
    env["SCOPE_TIMER_DIR"] = str(log_dir)
    env.update(extra_env)

    start = time.perf_counter()
    try:
        completed = subprocess.run(
            [str(binary), f"--iterations={iterations}", f"--scenario={scenario}"],
            env=env,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
    except OSError as error:
        raise BenchmarkInvariantError(f"could not execute benchmark binary: {error}") from error
    elapsed = time.perf_counter() - start
    if completed.returncode != 0:
        raise BenchmarkInvariantError(
            f"benchmark exited with {completed.returncode} (enabled={enabled})"
        )

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
    iterations = bounded_positive(iterations, "iterations", MAX_ITERATIONS)
    runs = bounded_positive(runs, "runs", MAX_RUNS)
    if scenario != "hotpath-bench":
        raise BenchmarkConfigurationError(
            f"unsupported benchmark scenario {scenario!r}; expected 'hotpath-bench'"
        )
    extra_env = normalize_extra_env(extra_env)
    expected_records = expected_record_count(iterations, extra_env)
    emits_output = profile_emits_output(extra_env)
    instrumentation_probe = probe_benchmark_binary(binary)

    with tempfile.TemporaryDirectory(prefix="scopetimer-demo-bench-") as tmpdir:
        log_dir = Path(tmpdir)

        for enabled in (False, True):
            warmup = run_once(binary, iterations, scenario, enabled, log_dir, extra_env)
            validate_run_result(
                warmup,
                enabled=enabled,
                emits_output=emits_output,
                expected_records=expected_records,
                description=f"warmup {'enabled' if enabled else 'disabled'}",
            )

        disabled_runs: list[dict[str, float | int | bool]] = []
        enabled_runs: list[dict[str, float | int | bool]] = []
        pair_orders: list[list[str]] = []
        run_sequence: list[str] = []
        for run_index in range(runs):
            order = (False, True) if run_index % 2 == 0 else (True, False)
            pair_orders.append(["enabled" if enabled else "disabled" for enabled in order])
            pair_results: dict[bool, dict[str, float | int | bool]] = {}
            for enabled in order:
                mode = "enabled" if enabled else "disabled"
                run_sequence.append(mode)
                result = run_once(binary, iterations, scenario, enabled, log_dir, extra_env)
                validate_run_result(
                    result,
                    enabled=enabled,
                    emits_output=emits_output,
                    expected_records=expected_records,
                    description=f"measurement pair {run_index + 1} {mode}",
                )
                pair_results[enabled] = result
            disabled_runs.append(pair_results[False])
            enabled_runs.append(pair_results[True])

    disabled_times = [entry["seconds"] for entry in disabled_runs]
    enabled_times = [entry["seconds"] for entry in enabled_runs]
    deltas = [enabled - disabled for disabled, enabled in zip(disabled_times, enabled_times)]
    overhead_pct = [((enabled / disabled) - 1.0) * 100.0 for disabled, enabled in zip(disabled_times, enabled_times)]

    final_enabled = enabled_runs[-1]
    line_count = int(final_enabled["log_lines"])
    per_record_us = (statistics.mean(deltas) / expected_records) * 1_000_000.0

    return {
        "binary": str(binary),
        "scenario": scenario,
        "iterations": iterations,
        "runs": runs,
        "env": dict(sorted(extra_env.items())),
        "instrumentation_probe": instrumentation_probe,
        "pair_orders": pair_orders,
        "run_sequence": run_sequence,
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
        "expected_enabled_records": expected_records,
        "per_record_denominator": expected_records,
        "profile_emits_output": emits_output,
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
    probe = report["instrumentation_probe"]
    print(
        "instrumentation:      "
        f"{probe['identity']} protocol={probe['protocol']} {probe['instrumentation']}"
    )
    print(f"disabled mean:        {report['disabled_mean_s']:.6f}s")
    print(f"enabled mean:         {report['enabled_mean_s']:.6f}s")
    print(f"mean overhead:        {report['delta_mean_s']:.6f}s ({report['overhead_mean_pct']:.3f}%)")
    print(f"disabled median:      {report['disabled_median_s']:.6f}s")
    print(f"enabled median:       {report['enabled_median_s']:.6f}s")
    print(f"median overhead:      {report['delta_median_s']:.6f}s ({report['overhead_median_pct']:.3f}%)")
    print(
        "enabled log output:   "
        f"{report['enabled_log_lines']} lines, {report['enabled_log_bytes']} bytes "
        f"({report['expected_enabled_records']} modeled records)"
    )
    if report["approx_per_record_us"] is not None:
        print(f"approx per record:    {report['approx_per_record_us']:.3f}us")
    print(f"disabled emits log:   {'yes' if report['disabled_log_exists'] else 'no'}")


def main() -> int:
    try:
        config = normalize_args(parse_args())
        if not config.binary.is_file():
            raise BenchmarkConfigurationError(f"benchmark binary not found: {config.binary}")

        report = build_report(
            config.binary,
            config.iterations,
            config.runs,
            config.scenario,
            config.extra_env,
        )
        if config.emit_json:
            print(json.dumps(report, indent=2))
        else:
            print_text_report(report)
        return 0
    except BenchmarkConfigurationError as error:
        print(f"benchmark_demo.py: error: {error}", file=sys.stderr)
        return 2
    except BenchmarkInvariantError as error:
        print(f"benchmark_demo.py: benchmark invariant failed: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
