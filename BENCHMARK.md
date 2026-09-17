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

Current runs measure elapsed time inside the executable using `steady_clock`, including sink setup and completed teardown. Subprocess startup and timeout polling are excluded. Timing method `steady-clock-v1` uses protocol 2 and fingerprint version 2; earlier measurements are retained in history but are not comparable.

## Current benchmark snapshot

- Recorded at: `2026-09-17T11:53:55+00:00`
- Commit: `c1a71f5`
- Subject: fix(scopetimer): simplify custom sink cadence
- Branch: `main`
- Dirty worktree: yes
- Config: `binary=./build-review/benchmark-build/Benchmark`, `build_dir=./build-review/benchmark-build`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Timing method: `steady-clock-v1`
- Comparison fingerprint: `7c04c62d99e3d433c2233a024f19e581949c0e3d0d6ec7742852326af45f904f`
- Comparison baseline: last benchmark checked in to `main`: `2026-07-25T13:12:36+00:00` on `2f6d4a3`
- Delta source: per-record overhead when available, otherwise mean overhead.

## Benchmark host

- System: `macOS-27.0-arm64-arm-64bit-Mach-O` (arm64), Python `3.14.7`.
- CPU: `Apple M5 Pro`, physical cores `18`, logical cores `18`.
- Memory: `24.00 GiB` (25769803776 bytes).
- Disk: `1.81 TiB` total, `603.23 GiB` free, `1.23 TiB` used.
- Disk details: filesystem=`/dev/disk3s1s1`, filesystem_type=`apfs`, device_node=`/dev/disk3s1s1`, mount_point=`/`, capacity=`68%`, solid_state=`True`, internal=`True`, smart_status=`Verified`, bus_protocol=`Apple Fabric`.

## Benchmark toolchain

- Compiler: `Apple clang version 21.0.0 (clang-2100.3.34.2)`.
- Compiler path: `/usr/bin/c++`.
- Benchmark binary SHA-256: `c6d5dbfd65bf034609aa0ac5923bde49b46f579a98c194d4495bc2d3cde2f934`.

## Current speed breakdown

- Fastest single-thread measured configuration: Hot-path timer, null sink at `0.011us/record` (`10.909ns/record`).
- Fastest single-thread configuration settings: `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0`.

| Configuration | Measurement | Cost per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.631us` | `1630.899ns` | `0.125264s` | `0.131675s` | default |
| Standard timer, wall time disabled | Single-thread estimate | `1.556us` | `1555.752ns` | `0.119493s` | `0.125907s` | `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, null sink | Single-thread estimate | `0.032us` | `31.582ns` | `0.002426s` | `0.008862s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink | Single-thread estimate | `0.054us` | `53.673ns` | `0.004122s` | `0.010732s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.264us` | `264.241ns` | `0.081177s` | `0.088350s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_BENCH_SINK_BYTES=4096`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, async sink | Aggregate throughput cost | `0.015us` | `15.056ns` | `0.004625s` | `0.011864s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, async sink | Aggregate throughput cost | `0.004us` | `3.556ns` | `0.001093s` | `0.008284s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, null sink | Single-thread estimate | `0.011us` | `10.909ns` | `0.000838s` | `0.007376s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |

## Profile results

| Profile | Measurement | Cost per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.631us` | `0.125264s` (1954.202%) | `0.131675s` | `76807` | incomparable configuration | incomparable |
| Standard timer, wall time disabled | Single-thread estimate | `1.556us` | `0.119493s` (1863.103%) | `0.125907s` | `76807` | incomparable configuration | incomparable |
| Standard timer, null sink | Single-thread estimate | `0.032us` | `0.002426s` (37.694%) | `0.008862s` | `0` | incomparable configuration | incomparable |
| Standard timer, buffered sink | Single-thread estimate | `0.054us` | `0.004122s` (62.393%) | `0.010732s` | `76807` | incomparable configuration | incomparable |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.264us` | `0.081177s` (1132.026%) | `0.088350s` | `307207` | incomparable configuration | incomparable |
| Standard timer, async sink | Aggregate throughput cost | `0.015us` | `0.004625s` (63.842%) | `0.011864s` | `307207` | incomparable configuration | incomparable |
| Hot-path timer, async sink | Aggregate throughput cost | `0.004us` | `0.001093s` (15.191%) | `0.008284s` | `307207` | incomparable configuration | incomparable |
| Hot-path timer, null sink | Single-thread estimate | `0.011us` | `0.000838s` (12.819%) | `0.007376s` | `0` | incomparable configuration | incomparable |

Full historical results remain in
`benchmarks/demo_benchmark_history.json`.

## Profile reference

The commands below rerun one profile at a time through
`scripts/benchmark_demo.py`. The harness handles the alternating
`SCOPE_TIMER=0` and `SCOPE_TIMER=1` passes for you; the `--env` flags
shown here are only the profile-specific knobs.

These examples use `binary=./build-review/benchmark-build/Benchmark`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, and a default `sink_bytes=4096` for profiles that do not override it below.

### Standard timer, default sink

Baseline ScopeTimer cost with the normal synchronous sink and wall-clock timestamps enabled.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8
```

### Standard timer, wall time disabled

Standard timer with `SCOPE_TIMER_WALLTIME=0` so the report shows the cost of dropping `start=` and `end=` timestamp formatting.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, null sink

Standard timer routed to a no-op sink so the benchmark measures ScopeTimer framework overhead without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink

Single-thread run with the thread-buffered sink enabled to show how much caller-thread write overhead falls when flushes are batched.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink (threaded stress)

Multi-threaded buffered run that stresses contention and cross-thread flush behavior under the standard timer format.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_BENCH_SINK_BYTES=4096 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, async sink

Multi-threaded run with the async sink so flush work moves to the background writer instead of the calling thread, using a 64 KiB handoff size to reduce enqueue frequency.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, async sink

Low-overhead output-producing profile: hot-path timer format plus the async sink, measured under the threaded stress workload with a 64 KiB async handoff size.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, null sink

Hot-path timer routed to the no-op sink so the benchmark shows the floor for ScopeTimer's own bookkeeping without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```
