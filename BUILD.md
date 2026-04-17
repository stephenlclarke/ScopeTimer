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
-- Installed build-wrapper locally: /Users/sclarke/github/ScopeTimer-develop-ci/build-docs/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Using Sonar build-wrapper: /Users/sclarke/github/ScopeTimer-develop-ci/build-docs/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Configuring done (1.5s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer-develop-ci/build-docs
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Test project /Users/sclarke/github/ScopeTimer-develop-ci/build-docs
      Start  1: run_demo
 1/20 Test  #1: run_demo ............................   Passed    0.32 sec
      Start  2: run_demo_iterations
 2/20 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/20 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/20 Test  #4: run_demo_positional_iterations ......   Passed    0.10 sec
      Start  5: run_demo_positional_zero
 5/20 Test  #5: run_demo_positional_zero ............   Passed    0.05 sec
      Start  6: run_demo_help
 6/20 Test  #6: run_demo_help .......................   Passed    0.01 sec
      Start  7: run_benchmark_default
 7/20 Test  #7: run_benchmark_default ...............   Passed    0.26 sec
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
20/20 Test #20: run_scopetimer_tests ................   Passed    0.68 sec

100% tests passed, 0 tests failed out of 20

Total Test time (real) =   1.71 sec
[100%] Run sonar build-wrapper, generate coverage, then run sonar-scanner
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer-develop-ci/build-docs/bw-output
[ 33%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 33%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Demo
[ 83%] Built target Benchmark
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Built target sonar_bw
[ 28%] Built target scopetimer_tests
[ 57%] Built target Demo
[ 85%] Built target Benchmark
[100%] Run tests once and generate gcovr reports (SonarQube XML + HTML)
-- Removing stale .gcda files to avoid stamp mismatch
Test project /Users/sclarke/github/ScopeTimer-develop-ci/build-docs
      Start  1: run_demo
 1/20 Test  #1: run_demo ............................   Passed    0.29 sec
      Start  2: run_demo_iterations
 2/20 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/20 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/20 Test  #4: run_demo_positional_iterations ......   Passed    0.10 sec
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
      Create runtime type propagation graph: Begin: 2026-04-17T19:54:58.308473Z, End: 2026-04-17T19:54:58.325215Z, Duration: 00:00:00.016
      Run SCC (Tarjan) on 15374 nodes: Begin: 2026-04-17T19:54:58.325338Z, End: 2026-04-17T19:54:58.332815Z, Duration: 00:00:00.007
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T19:54:58.332849Z, End: 2026-04-17T19:54:58.353734Z, Duration: 00:00:00.020
    Variable Type Analysis #2: Begin: 2026-04-17T19:54:58.355757Z, End: 2026-04-17T19:54:58.382281Z, Duration: 00:00:00.026
      Create runtime type propagation graph: Begin: 2026-04-17T19:54:58.355760Z, End: 2026-04-17T19:54:58.364613Z, Duration: 00:00:00.008
      Run SCC (Tarjan) on 15201 nodes: Begin: 2026-04-17T19:54:58.364655Z, End: 2026-04-17T19:54:58.370394Z, Duration: 00:00:00.005
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T19:54:58.370413Z, End: 2026-04-17T19:54:58.382250Z, Duration: 00:00:00.011
  Load config: Begin: 2026-04-17T19:54:58.383879Z, End: 2026-04-17T19:54:58.455971Z, Duration: 00:00:00.072
  Compute entry points: Begin: 2026-04-17T19:54:58.455989Z, End: 2026-04-17T19:54:58.468218Z, Duration: 00:00:00.012
  Slice call graph: Begin: 2026-04-17T19:54:58.468276Z, End: 2026-04-17T19:54:58.468545Z, Duration: 00:00:00.000
  Live variable analysis: Begin: 2026-04-17T19:54:58.468553Z, End: 2026-04-17T19:54:58.471468Z, Duration: 00:00:00.002
  Taint analysis for python: Begin: 2026-04-17T19:54:58.471513Z, End: 2026-04-17T19:54:58.524692Z, Duration: 00:00:00.053
  Report issues: Begin: 2026-04-17T19:54:58.524715Z, End: 2026-04-17T19:54:58.525744Z, Duration: 00:00:00.001
  Store cache: Begin: 2026-04-17T19:54:58.526030Z, End: 2026-04-17T19:54:58.528677Z, Duration: 00:00:00.002
20:54:58.529 INFO  python security sensor peak memory: 769 MB
20:54:58.529 INFO  Sensor PythonSecuritySensor [security] (done) | time=1002ms
20:54:58.531 INFO  ------------- Run sensors on project
20:54:58.584 INFO  Sensor PythonArchitectureSensor [architecture]
20:54:58.614 INFO  Found 1 potential Udg file location(s) for "py" in "/Users/sclarke/github/ScopeTimer-develop-ci/.scannerwork"
20:54:58.614 INFO  - /Users/sclarke/github/ScopeTimer-develop-ci/.scannerwork/architecture/py
20:54:58.620 INFO  Architecture analysis is enabled with the following features: legacy, discovery, flaws, smells, deviations, directives
20:54:58.621 INFO  * Protobuf reading starting | memory total=888 | free=292 | used=595 (MB)
20:54:58.622 INFO  * Reading SonarArchitecture UDG data from directory "/Users/sclarke/github/ScopeTimer-develop-ci/.scannerwork/architecture/py"
20:54:58.675 INFO  * Files successfully loaded: "4" out of "4"
20:54:58.675 INFO  * Purging externals (components not scanned) from graphs
20:54:58.675 INFO  * Purging excluded nodes from graphs
20:54:58.676 INFO  * Protobuf reading complete | memory total=888 | free=280 | used=607 (MB)
20:54:58.683 INFO  * Build architecture.graph.py.file_graph.default_perspective hierarchy graph complete (filtered=false) | memory total=888 | free=270 | used=617 (MB)
20:54:58.687 INFO  No directives configured
20:54:58.688 INFO  * No intended architecture defined or detected, using an empty model
20:54:58.726 INFO  Sensor PythonArchitectureSensor [architecture] (done) | time=141ms
20:54:58.726 INFO  Sensor Zero Coverage Sensor
20:54:58.726 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
20:54:58.726 INFO  Sensor Architecture Telemetry [architecture]
20:54:58.726 INFO  Sensor Architecture Telemetry [architecture] (done) | time=0ms
20:54:58.781 INFO  ------------- Gather SCA dependencies on project
20:54:59.056 INFO  Dependency analysis skipped
20:54:59.061 INFO  CPD Executor Calculating CPD for 8 files
20:54:59.069 INFO  CPD Executor CPD calculation finished (done) | time=7ms
20:54:59.069 INFO  SCM Publisher SCM provider for this project is: git
20:54:59.070 INFO  SCM Publisher 15 source files to be analyzed
20:54:59.414 INFO  SCM Publisher 14/15 source files have been analyzed (done) | time=343ms
20:54:59.414 WARN  Missing blame information for the following files:
20:54:59.414 WARN    * scripts/refresh_docs.py
20:54:59.414 WARN  This may lead to missing/broken features in SonarCloud
20:54:59.422 INFO  SCM revision ID 'c282c5675da2b9e711d4101fb00a4227fa65513d'
20:54:59.493 INFO  Analysis report generated in 76ms, dir size=902 KB
20:54:59.521 INFO  Analysis report compressed in 29ms, zip size=213 KB
20:54:59.714 INFO  Analysis report uploaded in 192ms
20:54:59.714 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
20:54:59.714 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
20:54:59.714 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZ2dAtk6_RbgRMgFnwD8
20:54:59.715 INFO  ------------- Upload SCA dependency files
20:55:00.823 INFO  Successfully sent architecture data
20:55:01.341 INFO  Sensor cache published successfully
20:55:01.393 INFO  Analysis total time: 17.531 s
20:55:01.394 INFO  SonarScanner Engine completed successfully
20:55:01.821 INFO  EXECUTION SUCCESS
20:55:01.822 INFO  Total time: 21.330s
Built target sonar_scan
```

<!-- markdownlint-enable MD013 -->
