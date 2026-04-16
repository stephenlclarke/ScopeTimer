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

To reproduce the benchmark-app overhead run from CMake, execute:

```bash
cmake -S . -B build-review
cmake --build build-review --target demo_benchmark
cmake --build build-review --target demo_benchmark_matrix
```

These targets configure a dedicated `build-bench` tree with coverage
disabled and `-O2` enabled, then build and benchmark the `Benchmark`
executable with `SCOPE_TIMER=0` and `SCOPE_TIMER=1` against the
CPU-bound `hotpath-bench` scenario.

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
-- Configuring done (1.4s)
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
1/8 Test #1: run_demo .........................   Passed    0.34 sec
    Start 2: run_demo_iterations
2/8 Test #2: run_demo_iterations ..............   Passed    0.05 sec
    Start 3: run_demo_help
3/8 Test #3: run_demo_help ....................   Passed    0.01 sec
    Start 4: run_benchmark_default
4/8 Test #4: run_benchmark_default ............   Passed    0.24 sec
    Start 5: run_benchmark_help
5/8 Test #5: run_benchmark_help ...............   Passed    0.01 sec
    Start 6: run_benchmark_buffered_hotpath
6/8 Test #6: run_benchmark_buffered_hotpath ...   Passed    0.02 sec
    Start 7: run_benchmark_async
7/8 Test #7: run_benchmark_async ..............   Passed    0.01 sec
    Start 8: run_scopetimer_tests
8/8 Test #8: run_scopetimer_tests .............   Passed    0.69 sec

100% tests passed, 0 tests failed out of 8

Total Test time (real) =   1.39 sec
[100%] Run sonar build-wrapper, generate coverage, then run sonar-scanner
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer/build-docs/bw-output
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
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
    Start 1: run_demo
1/8 Test #1: run_demo .........................   Passed    0.34 sec
    Start 2: run_demo_iterations
2/8 Test #2: run_demo_iterations ..............   Passed    0.05 sec
    Start 3: run_demo_help
3/8 Test #3: run_demo_help ....................   Passed    0.01 sec
    Start 4: run_benchmark_default
4/8 Test #4: run_benchmark_default ............   Passed    0.31 sec
    Start 5: run_benchmark_help
5/8 Test #5: run_benchmark_help ...............   Passed    0.01 sec
    Start 6: run_benchmark_buffered_hotpath
6/8 Test #6: run_benchmark_buffered_hotpath ...   Passed    0.02 sec
    Start 7: run_benchmark_async
7/8 Test #7: run_benchmark_async ..............   Passed    0.01 sec
    Start 8: run_scopetimer_tests
8/8 Test #8: run_scopetimer_tests .............   Passed    0.65 sec

100% tests passed, 0 tests failed out of 8

Total Test time (real) =   1.40 sec
(INFO) Reading coverage data...
(WARNING) /Users/sclarke/github/ScopeTimer/include/ScopeTimer.hpp:220: Ignoring negative hits in:                 } else if (!v.empty()) {.
(WARNING) Ignored 6 negative hits overall.
(WARNING) /Users/sclarke/github/ScopeTimer/include/ScopeTimer.hpp:729: Ignoring negative hits in:             if (auto* next = appendUnsignedToBuffer(cur, end, whole)) {.
(WARNING) Ignored 9 negative hits overall.
(INFO) Writing coverage report...
lines: 95.8% (1973 out of 2060)
functions: 98.9% (273 out of 276)
branches: 55.6% (1163 out of 2093)
[100%] Built target coverage
11:44:49.729 INFO  Scanner configuration file: /opt/homebrew/Cellar/sonar-scanner/8.0.1.6346/libexec/conf/sonar-scanner.properties
11:44:49.732 INFO  Project root configuration file: /Users/sclarke/github/ScopeTimer/sonar-project.properties
11:44:49.739 INFO  SonarScanner CLI 8.0.1.6346
11:44:49.740 INFO  Mac OS X 26.4.1 aarch64
11:44:50.832 INFO  Communicating with SonarQube Cloud
11:44:50.832 INFO  JRE provisioning: os[macos], arch[arm64]
11:44:51.426 INFO  Starting SonarScanner Engine...
11:44:51.426 INFO  Java 21.0.9 Eclipse Adoptium (64-bit)
11:44:52.681 INFO  Load global settings
11:44:52.994 INFO  Load global settings (done) | time=311ms
11:44:53.037 INFO  Server id: 1BD809FA-AWHW8ct9-T_TB3XqouNu
11:44:53.114 INFO  Loading required plugins
11:44:53.114 INFO  Load plugins index
11:44:53.153 INFO  Load plugins index (done) | time=38ms
11:44:53.153 INFO  Load/download plugins
11:44:53.182 INFO  Load/download plugins (done) | time=28ms
11:44:53.265 INFO  Loaded core extensions: a3s, sca, architecture
11:44:53.381 INFO  Process project properties
11:44:53.790 INFO  Project key: stephenlclarke_ScopeTimer
11:44:53.791 INFO  Base dir: /Users/sclarke/github/ScopeTimer
11:44:53.791 INFO  Working dir: /Users/sclarke/github/ScopeTimer/.scannerwork
> tail -n 60 ./build-docs.log
  Live variable analysis: Begin: 2026-04-16T10:45:05.816748Z, End: 2026-04-16T10:45:05.820332Z, Duration: 00:00:00.003
  Taint analysis for python: Begin: 2026-04-16T10:45:05.820406Z, End: 2026-04-16T10:45:05.882907Z, Duration: 00:00:00.062
  Report issues: Begin: 2026-04-16T10:45:05.882934Z, End: 2026-04-16T10:45:05.884107Z, Duration: 00:00:00.001
  Store cache: Begin: 2026-04-16T10:45:05.884474Z, End: 2026-04-16T10:45:05.888383Z, Duration: 00:00:00.003
11:45:05.889 INFO  python security sensor peak memory: 655 MB
11:45:05.889 INFO  Sensor PythonSecuritySensor [security] (done) | time=742ms
11:45:05.892 INFO  ------------- Run sensors on project
11:45:05.956 INFO  Sensor PythonArchitectureSensor [architecture]
11:45:05.989 INFO  Found 1 potential Udg file location(s) for "py" in "/Users/sclarke/github/ScopeTimer/.scannerwork"
11:45:05.989 INFO  - /Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py
11:45:05.996 INFO  Architecture analysis is enabled with the following features: legacy, discovery, flaws, smells, deviations, directives
11:45:05.998 INFO  * Protobuf reading starting | memory total=820 | free=150 | used=669 (MB)
11:45:05.998 INFO  * Reading SonarArchitecture UDG data from directory "/Users/sclarke/github/ScopeTimer/.scannerwork/architecture/py"
11:45:06.065 INFO  * Files successfully loaded: "3" out of "3"
11:45:06.065 INFO  * Purging externals (components not scanned) from graphs
11:45:06.065 INFO  * Purging excluded nodes from graphs
11:45:06.066 INFO  * Protobuf reading complete | memory total=820 | free=138 | used=681 (MB)
11:45:06.075 INFO  * Build architecture.graph.py.file_graph.default_perspective hierarchy graph complete (filtered=false) | memory total=820 | free=124 | used=695 (MB)
11:45:06.079 INFO  No directives configured
11:45:06.080 INFO  * No intended architecture defined or detected, using an empty model
11:45:06.133 INFO  Sensor PythonArchitectureSensor [architecture] (done) | time=176ms
11:45:06.133 INFO  Sensor Zero Coverage Sensor
11:45:06.133 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
11:45:06.133 INFO  Sensor Architecture Telemetry [architecture]
11:45:06.134 INFO  Sensor Architecture Telemetry [architecture] (done) | time=0ms
11:45:06.201 INFO  ------------- Gather SCA dependencies on project
11:45:06.501 INFO  Dependency analysis skipped
11:45:06.506 INFO  CPD Executor Calculating CPD for 7 files
11:45:06.513 INFO  CPD Executor CPD calculation finished (done) | time=7ms
11:45:06.514 INFO  SCM Publisher SCM provider for this project is: git
11:45:06.514 INFO  SCM Publisher 14 source files to be analyzed
11:45:06.682 INFO  SCM Publisher 3/14 source files have been analyzed (done) | time=168ms
11:45:06.683 WARN  Missing blame information for the following files:
11:45:06.683 WARN    * scripts/summarize_scope_times.sh
11:45:06.683 WARN    * scripts/record_demo_benchmarks.py
11:45:06.683 WARN    * benchmarks/demo_benchmark_history.json
11:45:06.683 WARN    * test/ScopeTimerTest.cpp
11:45:06.683 WARN    * example/Benchmark.cpp
11:45:06.683 WARN    * include/ScopeTimer.hpp
11:45:06.683 WARN    * scripts/benchmark_demo.py
11:45:06.683 WARN    * scripts/refresh_docs.py
11:45:06.683 WARN    * example/TelemetryWorkload.hpp
11:45:06.683 WARN    * example/Demo.cpp
11:45:06.683 WARN    * .github/workflows/build.yml
11:45:06.683 WARN  This may lead to missing/broken features in SonarCloud
11:45:06.687 INFO  SCM revision ID '694448990b8b9dedb21d056bc7b4db7f75435259'
11:45:06.765 INFO  Analysis report generated in 80ms, dir size=710 KB
11:45:06.794 INFO  Analysis report compressed in 28ms, zip size=175 KB
11:45:07.042 INFO  Analysis report uploaded in 249ms
11:45:07.043 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
11:45:07.043 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
11:45:07.043 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZ2V5QK-QQv6z29B2KgA
11:45:07.043 INFO  ------------- Upload SCA dependency files
11:45:09.094 INFO  Successfully sent architecture data
11:45:09.529 INFO  Sensor cache published successfully
11:45:09.558 INFO  Analysis total time: 16.294 s
11:45:09.559 INFO  SonarScanner Engine completed successfully
11:45:09.966 INFO  EXECUTION SUCCESS
11:45:09.967 INFO  Total time: 20.239s
Built target sonar_scan
```

<!-- markdownlint-enable MD013 -->
