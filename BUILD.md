<!-- Generated automatically by scripts/refresh_docs.py -->

# Build

Build, coverage, Sonar, and benchmark-target usage for this repo lives
here so the main README can stay focused on the library API.

See also:

- [README.md](README.md) for library usage
- [TESTS.md](TESTS.md) for log-format examples and summary output
- [BENCHMARK.md](BENCHMARK.md) for the latest benchmark snapshot

`coverage` and `sonar_scan` require `gcovr`. `sonar_scan` also needs
`SONAR_TOKEN`, access to your SonarCloud or SonarQube server, and a
build configured with `-DENABLE_SONAR=ON`. It passes the active CMake
build directory through to the scanner so out-of-tree builds analyze
the right artifacts.

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
> { cmake -S . -B ./build-docs -DAUTO_REFRESH_DOCS=OFF -DENABLE_SONAR=OFF && \
  cmake --build ./build-docs -j && \
  ctest --test-dir ./build-docs --output-on-failure
} > ./build-docs.log 2>&1
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
-- Configuring done (3.6s)
-- Generating done (0.1s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer/build-docs
[ 16%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 33%] Building CXX object CMakeFiles/Benchmark.dir/example/Benchmark.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 66%] Linking CXX executable Benchmark
[ 83%] Linking CXX executable Demo
[ 83%] Built target Benchmark
[ 83%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
Test project /Users/sclarke/github/ScopeTimer/build-docs
      Start  1: run_demo
 1/20 Test  #1: run_demo ............................   Passed    0.54 sec
      Start  2: run_demo_iterations
 2/20 Test  #2: run_demo_iterations .................   Passed    0.06 sec
      Start  3: run_demo_iterations_zero_flag
 3/20 Test  #3: run_demo_iterations_zero_flag .......   Passed    0.07 sec
      Start  4: run_demo_positional_iterations
 4/20 Test  #4: run_demo_positional_iterations ......   Passed    0.13 sec
      Start  5: run_demo_positional_zero
 5/20 Test  #5: run_demo_positional_zero ............   Passed    0.07 sec
      Start  6: run_demo_help
 6/20 Test  #6: run_demo_help .......................   Passed    0.04 sec
      Start  7: run_benchmark_default
 7/20 Test  #7: run_benchmark_default ...............   Passed    0.33 sec
      Start  8: run_benchmark_iterations_zero
 8/20 Test  #8: run_benchmark_iterations_zero .......   Passed    0.04 sec
      Start  9: run_benchmark_positional_zero
 9/20 Test  #9: run_benchmark_positional_zero .......   Passed    0.04 sec
      Start 10: run_benchmark_help
10/20 Test #10: run_benchmark_help ..................   Passed    0.02 sec
      Start 11: run_benchmark_invalid_scenario
11/20 Test #11: run_benchmark_invalid_scenario ......   Passed    0.02 sec
      Start 12: run_benchmark_buffered_hotpath
12/20 Test #12: run_benchmark_buffered_hotpath ......   Passed    0.03 sec
      Start 13: run_benchmark_buffered_fast_alias
13/20 Test #13: run_benchmark_buffered_fast_alias ...   Passed    0.03 sec
      Start 14: run_benchmark_async
14/20 Test #14: run_benchmark_async .................   Passed    0.03 sec
      Start 15: run_benchmark_null
15/20 Test #15: run_benchmark_null ..................   Passed    0.03 sec
      Start 16: run_benchmark_null_standard_alias
16/20 Test #16: run_benchmark_null_standard_alias ...   Passed    0.03 sec
      Start 17: run_benchmark_noop_alias
17/20 Test #17: run_benchmark_noop_alias ............   Passed    0.03 sec
      Start 18: run_benchmark_async_invalid_env
18/20 Test #18: run_benchmark_async_invalid_env .....   Passed    0.03 sec
      Start 19: run_benchmark_out_of_range_env
19/20 Test #19: run_benchmark_out_of_range_env ......   Passed    0.03 sec
      Start 20: run_scopetimer_tests
20/20 Test #20: run_scopetimer_tests ................   Passed    1.49 sec

100% tests passed, 0 tests failed out of 20

Total Test time (real) =   3.09 sec
```

<!-- markdownlint-enable MD013 -->
