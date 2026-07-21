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

- Recorded at: `2026-07-21T06:41:20+00:00`
- Commit: `7aad12c`
- Subject: fix(quality): resolve main branch sonar findings
- Branch: `critical-review-fixes`
- Dirty worktree: yes
- Config: `binary=./build-review/benchmark-build/Benchmark`, `build_dir=./build-review/benchmark-build`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison fingerprint: `2004021241e2df8b290560f6a8a7c19ef883530face958b2e7a64d19d83867b4`
- Comparison baseline: last benchmark checked in to `main`: `2026-07-11T18:10:56+00:00` on `51cd13c`
- Delta source: per-record overhead when available, otherwise mean overhead.

## Benchmark host

- System: `macOS-26.5.2-arm64-arm-64bit-Mach-O` (arm64), Python `3.14.6`.
- CPU: `Apple M5 Pro`, physical cores `18`, logical cores `18`.
- Memory: `24.00 GiB` (25769803776 bytes).
- Disk: `1.81 TiB` total, `924.36 GiB` free, `933.78 GiB` used.
- Disk details: filesystem=`/dev/disk3s1s1`, filesystem_type=`apfs`, device_node=`/dev/disk3s1s1`, mount_point=`/`, capacity=`51%`, solid_state=`True`, internal=`True`, smart_status=`Verified`, bus_protocol=`Apple Fabric`.

## Benchmark toolchain

- Compiler: `Apple clang version 21.0.0 (clang-2100.1.1.101)`.
- Compiler path: `/usr/bin/c++`.
- Benchmark binary SHA-256: `6b3070a4bfd233c81f9745675523f8eb090e79518783cb28bb8b2e30150ec232`.

## Current speed breakdown

- Fastest single-thread measured configuration: Standard timer, null sink at `0.127us/record` (`127.298ns/record`).
- Fastest single-thread configuration settings: `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0`.

| Configuration | Measurement | Cost per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.546us` | `1545.624ns` | `0.118715s` | `0.128473s` | default |
| Standard timer, wall time disabled | Single-thread estimate | `1.555us` | `1555.300ns` | `0.119458s` | `0.129312s` | `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, null sink | Single-thread estimate | `0.127us` | `127.298ns` | `0.009777s` | `0.019551s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink | Single-thread estimate | `0.128us` | `128.138ns` | `0.009842s` | `0.019624s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.388us` | `387.768ns` | `0.119125s` | `0.130492s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_BENCH_SINK_BYTES=4096`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, async sink | Aggregate throughput cost | `0.029us` | `29.050ns` | `0.008924s` | `0.019906s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, async sink | Aggregate throughput cost | `0.032us` | `31.998ns` | `0.009830s` | `0.019668s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, null sink | Single-thread estimate | `0.131us` | `131.286ns` | `0.010084s` | `0.019824s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |

## Profile results

| Profile | Measurement | Cost per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.546us` | `0.118715s` (1219.131%) | `0.128473s` | `76807` | +0.010492s (+9.7%) | slower |
| Standard timer, wall time disabled | Single-thread estimate | `1.555us` | `0.119458s` (1213.068%) | `0.129312s` | `76807` | +0.026475s (+28.5%) | slower |
| Standard timer, null sink | Single-thread estimate | `0.127us` | `0.009777s` (100.166%) | `0.019551s` | `0` | +0.007220s (+282.3%) | slower |
| Standard timer, buffered sink | Single-thread estimate | `0.128us` | `0.009842s` (100.729%) | `0.019624s` | `76807` | +0.006041s (+158.9%) | slower |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.388us` | `0.119125s` (1113.451%) | `0.130492s` | `307207` | +0.045259s (+61.3%) | slower |
| Standard timer, async sink | Aggregate throughput cost | `0.029us` | `0.008924s` (87.518%) | `0.019906s` | `307207` | +0.004924s (+123.1%) | slower |
| Hot-path timer, async sink | Aggregate throughput cost | `0.032us` | `0.009830s` (99.977%) | `0.019668s` | `307207` | +0.006767s (+220.9%) | slower |
| Hot-path timer, null sink | Single-thread estimate | `0.131us` | `0.010084s` (103.670%) | `0.019824s` | `0` | +0.009014s (+842.5%) | slower |

Full historical results remain in
`benchmarks/demo_benchmark_history.json`.

## Profile reference

The commands below rerun one profile at a time through
`scripts/benchmark_demo.py`. The harness handles the alternating
`SCOPE_TIMER=0` and `SCOPE_TIMER=1` passes for you; the `--env` flags
shown here are only the profile-specific knobs.

These examples use `binary=./build-review/build-review/benchmark-build/Benchmark`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, and a default `sink_bytes=4096` for profiles that do not override it below.

### Standard timer, default sink

Baseline ScopeTimer cost with the normal synchronous sink and wall-clock timestamps enabled.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8
```

### Standard timer, wall time disabled

Standard timer with `SCOPE_TIMER_WALLTIME=0` so the report shows the cost of dropping `start=` and `end=` timestamp formatting.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, null sink

Standard timer routed to a no-op sink so the benchmark measures ScopeTimer framework overhead without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink

Single-thread run with the thread-buffered sink enabled to show how much caller-thread write overhead falls when flushes are batched.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, buffered sink (threaded stress)

Multi-threaded buffered run that stresses contention and cross-thread flush behavior under the standard timer format.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=BUFFERED --env SCOPE_TIMER_BENCH_SINK_BYTES=4096 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Standard timer, async sink

Multi-threaded run with the async sink so flush work moves to the background writer instead of the calling thread, using a 64 KiB handoff size to reduce enqueue frequency.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, async sink

Low-overhead output-producing profile: hot-path timer format plus the async sink, measured under the threaded stress workload with a 64 KiB async handoff size.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=ASYNC --env SCOPE_TIMER_BENCH_SINK_BYTES=65536 --env SCOPE_TIMER_BENCH_THREADS=4 --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```

### Hot-path timer, null sink

Hot-path timer routed to the no-op sink so the benchmark shows the floor for ScopeTimer's own bookkeeping without output I/O.

```bash
python3 scripts/benchmark_demo.py --binary ./build-review/build-review/benchmark-build/Benchmark --scenario hotpath-bench --iterations 5 --runs 8 --env SCOPE_TIMER_BENCH_SINK=NULL --env SCOPE_TIMER_BENCH_TIMER=HOTPATH --env SCOPE_TIMER_WALLTIME=0
```
