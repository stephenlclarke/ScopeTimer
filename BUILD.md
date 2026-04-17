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
disabled and `-O2` enabled, then build and benchmark the `Benchmark`
executable with `SCOPE_TIMER=0` and `SCOPE_TIMER=1` against the
CPU-bound `hotpath-bench` scenario.

CI runs `demo_benchmark_matrix` automatically for pull requests and
for pushes to `main`, then uploads the refreshed benchmark report
as an artifact.

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
-- Configuring done (1.2s)
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
 1/17 Test  #1: run_demo ............................   Passed    0.35 sec
      Start  2: run_demo_iterations
 2/17 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/17 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/17 Test  #4: run_demo_positional_iterations ......   Passed    0.11 sec
      Start  5: run_demo_positional_zero
 5/17 Test  #5: run_demo_positional_zero ............   Passed    0.05 sec
      Start  6: run_demo_help
 6/17 Test  #6: run_demo_help .......................   Passed    0.01 sec
      Start  7: run_benchmark_default
 7/17 Test  #7: run_benchmark_default ...............   Passed    0.27 sec
      Start  8: run_benchmark_iterations_zero
 8/17 Test  #8: run_benchmark_iterations_zero .......   Passed    0.02 sec
      Start  9: run_benchmark_positional_zero
 9/17 Test  #9: run_benchmark_positional_zero .......   Passed    0.02 sec
      Start 10: run_benchmark_help
10/17 Test #10: run_benchmark_help ..................   Passed    0.01 sec
      Start 11: run_benchmark_invalid_scenario
11/17 Test #11: run_benchmark_invalid_scenario ......   Passed    0.01 sec
      Start 12: run_benchmark_buffered_hotpath
12/17 Test #12: run_benchmark_buffered_hotpath ......   Passed    0.02 sec
      Start 13: run_benchmark_buffered_fast_alias
13/17 Test #13: run_benchmark_buffered_fast_alias ...   Passed    0.08 sec
      Start 14: run_benchmark_async
14/17 Test #14: run_benchmark_async .................   Passed    0.01 sec
      Start 15: run_benchmark_async_invalid_env
15/17 Test #15: run_benchmark_async_invalid_env .....   Passed    0.01 sec
      Start 16: run_benchmark_out_of_range_env
16/17 Test #16: run_benchmark_out_of_range_env ......   Passed    0.02 sec
      Start 17: run_scopetimer_tests
17/17 Test #17: run_scopetimer_tests ................   Passed    0.71 sec

100% tests passed, 0 tests failed out of 17

Total Test time (real) =   1.81 sec
[100%] Run sonar build-wrapper, generate coverage, then run sonar-scanner
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer/build-docs/bw-output
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
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
Test project /Users/sclarke/github/ScopeTimer/build-docs
      Start  1: run_demo
 1/17 Test  #1: run_demo ............................   Passed    0.34 sec
      Start  2: run_demo_iterations
 2/17 Test  #2: run_demo_iterations .................   Passed    0.05 sec
      Start  3: run_demo_iterations_zero_flag
 3/17 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.05 sec
      Start  4: run_demo_positional_iterations
 4/17 Test  #4: run_demo_positional_iterations ......   Passed    0.10 sec
      Start  5: run_demo_positional_zero
 5/17 Test  #5: run_demo_positional_zero ............   Passed    0.05 sec
      Start  6: run_demo_help
 6/17 Test  #6: run_demo_help .......................   Passed    0.01 sec
      Start  7: run_benchmark_default
 7/17 Test  #7: run_benchmark_default ...............   Passed    0.26 sec
      Start  8: run_benchmark_iterations_zero
 8/17 Test  #8: run_benchmark_iterations_zero .......   Passed    0.02 sec
      Start  9: run_benchmark_positional_zero
 9/17 Test  #9: run_benchmark_positional_zero .......   Passed    0.02 sec
      Start 10: run_benchmark_help
10/17 Test #10: run_benchmark_help ..................   Passed    0.01 sec
      Start 11: run_benchmark_invalid_scenario
11/17 Test #11: run_benchmark_invalid_scenario ......   Passed    0.01 sec
      Start 12: run_benchmark_buffered_hotpath
12/17 Test #12: run_benchmark_buffered_hotpath ......   Passed    0.01 sec
      Start 13: run_benchmark_buffered_fast_alias
13/17 Test #13: run_benchmark_buffered_fast_alias ...   Passed    0.08 sec
      Start 14: run_benchmark_async
14/17 Test #14: run_benchmark_async .................   Passed    0.01 sec
      Start 15: run_benchmark_async_invalid_env
15/17 Test #15: run_benchmark_async_invalid_env .....   Passed    0.01 sec
      Start 16: run_benchmark_out_of_range_env
16/17 Test #16: run_benchmark_out_of_range_env ......   Passed    0.02 sec
      Start 17: run_scopetimer_tests
> tail -n 60 ./build-docs.log
    Load cache: Begin: 2026-04-17T16:36:15.320717Z, End: 2026-04-17T16:36:15.320724Z, Duration: 00:00:00.000
  Create runtime call graph: Begin: 2026-04-17T16:36:15.320794Z, End: 2026-04-17T16:36:15.398878Z, Duration: 00:00:00.078
    Variable Type Analysis #1: Begin: 2026-04-17T16:36:15.320959Z, End: 2026-04-17T16:36:15.356326Z, Duration: 00:00:00.035
      Create runtime type propagation graph: Begin: 2026-04-17T16:36:15.321433Z, End: 2026-04-17T16:36:15.335772Z, Duration: 00:00:00.014
      Run SCC (Tarjan) on 14889 nodes: Begin: 2026-04-17T16:36:15.335916Z, End: 2026-04-17T16:36:15.342675Z, Duration: 00:00:00.006
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T16:36:15.342713Z, End: 2026-04-17T16:36:15.356307Z, Duration: 00:00:00.013
    Variable Type Analysis #2: Begin: 2026-04-17T16:36:15.359290Z, End: 2026-04-17T16:36:15.396554Z, Duration: 00:00:00.037
      Create runtime type propagation graph: Begin: 2026-04-17T16:36:15.359293Z, End: 2026-04-17T16:36:15.374657Z, Duration: 00:00:00.015
      Run SCC (Tarjan) on 14713 nodes: Begin: 2026-04-17T16:36:15.374754Z, End: 2026-04-17T16:36:15.381549Z, Duration: 00:00:00.006
      Propagate runtime types to strongly connected components: Begin: 2026-04-17T16:36:15.381583Z, End: 2026-04-17T16:36:15.396501Z, Duration: 00:00:00.014
  Load config: Begin: 2026-04-17T16:36:15.398912Z, End: 2026-04-17T16:36:15.481352Z, Duration: 00:00:00.082
  Compute entry points: Begin: 2026-04-17T16:36:15.481404Z, End: 2026-04-17T16:36:15.494749Z, Duration: 00:00:00.013
  Slice call graph: Begin: 2026-04-17T16:36:15.494832Z, End: 2026-04-17T16:36:15.495108Z, Duration: 00:00:00.000
  Live variable analysis: Begin: 2026-04-17T16:36:15.495119Z, End: 2026-04-17T16:36:15.497577Z, Duration: 00:00:00.002
  Taint analysis for python: Begin: 2026-04-17T16:36:15.497614Z, End: 2026-04-17T16:36:15.544568Z, Duration: 00:00:00.046
  Report issues: Begin: 2026-04-17T16:36:15.544589Z, End: 2026-04-17T16:36:15.545460Z, Duration: 00:00:00.000
  Store cache: Begin: 2026-04-17T16:36:15.545720Z, End: 2026-04-17T16:36:15.548594Z, Duration: 00:00:00.002
17:36:15.549 INFO  python security sensor peak memory: 673 MB
17:36:15.549 INFO  Sensor PythonSecuritySensor [security] (done) | time=525ms
17:36:15.551 INFO  ------------- Run sensors on project
17:36:15.606 INFO  Sensor PythonArchitectureSensor [architecture]
17:36:15.635 INFO  Found 1 potential Udg file location(s) for "py" in "/Users/sclarke/github/ScopeTimer/.scannerwork"
17:36:15.635 INFO  - /Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py
17:36:15.642 INFO  Architecture analysis is enabled with the following features: legacy, discovery, flaws, smells, deviations, directives
17:36:15.643 INFO  * Protobuf reading starting | memory total=924 | free=356 | used=567 (MB)
17:36:15.643 INFO  * Reading SonarArchitecture UDG data from directory "/Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py"
17:36:15.698 INFO  * Files successfully loaded: "3" out of "3"
17:36:15.698 INFO  * Purging externals (components not scanned) from graphs
17:36:15.698 INFO  * Purging excluded nodes from graphs
17:36:15.698 INFO  * Protobuf reading complete | memory total=924 | free=344 | used=579 (MB)
17:36:15.706 INFO  * Build architecture.graph.py.file_graph.default_perspective hierarchy graph complete (filtered=false) | memory total=924 | free=328 | used=595 (MB)
17:36:15.709 INFO  No directives configured
17:36:15.711 INFO  * No intended architecture defined or detected, using an empty model
17:36:15.747 INFO  Sensor PythonArchitectureSensor [architecture] (done) | time=140ms
17:36:15.747 INFO  Sensor Zero Coverage Sensor
17:36:15.747 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
17:36:15.747 INFO  Sensor Architecture Telemetry [architecture]
17:36:15.747 INFO  Sensor Architecture Telemetry [architecture] (done) | time=0ms
17:36:15.803 INFO  ------------- Gather SCA dependencies on project
17:36:16.253 INFO  Dependency analysis skipped
17:36:16.262 INFO  CPD Executor Calculating CPD for 7 files
17:36:16.274 INFO  CPD Executor CPD calculation finished (done) | time=12ms
17:36:16.275 INFO  SCM Publisher SCM provider for this project is: git
17:36:16.276 INFO  SCM Publisher 14 source files to be analyzed
17:36:16.465 INFO  SCM Publisher 14/14 source files have been analyzed (done) | time=188ms
17:36:16.469 INFO  SCM revision ID 'c08eb32a6673f7c8687c73de2788de488ebfcfdd'
17:36:16.540 INFO  Analysis report generated in 72ms, dir size=735 KB
17:36:16.568 INFO  Analysis report compressed in 29ms, zip size=182 KB
17:36:16.817 INFO  Analysis report uploaded in 248ms
17:36:16.818 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
17:36:16.818 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
17:36:16.818 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZ2cTOtD_RbgRMgFnqd5
17:36:16.818 INFO  ------------- Upload SCA dependency files
17:36:17.788 INFO  Successfully sent architecture data
17:36:18.279 INFO  Sensor cache published successfully
17:36:18.322 INFO  Analysis total time: 14.279 s
17:36:18.322 INFO  SonarScanner Engine completed successfully
17:36:18.801 INFO  EXECUTION SUCCESS
17:36:18.802 INFO  Total time: 18.027s
Built target sonar_scan
```

<!-- markdownlint-enable MD013 -->
