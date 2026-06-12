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

- Recorded at: `2026-06-12T15:25:57+00:00`
- Commit: `c2ef1ca`
- Subject: test(scopetimer): improve coverage and pin sonar actions
- Branch: `develop`
- Dirty worktree: no
- Config: `binary=/Users/sclarke/github/ScopeTimer/build-bench/Benchmark`, `build_dir=/Users/sclarke/github/ScopeTimer/build-bench`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison baseline: last benchmark checked in to `main`: `2026-04-17T20:10:04+00:00` on `62e513c`
- Delta source: per-record overhead when available, otherwise mean overhead.

| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.672us` | `0.128459s` (435.106%) | `0.158093s` | `76807` | +0.402us (+31.6%) | slower |
| Standard timer, wall time disabled | `1.489us` | `0.114355s` (391.310%) | `0.143587s` | `76807` | +0.295us (+24.7%) | slower |
| Standard timer, null sink | `n/a` | `0.002388s` (8.512%) | `0.032261s` | `0` | +0.000105s (+4.6%) | slower |
| Standard timer, buffered sink | `0.083us` | `0.006410s` (22.644%) | `0.035056s` | `76807` | +0.035us (+73.3%) | slower |
| Standard timer, buffered sink (threaded stress) | `0.380us` | `0.116667s` (382.446%) | `0.147192s` | `307207` | +0.111us (+41.1%) | slower |
| Standard timer, async sink | `0.064us` | `0.019537s` (63.475%) | `0.050508s` | `307207` | +0.043us (+211.0%) | slower |
| Hot-path timer, async sink | `0.049us` | `0.015100s` (50.277%) | `0.045145s` | `307207` | +0.032us (+187.6%) | slower |
| Hot-path timer, null sink | `n/a` | `0.003397s` (11.857%) | `0.032008s` | `0` | +0.002114s (+164.9%) | slower |

Full historical results remain in
`benchmarks/demo_benchmark_history.json`.

## Profile reference

The commands below rerun one profile at a time through
`scripts/benchmark_demo.py`. The harness handles the alternating
`SCOPE_TIMER=0` and `SCOPE_TIMER=1` passes for you; the `--env` flags
shown here are only the profile-specific knobs.

These examples use `binary=./build-bench/Benchmark`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, and a default `sink_bytes=4096` for profiles that do not override it below.

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

### Standard timer, null sink

Standard timer routed to a no-op sink so the benchmark measures ScopeTimer framework overhead without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_WALLTIME=0
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

Multi-threaded run with the async sink so flush work moves to the background writer instead of the calling thread, using a 64 KiB handoff size to reduce enqueue frequency.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, async sink

Lowest-overhead profile: hot-path timer format plus the async sink, measured under the threaded stress workload with a 64 KiB async handoff size.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, null sink

Hot-path timer routed to the no-op sink so the benchmark shows the floor for ScopeTimer's own bookkeeping without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-bench/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```
