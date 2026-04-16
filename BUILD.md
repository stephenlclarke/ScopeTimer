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

Pull request CI runs `demo_benchmark_matrix` automatically for PR
open and update events, then uploads the refreshed benchmark report
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
-- Configuring done (1.1s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer/build-docs
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Test project /Users/sclarke/github/ScopeTimer/build-docs
    Start 1: run_demo
1/8 Test #1: run_demo .........................   Passed    0.26 sec
    Start 2: run_demo_iterations
2/8 Test #2: run_demo_iterations ..............   Passed    0.05 sec
    Start 3: run_demo_help
3/8 Test #3: run_demo_help ....................   Passed    0.01 sec
    Start 4: run_benchmark_default
4/8 Test #4: run_benchmark_default ............   Passed    0.24 sec
    Start 5: run_benchmark_help
5/8 Test #5: run_benchmark_help ...............   Passed    0.01 sec
    Start 6: run_benchmark_buffered_hotpath
6/8 Test #6: run_benchmark_buffered_hotpath ...   Passed    0.01 sec
    Start 7: run_benchmark_async
7/8 Test #7: run_benchmark_async ..............   Passed    0.01 sec
    Start 8: run_scopetimer_tests
8/8 Test #8: run_scopetimer_tests .............   Passed    0.62 sec

100% tests passed, 0 tests failed out of 8

Total Test time (real) =   1.23 sec
[100%] Run sonar build-wrapper, generate coverage, then run sonar-scanner
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer/build-docs/bw-output
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Built target sonar_bw
[ 85%] Built target Benchmark
[ 85%] Built target Demo
[ 85%] Built target scopetimer_tests
[100%] Run tests once and generate gcovr reports (SonarQube XML + HTML)
-- Removing stale .gcda files to avoid stamp mismatch
Test project /Users/sclarke/github/ScopeTimer/build-docs
    Start 1: run_demo
1/8 Test #1: run_demo .........................   Passed    0.35 sec
    Start 2: run_demo_iterations
2/8 Test #2: run_demo_iterations ..............   Passed    0.05 sec
    Start 3: run_demo_help
3/8 Test #3: run_demo_help ....................   Passed    0.01 sec
    Start 4: run_benchmark_default
4/8 Test #4: run_benchmark_default ............   Passed    0.26 sec
    Start 5: run_benchmark_help
5/8 Test #5: run_benchmark_help ...............   Passed    0.02 sec
    Start 6: run_benchmark_buffered_hotpath
6/8 Test #6: run_benchmark_buffered_hotpath ...   Passed    0.02 sec
    Start 7: run_benchmark_async
7/8 Test #7: run_benchmark_async ..............   Passed    0.02 sec
    Start 8: run_scopetimer_tests
8/8 Test #8: run_scopetimer_tests .............   Passed    0.69 sec

100% tests passed, 0 tests failed out of 8

Total Test time (real) =   1.40 sec
(INFO) Reading coverage data...
(WARNING) /Users/sclarke/github/ScopeTimer/include/ScopeTimer.hpp:390: Ignoring negative hits in:         }.
(WARNING) Ignored 5 negative hits overall.
(WARNING) /Users/sclarke/github/ScopeTimer/include/ScopeTimer.hpp:573: Ignoring negative hits in:             if (width > tmp.size() || static_cast<std::size_t>(end - out) < width) {.
(WARNING) Ignored 11 negative hits overall.
(INFO) Writing coverage report...
lines: 95.8% (1973 out of 2060)
functions: 98.9% (273 out of 276)
branches: 55.6% (1163 out of 2093)
[100%] Built target coverage
11:50:04.377 INFO  Scanner configuration file: /opt/homebrew/Cellar/sonar-scanner/8.0.1.6346/libexec/conf/sonar-scanner.properties
11:50:04.379 INFO  Project root configuration file: /Users/sclarke/github/ScopeTimer/sonar-project.properties
11:50:04.385 INFO  SonarScanner CLI 8.0.1.6346
11:50:04.386 INFO  Mac OS X 26.4.1 aarch64
11:50:05.400 INFO  Communicating with SonarQube Cloud
11:50:05.400 INFO  JRE provisioning: os[macos], arch[arm64]
11:50:05.842 INFO  Starting SonarScanner Engine...
11:50:05.842 INFO  Java 21.0.9 Eclipse Adoptium (64-bit)
11:50:07.046 INFO  Load global settings
11:50:07.435 INFO  Load global settings (done) | time=388ms
11:50:07.473 INFO  Server id: 1BD809FA-AWHW8ct9-T_TB3XqouNu
11:50:07.559 INFO  Loading required plugins
11:50:07.559 INFO  Load plugins index
11:50:07.629 INFO  Load plugins index (done) | time=69ms
11:50:07.629 INFO  Load/download plugins
11:50:07.657 INFO  Load/download plugins (done) | time=28ms
11:50:07.722 INFO  Loaded core extensions: a3s, sca, architecture
11:50:07.836 INFO  Process project properties
11:50:08.163 INFO  Project key: stephenlclarke_ScopeTimer
11:50:08.163 INFO  Base dir: /Users/sclarke/github/ScopeTimer
11:50:08.163 INFO  Working dir: /Users/sclarke/github/ScopeTimer/.scannerwork
> tail -n 60 ./build-docs.log
      Run SCC (Tarjan) on 14796 nodes: Begin: 2026-04-16T10:50:20.077850Z, End: 2026-04-16T10:50:20.090232Z, Duration: 00:00:00.012
      Propagate runtime types to strongly connected components: Begin: 2026-04-16T10:50:20.090325Z, End: 2026-04-16T10:50:20.108701Z, Duration: 00:00:00.018
    Variable Type Analysis #2: Begin: 2026-04-16T10:50:20.110833Z, End: 2026-04-16T10:50:20.150174Z, Duration: 00:00:00.039
      Create runtime type propagation graph: Begin: 2026-04-16T10:50:20.110836Z, End: 2026-04-16T10:50:20.122855Z, Duration: 00:00:00.012
      Run SCC (Tarjan) on 14625 nodes: Begin: 2026-04-16T10:50:20.122924Z, End: 2026-04-16T10:50:20.129816Z, Duration: 00:00:00.006
      Propagate runtime types to strongly connected components: Begin: 2026-04-16T10:50:20.129877Z, End: 2026-04-16T10:50:20.150074Z, Duration: 00:00:00.020
  Load config: Begin: 2026-04-16T10:50:20.152862Z, End: 2026-04-16T10:50:20.247195Z, Duration: 00:00:00.094
  Compute entry points: Begin: 2026-04-16T10:50:20.247252Z, End: 2026-04-16T10:50:20.262816Z, Duration: 00:00:00.015
  Slice call graph: Begin: 2026-04-16T10:50:20.262906Z, End: 2026-04-16T10:50:20.263209Z, Duration: 00:00:00.000
  Live variable analysis: Begin: 2026-04-16T10:50:20.263221Z, End: 2026-04-16T10:50:20.266575Z, Duration: 00:00:00.003
  Taint analysis for python: Begin: 2026-04-16T10:50:20.266672Z, End: 2026-04-16T10:50:20.335666Z, Duration: 00:00:00.068
  Report issues: Begin: 2026-04-16T10:50:20.335725Z, End: 2026-04-16T10:50:20.336971Z, Duration: 00:00:00.001
  Store cache: Begin: 2026-04-16T10:50:20.337318Z, End: 2026-04-16T10:50:20.340593Z, Duration: 00:00:00.003
11:50:20.341 INFO  python security sensor peak memory: 675 MB
11:50:20.342 INFO  Sensor PythonSecuritySensor [security] (done) | time=654ms
11:50:20.345 INFO  ------------- Run sensors on project
11:50:20.400 INFO  Sensor PythonArchitectureSensor [architecture]
11:50:20.430 INFO  Found 1 potential Udg file location(s) for "py" in "/Users/sclarke/github/ScopeTimer/.scannerwork"
11:50:20.430 INFO  - /Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py
11:50:20.437 INFO  Architecture analysis is enabled with the following features: legacy, discovery, flaws, smells, deviations, directives
11:50:20.439 INFO  * Protobuf reading starting | memory total=936 | free=350 | used=585 (MB)
11:50:20.439 INFO  * Reading SonarArchitecture UDG data from directory "/Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py"
11:50:20.502 INFO  * Files successfully loaded: "3" out of "3"
11:50:20.502 INFO  * Purging externals (components not scanned) from graphs
11:50:20.503 INFO  * Purging excluded nodes from graphs
11:50:20.503 INFO  * Protobuf reading complete | memory total=936 | free=338 | used=597 (MB)
11:50:20.514 INFO  * Build architecture.graph.py.file_graph.default_perspective hierarchy graph complete (filtered=false) | memory total=936 | free=325 | used=610 (MB)
11:50:20.518 INFO  No directives configured
11:50:20.520 INFO  * No intended architecture defined or detected, using an empty model
11:50:20.574 INFO  Sensor PythonArchitectureSensor [architecture] (done) | time=174ms
11:50:20.574 INFO  Sensor Zero Coverage Sensor
11:50:20.574 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
11:50:20.574 INFO  Sensor Architecture Telemetry [architecture]
11:50:20.575 INFO  Sensor Architecture Telemetry [architecture] (done) | time=0ms
11:50:20.647 INFO  ------------- Gather SCA dependencies on project
11:50:20.964 INFO  Dependency analysis skipped
11:50:20.970 INFO  CPD Executor Calculating CPD for 7 files
11:50:20.977 INFO  CPD Executor CPD calculation finished (done) | time=7ms
11:50:20.977 INFO  SCM Publisher SCM provider for this project is: git
11:50:20.977 INFO  SCM Publisher 14 source files to be analyzed
11:50:21.162 INFO  SCM Publisher 12/14 source files have been analyzed (done) | time=185ms
11:50:21.162 WARN  Missing blame information for the following files:
11:50:21.162 WARN    * scripts/refresh_docs.py
11:50:21.162 WARN    * .github/workflows/build.yml
11:50:21.163 WARN  This may lead to missing/broken features in SonarCloud
11:50:21.167 INFO  SCM revision ID 'ad4628d40a4841403227506e34bc9c126df9c8c4'
11:50:21.234 INFO  Analysis report generated in 70ms, dir size=733 KB
11:50:21.261 INFO  Analysis report compressed in 27ms, zip size=181 KB
11:50:21.514 INFO  Analysis report uploaded in 253ms
11:50:21.514 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
11:50:21.515 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
11:50:21.515 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZ2V6c9AXEfcJLsvZWoV
11:50:21.515 INFO  ------------- Upload SCA dependency files
11:50:22.525 INFO  Successfully sent architecture data
11:50:23.069 INFO  Sensor cache published successfully
11:50:23.096 INFO  Analysis total time: 15.374 s
11:50:23.097 INFO  SonarScanner Engine completed successfully
11:50:23.522 INFO  EXECUTION SUCCESS
11:50:23.523 INFO  Total time: 19.148s
Built target sonar_scan
```

<!-- markdownlint-enable MD013 -->
