<!-- Generated automatically by scripts/refresh_docs.py -->

# Build

Build, coverage, Sonar, and benchmark-target usage for this repo lives
here so the main README can stay focused on the library API.

See also:

- [README.md](README.md) for library usage
- [TESTS.md](TESTS.md) for log-format examples and summary output
- [BENCHMARK.md](BENCHMARK.md) for the latest benchmark snapshot

`coverage` and `sonar_scan` require `gcovr`. `sonar_scan` also needs
`SONAR_TOKEN` plus access to your SonarCloud or SonarQube server, and
it passes the active CMake build directory through to the scanner so
out-of-tree builds analyze the right artifacts.

`leak_check` runs `scopetimer_tests` under the native leak detector
for the current platform: `leaks` on macOS and `valgrind` on Linux.
That means MacBook developer runs use `leaks`, while Linux hosts and
the GitHub Actions build use Valgrind for leak detection.
Run it with:

```bash
cmake -S . -B build-review
cmake --build build-review --target leak_check
```

`scopetimer_header_coverage` is the header-only coverage gate for
`include/ScopeTimer.hpp`. It rebuilds `test/ScopeTimerTest.cpp` with
clang source-based coverage and enforces the configured line-coverage
threshold (default `80%`). Run it with:

```bash
cmake -S . -B build-review
cmake --build build-review --target scopetimer_header_coverage
```

`Demo` is the educational example app. The dedicated overhead workload
now lives in `example/Benchmark.cpp`.

Benchmarks are intentionally excluded from the default
`cmake --build` path so local builds stay fast. Run them explicitly
with:

```bash
cmake -S . -B build-review
cmake --build build-review --target demo_benchmark
cmake --build build-review --target demo_benchmark_matrix
```

These targets configure a dedicated `build-bench` tree with coverage
disabled and maximum benchmark-only optimization flags enabled
(default `-O3` on GCC/Clang and `/O2` on MSVC) without defining
`NDEBUG`, then build and benchmark the `Benchmark` executable with
`SCOPE_TIMER=0` and `SCOPE_TIMER=1` against the
CPU-bound `hotpath-bench` scenario.

Benchmarks are intentionally local-only for this repo and are not run
in GitHub Actions. Run `demo_benchmark_matrix` on the MacBook before
pushing changes that could affect performance.

The human-readable benchmark results now live in
[`BENCHMARK.md`](BENCHMARK.md). That file is refreshed automatically by
`demo_benchmark_matrix`, and the full history remains in
`benchmarks/demo_benchmark_history.json`.

<!-- markdownlint-disable MD013 -->

```bash
> rm -rf ./build-docs ./build-docs.log
> { cmake -S . -B ./build-docs -DAUTO_REFRESH_DOCS=OFF && \
  cmake --build ./build-docs -j && \
  ctest --test-dir ./build-docs --output-on-failure && \
  cmake --build ./build-docs --target sonar_scan; } > ./build-docs.log 2>&1
> sed -n '1,120p' ./build-docs.log
-- The CXX compiler identification is GNU 15.2.0
-- Checking whether CXX compiler has -isysroot
-- Checking whether CXX compiler has -isysroot - yes
-- Checking whether CXX compiler supports OSX deployment target flag
-- Checking whether CXX compiler supports OSX deployment target flag - yes
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /opt/homebrew/bin/g++-15 - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Using GCOV from environment: /opt/homebrew/bin/gcov-15
-- Downloading Sonar build-wrapper from: https://sonarcloud.io/static/cpp/build-wrapper-macosx-arm64.zip
-- Arm64 build-wrapper download failed; trying x86 fallback: https://sonarcloud.io/static/cpp/build-wrapper-macosx-x86.zip
-- Installed build-wrapper locally: /Users/sclarke/github/ScopeTimer/build-docs/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Using Sonar build-wrapper: /Users/sclarke/github/ScopeTimer/build-docs/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Configuring done (1.1s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer/build-docs
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Test project /Users/sclarke/github/ScopeTimer/build-docs
      Start  1: run_demo
 1/20 Test  #1: run_demo ............................   Passed    0.36 sec
      Start  2: run_demo_iterations
 2/20 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/20 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/20 Test  #4: run_demo_positional_iterations ......   Passed    0.11 sec
      Start  5: run_demo_positional_zero
 5/20 Test  #5: run_demo_positional_zero ............   Passed    0.05 sec
      Start  6: run_demo_help
 6/20 Test  #6: run_demo_help .......................   Passed    0.02 sec
      Start  7: run_benchmark_default
 7/20 Test  #7: run_benchmark_default ...............   Passed    0.29 sec
      Start  8: run_benchmark_iterations_zero
 8/20 Test  #8: run_benchmark_iterations_zero .......   Passed    0.02 sec
      Start  9: run_benchmark_positional_zero
 9/20 Test  #9: run_benchmark_positional_zero .......   Passed    0.02 sec
      Start 10: run_benchmark_help
10/20 Test #10: run_benchmark_help ..................   Passed    0.01 sec
      Start 11: run_benchmark_invalid_scenario
11/20 Test #11: run_benchmark_invalid_scenario ......   Passed    0.01 sec
      Start 12: run_benchmark_buffered_hotpath
12/20 Test #12: run_benchmark_buffered_hotpath ......   Passed    0.02 sec
      Start 13: run_benchmark_buffered_fast_alias
13/20 Test #13: run_benchmark_buffered_fast_alias ...   Passed    0.02 sec
      Start 14: run_benchmark_async
14/20 Test #14: run_benchmark_async .................   Passed    0.01 sec
      Start 15: run_benchmark_null
15/20 Test #15: run_benchmark_null ..................   Passed    0.01 sec
      Start 16: run_benchmark_null_standard_alias
16/20 Test #16: run_benchmark_null_standard_alias ...   Passed    0.01 sec
      Start 17: run_benchmark_noop_alias
17/20 Test #17: run_benchmark_noop_alias ............   Passed    0.01 sec
      Start 18: run_benchmark_async_invalid_env
18/20 Test #18: run_benchmark_async_invalid_env .....   Passed    0.01 sec
      Start 19: run_benchmark_out_of_range_env
19/20 Test #19: run_benchmark_out_of_range_env ......   Passed    0.02 sec
      Start 20: run_scopetimer_tests
20/20 Test #20: run_scopetimer_tests ................   Passed    0.77 sec

100% tests passed, 0 tests failed out of 20

Total Test time (real) =   1.89 sec
[100%] Run sonar build-wrapper, generate coverage, then run sonar-scanner
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer/build-docs/bw-output
[ 33%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 33%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Built target sonar_bw
[ 28%] Built target scopetimer_tests
[ 57%] Built target Demo
[ 85%] Built target Benchmark
[100%] Run tests once and generate gcovr reports (SonarQube XML + HTML)
-- Removing stale .gcda files to avoid stamp mismatch
Test project /Users/sclarke/github/ScopeTimer/build-docs
      Start  1: run_demo
 1/20 Test  #1: run_demo ............................   Passed    0.30 sec
      Start  2: run_demo_iterations
 2/20 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/20 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/20 Test  #4: run_demo_positional_iterations ......   Passed    0.11 sec
      Start  5: run_demo_positional_zero
 5/20 Test  #5: run_demo_positional_zero ............   Passed    0.05 sec
      Start  6: run_demo_help
 6/20 Test  #6: run_demo_help .......................   Passed    0.01 sec
      Start  7: run_benchmark_default
 7/20 Test  #7: run_benchmark_default ...............   Passed    0.27 sec
      Start  8: run_benchmark_iterations_zero
 8/20 Test  #8: run_benchmark_iterations_zero .......   Passed    0.02 sec
      Start  9: run_benchmark_positional_zero
 9/20 Test  #9: run_benchmark_positional_zero .......   Passed    0.02 sec
      Start 10: run_benchmark_help
10/20 Test #10: run_benchmark_help ..................   Passed    0.01 sec
      Start 11: run_benchmark_invalid_scenario
11/20 Test #11: run_benchmark_invalid_scenario ......   Passed    0.01 sec
      Start 12: run_benchmark_buffered_hotpath
12/20 Test #12: run_benchmark_buffered_hotpath ......   Passed    0.01 sec
      Start 13: run_benchmark_buffered_fast_alias
13/20 Test #13: run_benchmark_buffered_fast_alias ...   Passed    0.02 sec
      Start 14: run_benchmark_async
> tail -n 60 ./build-docs.log
      Run SCC (Tarjan) on 15384 nodes: Begin: 2026-04-17T20:09:03.938446Z, End: 2026-04-17T20:09:03.946818Z, Duration: 00:00:00.008
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T20:09:03.946866Z, End: 2026-04-17T20:09:03.966461Z, Duration: 00:00:00.019
    Variable Type Analysis #2: Begin: 2026-04-17T20:09:03.968705Z, End: 2026-04-17T20:09:04.005456Z, Duration: 00:00:00.036
      Create runtime type propagation graph: Begin: 2026-04-17T20:09:03.968708Z, End: 2026-04-17T20:09:03.983944Z, Duration: 00:00:00.015
      Run SCC (Tarjan) on 15210 nodes: Begin: 2026-04-17T20:09:03.983992Z, End: 2026-04-17T20:09:03.990410Z, Duration: 00:00:00.006
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T20:09:03.990445Z, End: 2026-04-17T20:09:04.005404Z, Duration: 00:00:00.014
  Load config: Begin: 2026-04-17T20:09:04.007526Z, End: 2026-04-17T20:09:04.107472Z, Duration: 00:00:00.099
  Compute entry points: Begin: 2026-04-17T20:09:04.107519Z, End: 2026-04-17T20:09:04.122174Z, Duration: 00:00:00.014
  Slice call graph: Begin: 2026-04-17T20:09:04.122271Z, End: 2026-04-17T20:09:04.122595Z, Duration: 00:00:00.000
  Live variable analysis: Begin: 2026-04-17T20:09:04.122609Z, End: 2026-04-17T20:09:04.126224Z, Duration: 00:00:00.003
  Taint analysis for python: Begin: 2026-04-17T20:09:04.126318Z, End: 2026-04-17T20:09:04.195419Z, Duration: 00:00:00.069
  Report issues: Begin: 2026-04-17T20:09:04.195458Z, End: 2026-04-17T20:09:04.196936Z, Duration: 00:00:00.001
  Store cache: Begin: 2026-04-17T20:09:04.197422Z, End: 2026-04-17T20:09:04.200769Z, Duration: 00:00:00.003
21:09:04.202 INFO  python security sensor peak memory: 643 MB
21:09:04.202 INFO  Sensor PythonSecuritySensor [security] (done) | time=606ms
21:09:04.204 INFO  ------------- Run sensors on project
21:09:04.265 INFO  Sensor PythonArchitectureSensor [architecture]
21:09:04.297 INFO  Found 1 potential Udg file location(s) for "py" in "/Users/sclarke/github/ScopeTimer/.scannerwork"
21:09:04.297 INFO  - /Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py
21:09:04.303 INFO  Architecture analysis is enabled with the following features: legacy, discovery, flaws, smells, deviations, directives
21:09:04.304 INFO  * Protobuf reading starting | memory total=720 | free=192 | used=527 (MB)
21:09:04.304 INFO  * Reading SonarArchitecture UDG data from directory "/Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py"
21:09:04.367 INFO  * Files successfully loaded: "4" out of "4"
21:09:04.367 INFO  * Purging externals (components not scanned) from graphs
21:09:04.367 INFO  * Purging excluded nodes from graphs
21:09:04.368 INFO  * Protobuf reading complete | memory total=720 | free=180 | used=539 (MB)
21:09:04.377 INFO  * Build architecture.graph.py.file_graph.default_perspective hierarchy graph complete (filtered=false) | memory total=720 | free=166 | used=553 (MB)
21:09:04.380 INFO  No directives configured
21:09:04.381 INFO  * No intended architecture defined or detected, using an empty model
21:09:04.422 INFO  Sensor PythonArchitectureSensor [architecture] (done) | time=157ms
21:09:04.422 INFO  Sensor Zero Coverage Sensor
21:09:04.422 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
21:09:04.422 INFO  Sensor Architecture Telemetry [architecture]
21:09:04.422 INFO  Sensor Architecture Telemetry [architecture] (done) | time=0ms
21:09:04.498 INFO  ------------- Gather SCA dependencies on project
21:09:04.702 INFO  Dependency analysis skipped
21:09:04.704 INFO  CPD Executor Calculating CPD for 8 files
21:09:04.712 INFO  CPD Executor CPD calculation finished (done) | time=8ms
21:09:04.712 INFO  SCM Publisher SCM provider for this project is: git
21:09:04.713 INFO  SCM Publisher 16 source files to be analyzed
21:09:04.951 INFO  SCM Publisher 14/16 source files have been analyzed (done) | time=238ms
21:09:04.951 WARN  Missing blame information for the following files:
21:09:04.951 WARN    * demo_benchmark_history.json
21:09:04.952 WARN    * scripts/refresh_docs.py
21:09:04.952 WARN  This may lead to missing/broken features in SonarCloud
21:09:04.955 INFO  SCM revision ID '4a640755e9e07d3cbb01fa3f8b889f9c4baf775c'
21:09:05.033 INFO  Analysis report generated in 79ms, dir size=1 MB
21:09:05.064 INFO  Analysis report compressed in 32ms, zip size=248 KB
21:09:05.628 INFO  Analysis report uploaded in 563ms
21:09:05.628 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
21:09:05.628 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
21:09:05.628 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZ2dD8EnBQE_A6JxFvcX
21:09:05.628 INFO  ------------- Upload SCA dependency files
21:09:07.543 INFO  Successfully sent architecture data
21:09:08.276 INFO  Sensor cache published successfully
21:09:08.307 INFO  Analysis total time: 16.455 s
21:09:08.307 INFO  SonarScanner Engine completed successfully
21:09:08.754 INFO  EXECUTION SUCCESS
21:09:08.754 INFO  Total time: 20.290s
Built target sonar_scan
```

<!-- markdownlint-enable MD013 -->
