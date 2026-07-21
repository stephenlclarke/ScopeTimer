<!-- Generated automatically by scripts/refresh_docs.py -->

# Build

Build, coverage, Sonar, and benchmark-target usage for this repo lives
here so the main README can stay focused on the library API.

See also:

- [README.md](README.md) for library usage
- [TESTS.md](TESTS.md) for log-format examples and summary output
- [BENCHMARK.md](BENCHMARK.md) for the latest benchmark snapshot

`coverage` and `sonar_scan` require `gcovr` and a build configured
with `-DENABLE_COVERAGE=ON`. `sonar_scan` also needs
`SONAR_TOKEN`, access to your SonarCloud or SonarQube server, and a
build configured with `-DENABLE_SONAR=ON`. It passes the active CMake
build directory through to the scanner so out-of-tree builds analyze
the right artifacts. To stay within the free-tier branch limit, both
the local target and GitHub Actions restrict Sonar scans to `main`.

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

These targets configure a dedicated `<build-dir>/benchmark-build` tree with coverage
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

The generated transcript below is captured by the managed
`docs_refresh` target. Use that target so the ownership marker is
created before its nested build directory is replaced.

<!-- markdownlint-disable MD013 -->

```bash
> cmake --build ./build-review --target docs_refresh
> # Captured nested configure/build/test output:
-- The CXX compiler identification is AppleClang 21.0.0.21000101
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Configuring done (0.5s)
-- Generating done (0.0s)
-- Build files have been written to: ./build-review/docs-refresh
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_release_compile_test.dir/test/ScopeTimerReleaseCompileTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 62%] Linking CXX executable scopetimer_release_compile_test
[ 62%] Built target scopetimer_release_compile_test
[ 75%] Linking CXX executable Benchmark
[ 87%] Linking CXX executable Demo
[ 87%] Built target Benchmark
[ 87%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Test project ./build-review/docs-refresh
      Start  1: run_demo
 1/29 Test  #1: run_demo ................................   Passed    0.33 sec
      Start  2: run_demo_iterations
 2/29 Test  #2: run_demo_iterations .....................   Passed    0.04 sec
      Start  3: run_demo_positional_iterations
 3/29 Test  #3: run_demo_positional_iterations ..........   Passed    0.10 sec
      Start  4: run_demo_help
 4/29 Test  #4: run_demo_help ...........................   Passed    0.01 sec
      Start  5: run_benchmark_default
 5/29 Test  #5: run_benchmark_default ...................   Passed    0.25 sec
      Start  6: run_benchmark_help
 6/29 Test  #6: run_benchmark_help ......................   Passed    0.00 sec
      Start  7: run_benchmark_instrumentation_status
 7/29 Test  #7: run_benchmark_instrumentation_status ....   Passed    0.00 sec
      Start  8: run_benchmark_buffered_hotpath
 8/29 Test  #8: run_benchmark_buffered_hotpath ..........   Passed    0.01 sec
      Start  9: run_benchmark_buffered_fast_alias
 9/29 Test  #9: run_benchmark_buffered_fast_alias .......   Passed    0.01 sec
      Start 10: run_benchmark_async
10/29 Test #10: run_benchmark_async .....................   Passed    0.01 sec
      Start 11: run_benchmark_null
11/29 Test #11: run_benchmark_null ......................   Passed    0.00 sec
      Start 12: run_benchmark_null_standard_alias
12/29 Test #12: run_benchmark_null_standard_alias .......   Passed    0.00 sec
      Start 13: run_benchmark_noop_alias
13/29 Test #13: run_benchmark_noop_alias ................   Passed    0.01 sec
      Start 14: run_benchmark_max_sink_bytes
14/29 Test #14: run_benchmark_max_sink_bytes ............   Passed    0.00 sec
      Start 15: run_demo_iterations_zero_flag
15/29 Test #15: run_demo_iterations_zero_flag ...........   Passed    0.03 sec
      Start 16: run_demo_positional_zero
16/29 Test #16: run_demo_positional_zero ................   Passed    0.03 sec
      Start 17: run_benchmark_iterations_zero
17/29 Test #17: run_benchmark_iterations_zero ...........   Passed    0.03 sec
      Start 18: run_benchmark_positional_zero
18/29 Test #18: run_benchmark_positional_zero ...........   Passed    0.03 sec
      Start 19: run_benchmark_invalid_scenario
19/29 Test #19: run_benchmark_invalid_scenario ..........   Passed    0.03 sec
      Start 20: run_benchmark_async_invalid_env
20/29 Test #20: run_benchmark_async_invalid_env .........   Passed    0.03 sec
      Start 21: run_benchmark_out_of_range_env
21/29 Test #21: run_benchmark_out_of_range_env ..........   Passed    0.03 sec
      Start 22: run_benchmark_invalid_sink_bytes
22/29 Test #22: run_benchmark_invalid_sink_bytes ........   Passed    0.03 sec
      Start 23: run_benchmark_out_of_range_sink_bytes
23/29 Test #23: run_benchmark_out_of_range_sink_bytes ...   Passed    0.03 sec
      Start 24: run_scopetimer_tests
24/29 Test #24: run_scopetimer_tests ....................   Passed    1.26 sec
      Start 25: run_scopetimer_release_compile_test
25/29 Test #25: run_scopetimer_release_compile_test .....   Passed    0.21 sec
      Start 26: run_benchmark_tool_tests
26/29 Test #26: run_benchmark_tool_tests ................   Passed    0.08 sec
      Start 27: run_refresh_docs_tests
27/29 Test #27: run_refresh_docs_tests ..................   Passed    0.06 sec
      Start 28: run_sonar_branch_guard_tests
28/29 Test #28: run_sonar_branch_guard_tests ............   Passed    0.67 sec
      Start 29: run_cmake_consumer_test
29/29 Test #29: run_cmake_consumer_test .................   Passed    0.93 sec

100% tests passed out of 29

Total Test time (real) =   4.28 sec
```

<!-- markdownlint-enable MD013 -->
