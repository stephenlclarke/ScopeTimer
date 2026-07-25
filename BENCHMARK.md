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

- Recorded at: `2026-07-25T13:12:36+00:00`
- Commit: `2f6d4a3`
- Subject: test(scopetimer): cover sink failure boundaries (#6)
- Branch: `main`
- Dirty worktree: yes
- Config: `binary=./build-review/benchmark-build/Benchmark`, `build_dir=./build-review/benchmark-build`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison fingerprint: `2004021241e2df8b290560f6a8a7c19ef883530face958b2e7a64d19d83867b4`
- Comparison baseline: last benchmark checked in to `main`: `2026-07-21T06:41:20+00:00` on `7aad12c`
- Delta source: per-record overhead when available, otherwise mean overhead.

## Benchmark host

- System: `macOS-26.5.2-arm64-arm-64bit-Mach-O` (arm64), Python `3.14.6`.
- CPU: `Apple M5 Pro`, physical cores `18`, logical cores `18`.
- Memory: `24.00 GiB` (25769803776 bytes).
- Disk: `1.81 TiB` total, `955.18 GiB` free, `902.96 GiB` used.
- Disk details: filesystem=`/dev/disk3s1s1`, filesystem_type=`apfs`, device_node=`/dev/disk3s1s1`, mount_point=`/`, capacity=`49%`, solid_state=`True`, internal=`True`, smart_status=`Verified`, bus_protocol=`Apple Fabric`.

## Benchmark toolchain

- Compiler: `Apple clang version 21.0.0 (clang-2100.1.1.101)`.
- Compiler path: `/usr/bin/c++`.
- Benchmark binary SHA-256: `b0db51bbec855bbd548b486aaf42d150a860fc501139f504a46d36f2e8f24ad2`.

## Current speed breakdown

- Fastest single-thread measured configuration: Standard timer, buffered sink at `0.112us/record` (`111.500ns/record`).
- Fastest single-thread configuration settings: `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0`.

| Configuration | Measurement | Cost per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.553us` | `1553.430ns` | `0.119314s` | `0.129553s` | default |
| Standard timer, wall time disabled | Single-thread estimate | `1.528us` | `1528.208ns` | `0.117377s` | `0.130221s` | `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, null sink | Single-thread estimate | `0.129us` | `128.746ns` | `0.009889s` | `0.020033s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink | Single-thread estimate | `0.112us` | `111.500ns` | `0.008564s` | `0.019540s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.369us` | `368.787ns` | `0.113294s` | `0.127374s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_BENCH_SINK_BYTES=4096`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, async sink | Aggregate throughput cost | `0.022us` | `21.906ns` | `0.006730s` | `0.020381s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, async sink | Aggregate throughput cost | `0.023us` | `22.829ns` | `0.007013s` | `0.019831s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, null sink | Single-thread estimate | `0.128us` | `128.180ns` | `0.009845s` | `0.020109s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |

## Profile results

| Profile | Measurement | Cost per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | Single-thread estimate | `1.553us` | `0.119314s` (1165.571%) | `0.129553s` | `76807` | +0.008us (+0.5%) | unchanged |
| Standard timer, wall time disabled | Single-thread estimate | `1.528us` | `0.117377s` (994.969%) | `0.130221s` | `76807` | -0.027us (-1.7%) | unchanged |
| Standard timer, null sink | Single-thread estimate | `0.129us` | `0.009889s` (97.600%) | `0.020033s` | `0` | +0.001us (+1.1%) | unchanged |
| Standard timer, buffered sink | Single-thread estimate | `0.112us` | `0.008564s` (83.491%) | `0.019540s` | `76807` | -0.017us (-13.0%) | faster |
| Standard timer, buffered sink (threaded stress) | Aggregate throughput cost | `0.369us` | `0.113294s` (906.499%) | `0.127374s` | `307207` | -0.019us (-4.9%) | faster |
| Standard timer, async sink | Aggregate throughput cost | `0.022us` | `0.006730s` (63.694%) | `0.020381s` | `307207` | -0.007us (-24.6%) | faster |
| Hot-path timer, async sink | Aggregate throughput cost | `0.023us` | `0.007013s` (67.451%) | `0.019831s` | `307207` | -0.009us (-28.7%) | faster |
| Hot-path timer, null sink | Single-thread estimate | `0.128us` | `0.009845s` (95.966%) | `0.020109s` | `0` | -0.003us (-2.4%) | faster |

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
