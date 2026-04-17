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

- Recorded at: `2026-04-17T20:02:05+00:00`
- Commit: `bb2f7e3`
- Subject: chore(sonar): pin Python analysis version
- Branch: `main`
- Dirty worktree: no
- Config: `binary=/Users/sclarke/github/ScopeTimer/build-bench/Benchmark`, `build_dir=/Users/sclarke/github/ScopeTimer/build-bench`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison baseline: last benchmark checked in to `main`: `2026-04-17T19:12:13+00:00` on `01f5457`
- Delta source: per-record overhead when available, otherwise mean overhead.

| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.322us` | `0.101501s` (643.412%) | `0.117291s` | `76807` | +0.075us (+6.0%) | slower |
| Standard timer, wall time disabled | `1.155us` | `0.088727s` (591.690%) | `0.103742s` | `76807` | +0.011us (+1.0%) | unchanged |
| Standard timer, null sink | `n/a` | `0.002393s` (17.152%) | `0.016372s` | `0` | +0.000157s (+7.0%) | slower |
| Standard timer, buffered sink | `0.046us` | `0.003565s` (25.370%) | `0.017677s` | `76807` | +0.004us (+8.7%) | slower |
| Standard timer, buffered sink (threaded stress) | `0.262us` | `0.080608s` (531.010%) | `0.095796s` | `307207` | -0.005us (-1.8%) | unchanged |
| Standard timer, async sink | `0.018us` | `0.005551s` (36.051%) | `0.020966s` | `307207` | -0.001us (-3.1%) | faster |
| Hot-path timer, async sink | `0.015us` | `0.004685s` (31.185%) | `0.019806s` | `307207` | -0.001us (-6.9%) | faster |
| Hot-path timer, null sink | `n/a` | `0.001255s` (9.193%) | `0.014940s` | `0` | +0.000047s (+3.9%) | slower |

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
