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

- Recorded at: `2026-07-11T18:10:56+00:00`
- Commit: `51cd13c`
- Subject: perf(scopetimer): reduce logging overhead and enrich benchmarks
- Branch: `critical-review-fixes`
- Dirty worktree: yes
- Config: `binary=./build-review/benchmark-build/Benchmark`, `build_dir=./build-review/benchmark-build`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison fingerprint: `2004021241e2df8b290560f6a8a7c19ef883530face958b2e7a64d19d83867b4`
- Comparison baseline: last benchmark checked in to `main`: `2026-06-12T16:26:56+00:00` on `cfa0899`
- Delta source: per-record overhead when available, otherwise mean overhead.

## Benchmark host

- System: `macOS-26.5.1-arm64-arm-64bit-Mach-O` (arm64), Python `3.14.6`.
- CPU: `Apple M5 Pro`, physical cores `18`, logical cores `18`.
- Memory: `24.00 GiB` (25769803776 bytes).
- Disk: `1.81 TiB` total, `1.33 TiB` free, `495.65 GiB` used.
- Disk details: filesystem=`/dev/disk3s1s1`, filesystem_type=`apfs`, device_node=`/dev/disk3s1s1`, mount_point=`/`, capacity=`27%`, solid_state=`True`, internal=`True`, smart_status=`Verified`, bus_protocol=`Apple Fabric`.

## Benchmark toolchain

- Compiler: `Apple clang version 21.0.0 (clang-2100.1.1.101)`.
- Compiler path: `/usr/bin/c++`.
- Benchmark binary SHA-256: `9ac0e70e60a7b04b299de5446c2c1574978b9c8e81c2e5017b3cc952cc6c328d`.

## Current speed breakdown

- Fastest measured configuration: Hot-path timer, async sink at `0.010us/record` (`9.971ns/record`).
- Fastest configuration settings: `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0`.

| Configuration | Per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |
| --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.409us` | `1409.016ns` | `0.108222s` | `0.117800s` | default |
| Standard timer, wall time disabled | `1.211us` | `1210.603ns` | `0.092983s` | `0.102419s` | `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, null sink | `0.033us` | `33.297ns` | `0.002557s` | `0.011649s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink | `0.049us` | `49.487ns` | `0.003801s` | `0.012989s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink (threaded stress) | `0.240us` | `240.445ns` | `0.073866s` | `0.083777s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_BENCH_SINK_BYTES=4096`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, async sink | `0.013us` | `13.022ns` | `0.004000s` | `0.013936s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, async sink | `0.010us` | `9.971ns` | `0.003063s` | `0.012955s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, null sink | `0.014us` | `13.930ns` | `0.001070s` | `0.010288s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |

## Profile results

| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.409us` | `0.108222s` (1130.470%) | `0.117800s` | `76807` | incomparable configuration | incomparable |
| Standard timer, wall time disabled | `1.211us` | `0.092983s` (985.519%) | `0.102419s` | `76807` | incomparable configuration | incomparable |
| Standard timer, null sink | `0.033us` | `0.002557s` (28.239%) | `0.011649s` | `0` | incomparable configuration | incomparable |
| Standard timer, buffered sink | `0.049us` | `0.003801s` (41.522%) | `0.012989s` | `76807` | incomparable configuration | incomparable |
| Standard timer, buffered sink (threaded stress) | `0.240us` | `0.073866s` (745.601%) | `0.083777s` | `307207` | incomparable configuration | incomparable |
| Standard timer, async sink | `0.013us` | `0.004000s` (40.349%) | `0.013936s` | `307207` | incomparable configuration | incomparable |
| Hot-path timer, async sink | `0.010us` | `0.003063s` (30.934%) | `0.012955s` | `307207` | incomparable configuration | incomparable |
| Hot-path timer, null sink | `0.014us` | `0.001070s` (11.618%) | `0.010288s` | `0` | incomparable configuration | incomparable |

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
