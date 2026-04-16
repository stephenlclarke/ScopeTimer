<!-- markdownlint-disable MD013 -->

# Benchmark Results

Human-readable ScopeTimer benchmark reporting. The machine-readable
history lives in `benchmarks/demo_benchmark_history.json`.

## Reproducing the benchmarks

```bash
cmake -S . -B build-review
cmake --build build-review --target demo_benchmark
cmake --build build-review --target demo_benchmark_matrix
```

`demo_benchmark` runs the direct enabled-vs-disabled benchmark for
`example/Benchmark.cpp`. `demo_benchmark_matrix` runs the full profile
matrix, appends `benchmarks/demo_benchmark_history.json`, and refreshes
this file with the latest snapshot.

## Current benchmark snapshot

- Recorded at: `2026-04-16T10:45:23+00:00`
- Commit: `6944489`
- Subject: perf: avoid label allocs in hot path and simplify buffer use
- Branch: `develop`
- Dirty worktree: yes
- Config: `binary=/Users/sclarke/github/ScopeTimer/build-bench/Benchmark`, `build_dir=/Users/sclarke/github/ScopeTimer/build-bench`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O2`
- Previous benchmark: `2026-04-16T10:39:49+00:00` on `6944489`
- Delta source: per-record overhead when available, otherwise mean overhead.

| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs previous | Status |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.467us` | `0.112667s` (626.768%) | `0.130653s` | `76807` | -0.089us (-5.7%) | faster |
| Standard timer, wall time disabled | `1.536us` | `0.117945s` (686.597%) | `0.135142s` | `76807` | +0.052us (+3.5%) | slower |
| Standard timer, buffered sink | `0.073us` | `0.005589s` (33.574%) | `0.022281s` | `76807` | -0.006us (-7.8%) | faster |
| Standard timer, buffered sink (threaded stress) | `1.903us` | `0.584532s` (3251.913%) | `0.602755s` | `307207` | -0.067us (-3.4%) | faster |
| Standard timer, async sink | `0.184us` | `0.056567s` (323.844%) | `0.074166s` | `307207` | +0.098us (+114.5%) | slower |
| Hot-path timer, async sink | `0.029us` | `0.008896s` (51.431%) | `0.026236s` | `307207` | +0.001us (+3.9%) | slower |

Full historical results remain in
`benchmarks/demo_benchmark_history.json`.

## Profile reference

The commands below rerun one profile at a time through
`scripts/benchmark_demo.py`. The harness handles the alternating
`SCOPE_TIMER=0` and `SCOPE_TIMER=1` passes for you; the `--env` flags
shown here are only the profile-specific knobs.

These examples use `binary=./build-bench/Benchmark`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, and `sink_bytes=4096`.

### Standard timer, default sink

Baseline ScopeTimer cost with the normal synchronous sink and wall-clock timestamps enabled.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8
```

### Standard timer, wall time disabled

Standard timer with `SCOPE_TIMER_WALLTIME=0` so the report shows the cost of dropping `start=` and `end=` timestamp formatting.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink

Single-thread run with the thread-buffered sink enabled to show how much caller-thread write overhead falls when flushes are batched.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink (threaded stress)

Multi-threaded buffered run that stresses contention and cross-thread flush behavior under the standard timer format.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_BENCH_SINK_BYTES=4096 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, async sink

Multi-threaded run with the async sink so flush work moves to the background writer instead of the calling thread.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=4096 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, async sink

Lowest-overhead profile: hot-path timer format plus the async sink, measured under the threaded stress workload.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=4096 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```
