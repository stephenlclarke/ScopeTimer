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

- Recorded at: `2026-06-12T16:26:56+00:00`
- Commit: `cfa0899`
- Subject: docs(benchmarks): refresh quality snapshot
- Branch: `develop`
- Dirty worktree: yes
- Config: `binary=build-bench/Benchmark`, `build_dir=build-bench`, `scenario=hotpath-bench`, `iterations=5`, `runs=8`, `threads=4`, `sink_bytes=4096`, `cxx_flags=-O3`
- Comparison baseline: last benchmark checked in to `main`: `2026-06-12T15:25:57+00:00` on `c2ef1ca`
- Delta source: per-record overhead when available, otherwise mean overhead.

## Benchmark host

- System: `macOS-26.4.1-arm64-arm-64bit` (arm64), Python `3.12.13`.
- CPU: `Apple M2 Pro`, physical cores `10`, logical cores `10`.
- Memory: `16.00 GiB` (17179869184 bytes).
- Disk: `926.35 GiB` total, `483.55 GiB` free, `442.80 GiB` used.
- Disk details: filesystem=`/dev/disk3s3s1`, filesystem_type=`apfs`, device_node=`/dev/disk3s3s1`, mount_point=`/`, capacity=`48%`, solid_state=`True`, internal=`True`, smart_status=`Verified`, bus_protocol=`Apple Fabric`.

## Current speed breakdown

- Fastest measured configuration: Hot-path timer, async sink at `0.016us/record` (`16.455ns/record`).
- Fastest configuration settings: `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0`.

| Configuration | Per record | Nanoseconds per record | Mean overhead | Enabled mean | Key settings |
| --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.581us` | `1581.027ns` | `0.121434s` | `0.151389s` | default |
| Standard timer, wall time disabled | `1.628us` | `1627.531ns` | `0.125006s` | `0.155477s` | `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, null sink | `n/a` | `n/a` | `0.003515s` | `0.034340s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink | `0.117us` | `117.178ns` | `0.009000s` | `0.038795s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, buffered sink (threaded stress) | `0.376us` | `376.094ns` | `0.115539s` | `0.146733s` | `SCOPE_TIMER_BENCH_SINK=BUFFERED`, `SCOPE_TIMER_BENCH_SINK_BYTES=4096`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Standard timer, async sink | `0.033us` | `33.145ns` | `0.010182s` | `0.040986s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, async sink | `0.016us` | `16.455ns` | `0.005055s` | `0.036285s` | `SCOPE_TIMER_BENCH_SINK=ASYNC`, `SCOPE_TIMER_BENCH_SINK_BYTES=65536`, `SCOPE_TIMER_BENCH_THREADS=4`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |
| Hot-path timer, null sink | `n/a` | `n/a` | `0.002261s` | `0.032231s` | `SCOPE_TIMER_BENCH_SINK=NULL`, `SCOPE_TIMER_BENCH_TIMER=HOTPATH`, `SCOPE_TIMER_WALLTIME=0` |

## Profile results

| Profile | Per record | Mean overhead | Enabled mean | Log lines | Delta vs main baseline | Status |
| --- | --- | --- | --- | --- | --- | --- |
| Standard timer, default sink | `1.581us` | `0.121434s` (405.460%) | `0.151389s` | `76807` | -0.091us (-5.5%) | faster |
| Standard timer, wall time disabled | `1.628us` | `0.125006s` (410.093%) | `0.155477s` | `76807` | +0.139us (+9.3%) | slower |
| Standard timer, null sink | `n/a` | `0.003515s` (11.574%) | `0.034340s` | `0` | +0.001127s (+47.2%) | slower |
| Standard timer, buffered sink | `0.117us` | `0.009000s` (30.283%) | `0.038795s` | `76807` | +0.034us (+40.4%) | slower |
| Standard timer, buffered sink (threaded stress) | `0.376us` | `0.115539s` (370.758%) | `0.146733s` | `307207` | -0.004us (-1.0%) | unchanged |
| Standard timer, async sink | `0.033us` | `0.010182s` (33.077%) | `0.040986s` | `307207` | -0.030us (-47.9%) | faster |
| Hot-path timer, async sink | `0.016us` | `0.005055s` (16.542%) | `0.036285s` | `307207` | -0.033us (-66.5%) | faster |
| Hot-path timer, null sink | `n/a` | `0.002261s` (7.578%) | `0.032231s` | `0` | -0.001136s (-33.4%) | faster |

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
