# -----------------------------------------------------------------------------
# ScopeTimer - CMake configuration and build scripts
# Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# In accordance with section 13 of the AGPL, if you modify this program,
# your modified version must prominently offer all users interacting with it
# remotely through a computer network an opportunity to receive the source
# code of your version.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(ScopeTimer LANGUAGES CXX)

# --- Basics ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Export compile DB (used by Sonar via build-wrapper)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
find_package(Threads REQUIRED)

# Header-only consumer target. Keep the namespaced alias available to vendored
# CMake consumers without adding install/export packaging to this development
# build.
add_library(ScopeTimer INTERFACE)
add_library(ScopeTimer::ScopeTimer ALIAS ScopeTimer)
target_compile_features(ScopeTimer INTERFACE cxx_std_17)
target_include_directories(ScopeTimer INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(ScopeTimer INTERFACE Threads::Threads)

# --- Demo app -------------------------------------------------------------
# Use example/Demo.cpp if present, else ./Demo.cpp.
if(EXISTS "${CMAKE_SOURCE_DIR}/example/Demo.cpp")
  set(DEMO_SRC example/Demo.cpp)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/Demo.cpp")
  set(DEMO_SRC Demo.cpp)
else()
  message(FATAL_ERROR "No Demo.cpp found (looked in example/ and project root)")
endif()

add_executable(Demo ${DEMO_SRC})
target_link_libraries(Demo PRIVATE ScopeTimer::ScopeTimer)

# --- Benchmark app --------------------------------------------------------
# Keep the dedicated benchmark workload separate from the educational demo.
if(EXISTS "${CMAKE_SOURCE_DIR}/example/Benchmark.cpp")
  set(BENCHMARK_SRC example/Benchmark.cpp)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/Benchmark.cpp")
  set(BENCHMARK_SRC Benchmark.cpp)
else()
  message(FATAL_ERROR "No Benchmark.cpp found (looked in example/ and project root)")
endif()

add_executable(Benchmark ${BENCHMARK_SRC})
target_link_libraries(Benchmark PRIVATE ScopeTimer::ScopeTimer)

# --- Unit tests (optional) -------------------------------------------------
set(TEST_TARGETS)
if(BUILD_TESTING AND EXISTS "${CMAKE_SOURCE_DIR}/test/ScopeTimerTest.cpp")
  add_executable(scopetimer_tests test/ScopeTimerTest.cpp)
  target_link_libraries(scopetimer_tests PRIVATE ScopeTimer::ScopeTimer)
  target_compile_definitions(scopetimer_tests PRIVATE SCOPETIMER_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
  list(APPEND TEST_TARGETS scopetimer_tests)
endif()

if(BUILD_TESTING AND EXISTS "${CMAKE_SOURCE_DIR}/test/ScopeTimerReleaseCompileTest.cpp")
  add_executable(scopetimer_release_compile_test test/ScopeTimerReleaseCompileTest.cpp)
  target_link_libraries(scopetimer_release_compile_test PRIVATE ScopeTimer::ScopeTimer)
  target_compile_definitions(scopetimer_release_compile_test PRIVATE NDEBUG)
  list(APPEND TEST_TARGETS scopetimer_release_compile_test)
endif()

# --- CTest registration -----------------------------------------------------
function(scopetimer_set_test_working_directory)
  set_tests_properties(${ARGN} PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endfunction()

function(scopetimer_set_benchmark_test_env TEST_NAME TEST_ENV)
  set_tests_properties(
    ${TEST_NAME}
    PROPERTIES
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      ENVIRONMENT "${TEST_ENV}"
  )
endfunction()

if(BUILD_TESTING AND TARGET Demo)
  add_test(NAME run_demo COMMAND Demo)
  add_test(NAME run_demo_iterations COMMAND Demo --iterations=1)
  add_test(NAME run_demo_iterations_zero_flag COMMAND Demo --iterations=0)
  add_test(NAME run_demo_positional_iterations COMMAND Demo 2)
  add_test(NAME run_demo_positional_zero COMMAND Demo 0)
  add_test(NAME run_demo_help COMMAND Demo --help)
  scopetimer_set_test_working_directory(
    run_demo
    run_demo_iterations
    run_demo_iterations_zero_flag
    run_demo_positional_iterations
    run_demo_positional_zero
    run_demo_help
  )
  set_tests_properties(
    run_demo_iterations_zero_flag
    run_demo_positional_zero
    PROPERTIES WILL_FAIL TRUE
  )
endif()

if(BUILD_TESTING AND TARGET Benchmark)
  add_test(NAME run_benchmark_default COMMAND Benchmark --iterations=1 --scenario=hotpath-bench)
  add_test(NAME run_benchmark_iterations_zero COMMAND Benchmark --iterations=0 --scenario=hotpath-bench)
  add_test(NAME run_benchmark_positional_zero COMMAND Benchmark 0)
  add_test(NAME run_benchmark_help COMMAND Benchmark --help)
  if(NOT CMAKE_CONFIGURATION_TYPES AND
     NOT CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
    add_test(
      NAME run_benchmark_instrumentation_status
      COMMAND Benchmark --instrumentation-status
    )
    set_tests_properties(
      run_benchmark_instrumentation_status
      PROPERTIES
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        PASS_REGULAR_EXPRESSION "^ScopeTimerBenchmark protocol=1 instrumentation=enabled"
    )
  endif()
  add_test(NAME run_benchmark_invalid_scenario COMMAND Benchmark --scenario=bogus)
  add_test(NAME run_benchmark_buffered_hotpath COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_buffered_fast_alias COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_async COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_null COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_null_standard_alias COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_noop_alias COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_async_invalid_env COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_out_of_range_env COMMAND Benchmark --iterations=1)
  scopetimer_set_test_working_directory(
    run_benchmark_default
    run_benchmark_iterations_zero
    run_benchmark_positional_zero
    run_benchmark_help
    run_benchmark_invalid_scenario
    run_benchmark_buffered_hotpath
    run_benchmark_buffered_fast_alias
    run_benchmark_async
    run_benchmark_null
    run_benchmark_null_standard_alias
    run_benchmark_noop_alias
    run_benchmark_async_invalid_env
    run_benchmark_out_of_range_env
  )
  set_tests_properties(
    run_benchmark_iterations_zero
    run_benchmark_positional_zero
    run_benchmark_invalid_scenario
    run_benchmark_async_invalid_env
    run_benchmark_out_of_range_env
    PROPERTIES WILL_FAIL TRUE
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_buffered_hotpath
    "SCOPE_TIMER_BENCH_SINK=BUFFERED;SCOPE_TIMER_BENCH_TIMER=HOTPATH;SCOPE_TIMER_BENCH_THREADS=2"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_buffered_fast_alias
    "SCOPE_TIMER_BENCH_SINK=buffered;SCOPE_TIMER_BENCH_TIMER=fast;SCOPE_TIMER_BENCH_THREADS=1;SCOPE_TIMER_BENCH_SINK_BYTES=1"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_async
    "SCOPE_TIMER_BENCH_SINK=ASYNC"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_null
    "SCOPE_TIMER_BENCH_SINK=NULL;SCOPE_TIMER_BENCH_TIMER=HOTPATH"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_async_invalid_env
    "SCOPE_TIMER_BENCH_SINK=async;SCOPE_TIMER_BENCH_THREADS=bogus;SCOPE_TIMER_BENCH_SINK_BYTES=bogus"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_null_standard_alias
    "SCOPE_TIMER_BENCH_SINK=null"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_noop_alias
    "SCOPE_TIMER_BENCH_SINK=noop"
  )
  scopetimer_set_benchmark_test_env(
    run_benchmark_out_of_range_env
    "SCOPE_TIMER_BENCH_THREADS=9999999999999999999999999999;SCOPE_TIMER_BENCH_SINK_BYTES=9999999999999999999999999999"
  )
endif()

if(TARGET scopetimer_tests)
  add_test(NAME run_scopetimer_tests COMMAND scopetimer_tests)
  set_tests_properties(run_scopetimer_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(TARGET scopetimer_release_compile_test)
  add_test(NAME run_scopetimer_release_compile_test COMMAND scopetimer_release_compile_test)
  set_tests_properties(
    run_scopetimer_release_compile_test
    PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  )
endif()

find_program(LEAKS_EXECUTABLE NAMES leaks)
find_program(VALGRIND_EXECUTABLE NAMES valgrind)

if(TARGET scopetimer_tests)
  if(APPLE AND LEAKS_EXECUTABLE)
    add_custom_target(leak_check
      COMMENT "Run scopetimer_tests under macOS leaks"
      COMMAND ${LEAKS_EXECUTABLE}
              -quiet
              -noContent
              -atExit
              --
              $<TARGET_FILE:scopetimer_tests>
      DEPENDS scopetimer_tests
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      VERBATIM
    )
  elseif(UNIX AND NOT APPLE AND VALGRIND_EXECUTABLE)
    add_custom_target(leak_check
      COMMENT "Run scopetimer_tests under Valgrind leak checking"
      COMMAND ${CMAKE_COMMAND} -E env
              # Skip timing-threshold assertions during Valgrind runs. They are
              # performance checks, not correctness checks, and Valgrind's
              # instrumentation makes those thresholds meaningless.
              SCOPETIMER_PERF_MODE=SKIP
              ${VALGRIND_EXECUTABLE}
              --leak-check=full
              --show-leak-kinds=all
              --errors-for-leak-kinds=definite,indirect
              --error-exitcode=1
              --num-callers=20
              $<TARGET_FILE:scopetimer_tests>
      DEPENDS scopetimer_tests
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      VERBATIM
    )
  else()
    add_custom_target(leak_check
      COMMENT "ERROR: leak_check requires macOS leaks or Linux Valgrind plus scopetimer_tests"
      COMMAND ${CMAKE_COMMAND} -E echo "[leak_check] ERROR: install macOS leaks or Linux valgrind, and ensure scopetimer_tests is enabled."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endif()
endif()

# --- Performance comparison helper ---------------------------------------
if(TARGET scopetimer_tests)
  add_custom_target(perf_runs
    COMMENT "Run performance tests: disabled (SCOPE_TIMER=0), default sink, thread-buffered sink"
    COMMAND ${CMAKE_COMMAND} -E echo "=== Perf: timers disabled \\(SCOPE_TIMER=0, default sink\\) ==="
    COMMAND ${CMAKE_COMMAND} -E time ${CMAKE_COMMAND} -E env SCOPE_TIMER=0 SCOPETIMER_PERF_MODE=DEFAULT_ONLY ${CMAKE_BINARY_DIR}/scopetimer_tests
    COMMAND ${CMAKE_COMMAND} -E echo "=== Perf: timers enabled, default sink ==="
    COMMAND ${CMAKE_COMMAND} -E time ${CMAKE_COMMAND} -E env SCOPE_TIMER=1 SCOPETIMER_PERF_MODE=DEFAULT_ONLY ${CMAKE_BINARY_DIR}/scopetimer_tests
    COMMAND ${CMAKE_COMMAND} -E echo "=== Perf: timers enabled, thread-buffered sink ==="
    COMMAND ${CMAKE_COMMAND} -E time ${CMAKE_COMMAND} -E env SCOPE_TIMER=1 SCOPETIMER_PERF_MODE=BUFFERED_ONLY ${CMAKE_BINARY_DIR}/scopetimer_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS scopetimer_tests
  )
endif()

# --- Benchmark helper -----------------------------------------------------
# Reproduces the dedicated benchmark-app enabled-vs-disabled ScopeTimer measurement
# in a dedicated non-coverage build tree so gcov instrumentation does not skew
# the results.
set(_SCOPETIMER_LEGACY_BENCH_BUILD_DIR "${CMAKE_SOURCE_DIR}/build-bench")
set(_SCOPETIMER_DEFAULT_BENCH_BUILD_DIR "${CMAKE_BINARY_DIR}/benchmark-build")
if(NOT DEFINED DEMO_BENCH_BUILD_DIR
   OR "${DEMO_BENCH_BUILD_DIR}" STREQUAL "${_SCOPETIMER_LEGACY_BENCH_BUILD_DIR}")
  set(DEMO_BENCH_BUILD_DIR "${_SCOPETIMER_DEFAULT_BENCH_BUILD_DIR}" CACHE PATH
      "Dedicated build directory used by the benchmark target" FORCE)
else()
  set(DEMO_BENCH_BUILD_DIR "${DEMO_BENCH_BUILD_DIR}" CACHE PATH
      "Dedicated build directory used by the benchmark target")
endif()
set(DEMO_BENCH_ITERATIONS "5" CACHE STRING
    "Number of Benchmark iterations per benchmark run")
set(DEMO_BENCH_RUNS "8" CACHE STRING
    "Number of alternating enabled/disabled benchmark runs")
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
  set(_DEMO_BENCH_DEFAULT_CXX_FLAGS "-O3")
elseif(MSVC)
  set(_DEMO_BENCH_DEFAULT_CXX_FLAGS "/O2")
else()
  set(_DEMO_BENCH_DEFAULT_CXX_FLAGS "-O3")
endif()
set(DEMO_BENCH_CXX_FLAGS "${_DEMO_BENCH_DEFAULT_CXX_FLAGS}" CACHE STRING
    "Optimization flags used by the dedicated benchmark build without defining NDEBUG")
set(DEMO_BENCH_THREADS "4" CACHE STRING
    "Worker thread count used by the benchmark matrix target")
set(DEMO_BENCH_SINK_BYTES "4096" CACHE STRING
    "Flush threshold used by buffered and async benchmark matrix runs")
set(DEMO_BENCH_HISTORY_FILE "${CMAKE_SOURCE_DIR}/benchmarks/demo_benchmark_history.json" CACHE FILEPATH
    "History file updated by the benchmark matrix target")
set(DEMO_BENCH_REPORT_FILE "${CMAKE_SOURCE_DIR}/BENCHMARK.md" CACHE FILEPATH
    "Markdown report updated by the benchmark matrix target")
set(SCOPETIMER_HEADER_COVERAGE_BUILD_DIR "${CMAKE_BINARY_DIR}/scopetimer-header-coverage" CACHE PATH
    "Directory used by the ScopeTimer.hpp header coverage target")
set(SCOPETIMER_HEADER_COVERAGE_THRESHOLD "80" CACHE STRING
    "Minimum ScopeTimer.hpp line coverage percentage enforced by the header coverage target")
set(DOCS_BUILD_DIR "${CMAKE_BINARY_DIR}/docs-refresh")
option(AUTO_REFRESH_DOCS "Automatically refresh BUILD.md and TESTS.md during the default build" OFF)
find_package(Python3 COMPONENTS Interpreter QUIET)

if(Python3_Interpreter_FOUND)
  if(BUILD_TESTING)
    add_test(
      NAME run_benchmark_tool_tests
      COMMAND ${Python3_EXECUTABLE} -m unittest discover -s scripts/tests -v
    )
    set_tests_properties(
      run_benchmark_tool_tests
      PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_test(
      NAME run_refresh_docs_tests
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/RefreshDocsTest.py
    )
    set_tests_properties(
      run_refresh_docs_tests
      PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )

    add_test(
      NAME run_sonar_branch_guard_tests
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/SonarBranchGuardTest.py
    )
    set_tests_properties(
      run_sonar_branch_guard_tests
      PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
  endif()

  function(scopetimer_add_docs_refresh_target)
    set(target_options)
    if(AUTO_REFRESH_DOCS)
      list(APPEND target_options ALL)
    endif()
    add_custom_target(docs_refresh ${target_options}
      COMMENT "Regenerate BUILD.md and TESTS.md from live build artifacts"
      COMMAND ${Python3_EXECUTABLE}
              ${CMAKE_SOURCE_DIR}/scripts/refresh_docs.py
              --repo-root ${CMAKE_SOURCE_DIR}
              --managed-root ${CMAKE_BINARY_DIR}
              --docs-build-dir ${DOCS_BUILD_DIR}
              --build-doc ${CMAKE_SOURCE_DIR}/BUILD.md
              --tests-doc ${CMAKE_SOURCE_DIR}/TESTS.md
      VERBATIM
    )
  endfunction()

  add_custom_target(scopetimer_header_coverage
    COMMENT "Measure ScopeTimer.hpp with clang source-based coverage and enforce the configured threshold"
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/scripts/check_scope_timer_header_coverage.py
            --repo-root ${CMAKE_SOURCE_DIR}
            --build-dir ${SCOPETIMER_HEADER_COVERAGE_BUILD_DIR}
            --threshold ${SCOPETIMER_HEADER_COVERAGE_THRESHOLD}
    VERBATIM
  )

  add_custom_target(demo_benchmark
    COMMENT "Build a clean optimized Benchmark binary and benchmark ScopeTimer enabled vs disabled"
    COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${DEMO_BENCH_BUILD_DIR}
            -DENABLE_COVERAGE=OFF
            -DENABLE_SONAR=OFF
            -DAUTO_REFRESH_DOCS=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_BUILD_TYPE:STRING=
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_FLAGS=${DEMO_BENCH_CXX_FLAGS}
    COMMAND ${CMAKE_COMMAND} --build ${DEMO_BENCH_BUILD_DIR} --target Benchmark -j
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/scripts/benchmark_demo.py
            --binary ${DEMO_BENCH_BUILD_DIR}/Benchmark
            --scenario hotpath-bench
            --iterations ${DEMO_BENCH_ITERATIONS}
            --runs ${DEMO_BENCH_RUNS}
    VERBATIM
  )

  add_custom_target(demo_benchmark_matrix
    COMMENT "Run the reproducible ScopeTimer optimization benchmark matrix and record the results"
    COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${DEMO_BENCH_BUILD_DIR}
            -DENABLE_COVERAGE=OFF
            -DENABLE_SONAR=OFF
            -DAUTO_REFRESH_DOCS=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_BUILD_TYPE:STRING=
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_FLAGS=${DEMO_BENCH_CXX_FLAGS}
    COMMAND ${CMAKE_COMMAND} --build ${DEMO_BENCH_BUILD_DIR} --target Benchmark -j
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/scripts/record_demo_benchmarks.py
            --binary ${DEMO_BENCH_BUILD_DIR}/Benchmark
            --scenario hotpath-bench
            --iterations ${DEMO_BENCH_ITERATIONS}
            --runs ${DEMO_BENCH_RUNS}
            --threads ${DEMO_BENCH_THREADS}
            --sink-bytes ${DEMO_BENCH_SINK_BYTES}
            --history-file ${DEMO_BENCH_HISTORY_FILE}
            --report-file ${DEMO_BENCH_REPORT_FILE}
            --build-dir ${DEMO_BENCH_BUILD_DIR}
            --cxx-flags=${DEMO_BENCH_CXX_FLAGS}
    VERBATIM
  )

  scopetimer_add_docs_refresh_target()
else()
  function(scopetimer_add_missing_python_target TARGET_NAME ERROR_DETAIL)
    set(target_options)
    if(ARGC GREATER 2 AND ARGV2 STREQUAL "ALL")
      list(APPEND target_options ALL)
    endif()
    add_custom_target(${TARGET_NAME} ${target_options}
      COMMENT "ERROR: ${ERROR_DETAIL}"
      COMMAND ${CMAKE_COMMAND} -E echo "[${TARGET_NAME}] ERROR: Python 3 interpreter not found."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endfunction()

  scopetimer_add_missing_python_target(
    scopetimer_header_coverage
    "Python 3 is required for the ScopeTimer.hpp header coverage helper"
  )
  scopetimer_add_missing_python_target(
    demo_benchmark
    "Python 3 is required for the benchmark helper"
  )
  scopetimer_add_missing_python_target(
    demo_benchmark_matrix
    "Python 3 is required for the benchmark matrix helper"
  )
  if(AUTO_REFRESH_DOCS)
    scopetimer_add_missing_python_target(
      docs_refresh
      "Python 3 is required for automatic markdown refresh"
      ALL
    )
  else()
    scopetimer_add_missing_python_target(
      docs_refresh
      "Python 3 is required for automatic markdown refresh"
    )
  endif()
endif()

# --- Coverage (GCC + gcovr) ----------------------------------------------
# Keep it minimal: GCC only. Generate SonarQube XML + HTML (viewable locally).
option(ENABLE_COVERAGE "Enable GCC gcov coverage flags" OFF)
find_program(GCOVR_EXECUTABLE NAMES gcovr)
if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Demo PRIVATE --coverage -O0 -g)
    target_link_options(Demo PRIVATE --coverage)
    target_compile_options(Benchmark PRIVATE --coverage -O0 -g)
    target_link_options(Benchmark PRIVATE --coverage)
    if(TARGET scopetimer_tests)
      target_compile_options(scopetimer_tests PRIVATE --coverage -O0 -g)
      target_link_options(scopetimer_tests PRIVATE --coverage)
    endif()
  else()
    message(WARNING "ENABLE_COVERAGE=ON but compiler is ${CMAKE_CXX_COMPILER_ID}. Set CC/CXX to GCC to generate gcov data.")
  endif()
endif()

# Auto-pick a gcov that matches the GCC in use (e.g. gcov-15).
# Honor explicit GCOV env if supplied (e.g., gcov-15). We'll skip auto-detect if set.
set(_GCC_MAJOR "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION)
    string(REGEX MATCH "^[0-9]+" _GCC_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
  endif()
endif()
set(COVERAGE_READY OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND GCOVR_EXECUTABLE)
  if(DEFINED ENV{GCOV} AND NOT "$ENV{GCOV}" STREQUAL "")
    set(GCOV_EXE "$ENV{GCOV}")
    message(STATUS "Using GCOV from environment: ${GCOV_EXE}")
  endif()
  if(NOT GCOV_EXE)
    if(_GCC_MAJOR)
      find_program(GCOV_EXE NAMES gcov-${_GCC_MAJOR} gcov REQUIRED)
    else()
      find_program(GCOV_EXE NAMES gcov REQUIRED)
    endif()
  endif()
  if(GCOV_EXE)
    set(COVERAGE_READY ON)
  endif()
elseif(ENABLE_COVERAGE AND NOT GCOVR_EXECUTABLE)
  message(STATUS "gcovr not found. Plain builds still work, but the 'coverage' and 'sonar_scan' targets require gcovr.")
endif()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cmake")
file(WRITE "${CMAKE_BINARY_DIR}/cmake/CleanGcda.cmake" "
file(GLOB_RECURSE gcda \"${CMAKE_BINARY_DIR}/*.gcda\")
if(gcda)
  message(STATUS \"Removing stale .gcda files to avoid stamp mismatch\")
  file(REMOVE \${gcda})
endif()
file(GLOB_RECURSE gcov_json \"${CMAKE_BINARY_DIR}/*.gcov.json.gz\")
if(gcov_json)
  message(STATUS \"Removing stale .gcov.json.gz files to avoid gcovr merge conflicts\")
  file(REMOVE \${gcov_json})
endif()
")

set(COVERAGE_EXCLUDE_ARGS
  --exclude ".*_deps/.*"
  --exclude ".*bw-output/.*"
  --exclude ".*CMakeFiles/.*"
  --exclude ".*build-wrapper-dump.json"
  --exclude "(.*/)?test/.*"
)

if(COVERAGE_READY)
  add_custom_target(coverage DEPENDS Demo Benchmark ${TEST_TARGETS}
    COMMENT "Run tests once and generate gcovr reports (SonarQube XML + HTML)"
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake/CleanGcda.cmake
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${GCOVR_EXECUTABLE}
            --gcov-executable ${GCOV_EXE}
            --root ${CMAKE_SOURCE_DIR}
            --object-directory ${CMAKE_BINARY_DIR}
            --merge-mode-functions merge-use-line-min
            --sonarqube ${CMAKE_BINARY_DIR}/sonarqube-coverage.xml
            --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
            --gcov-ignore-errors=no_working_dir_found
            --gcov-ignore-parse-errors=negative_hits.warn_once_per_file
            ${COVERAGE_EXCLUDE_ARGS}
    COMMAND ${GCOVR_EXECUTABLE}
            --gcov-executable ${GCOV_EXE}
            --root ${CMAKE_SOURCE_DIR}
            --object-directory ${CMAKE_BINARY_DIR}
            --merge-mode-functions merge-use-line-min
            --txt-summary
            --delete
            --gcov-ignore-errors=no_working_dir_found
            --gcov-ignore-parse-errors=negative_hits.warn_once_per_file
            ${COVERAGE_EXCLUDE_ARGS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
else()
  add_custom_target(coverage
    COMMENT "ERROR: coverage prerequisites missing — install gcovr and use a GNU compiler, or configure with -DENABLE_COVERAGE=OFF"
    COMMAND ${CMAKE_COMMAND} -E echo "[coverage] ERROR: coverage requires gcovr plus a GNU compiler/toolchain."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )
endif()

# --- Sonar build-wrapper capture -----------------------------------------
# The scanner configuration now lives entirely in sonar-project.properties.
# This target captures a clean build so the wrapper emits a fresh
# compile_commands.json into ${CMAKE_BINARY_DIR}/bw-output/.
option(ENABLE_SONAR "Enable Sonar build-wrapper and scanner targets" OFF)
set(SONAR_BW_OUT ${CMAKE_BINARY_DIR}/bw-output CACHE PATH "build-wrapper output directory")
set(SONAR_BUILD_WRAPPER "" CACHE FILEPATH "Path to an installed build-wrapper executable")
set(SONAR_SCANNER_EXECUTABLE "" CACHE FILEPATH "Path to an installed sonar-scanner executable")

function(scopetimer_find_build_wrapper OUT_VAR)
  if(SONAR_BUILD_WRAPPER)
    if(EXISTS "${SONAR_BUILD_WRAPPER}" AND NOT IS_DIRECTORY "${SONAR_BUILD_WRAPPER}")
      set(${OUT_VAR} "${SONAR_BUILD_WRAPPER}" PARENT_SCOPE)
    else()
      message(WARNING "Configured SONAR_BUILD_WRAPPER does not name a file: ${SONAR_BUILD_WRAPPER}")
    endif()
    return()
  endif()

  if(DEFINED ENV{SONAR_BUILD_WRAPPER} AND NOT "$ENV{SONAR_BUILD_WRAPPER}" STREQUAL "")
    if(EXISTS "$ENV{SONAR_BUILD_WRAPPER}" AND NOT IS_DIRECTORY "$ENV{SONAR_BUILD_WRAPPER}")
      set(${OUT_VAR} "$ENV{SONAR_BUILD_WRAPPER}" PARENT_SCOPE)
    else()
      message(WARNING "SONAR_BUILD_WRAPPER does not name a file: $ENV{SONAR_BUILD_WRAPPER}")
    endif()
    return()
  endif()

  if(APPLE)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
      find_program(_BW_EXE NAMES build-wrapper-macosx-arm64)
    else()
      find_program(_BW_EXE NAMES build-wrapper-macosx-x86)
    endif()
  elseif(WIN32)
    find_program(_BW_EXE NAMES build-wrapper-win-x86-64.exe)
  else()
    find_program(_BW_EXE NAMES build-wrapper-linux-x86-64)
  endif()
  set(${OUT_VAR} "${_BW_EXE}" PARENT_SCOPE)
endfunction()

if(ENABLE_SONAR)
  scopetimer_find_build_wrapper(SONAR_BUILD_WRAPPER_EXE)
  if(SONAR_BUILD_WRAPPER_EXE)
    message(STATUS "Using Sonar build-wrapper: ${SONAR_BUILD_WRAPPER_EXE}")
    add_custom_target(sonar_bw
      COMMENT "Capture clean build with Sonar build-wrapper -> ${SONAR_BW_OUT}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${SONAR_BW_OUT}
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
      COMMAND ${SONAR_BUILD_WRAPPER_EXE} --out-dir ${SONAR_BW_OUT}
              ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --clean-first -j
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${CMAKE_BINARY_DIR}/compile_commands.json
              ${SONAR_BW_OUT}/compile_commands.json
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      VERBATIM
    )
  else()

    message(STATUS "Sonar build-wrapper is not installed. 'sonar_bw' will print a helpful error.")
    add_custom_target(sonar_bw
      COMMENT "ERROR: Sonar build-wrapper missing — install it or set SONAR_BUILD_WRAPPER"
      COMMAND ${CMAKE_COMMAND} -E echo "[sonar_bw] ERROR: install Sonar build-wrapper or set SONAR_BUILD_WRAPPER to its executable."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endif()
else()
  add_custom_target(sonar_bw
    COMMENT "ERROR: Sonar build-wrapper disabled — configure with -DENABLE_SONAR=ON"
    COMMAND ${CMAKE_COMMAND} -E echo "[sonar_bw] ERROR: configure with -DENABLE_SONAR=ON to enable Sonar build-wrapper tooling."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )
endif()

# --- Sonar scanner ---------------------------------------------------------
# Runs sonar-scanner using sonar-project.properties at repo root.
# Depends on coverage and sonar_bw so the active build directory always
# provides the compile database and SonarQube coverage report.
function(scopetimer_find_scanner OUT_VAR)
  if(SONAR_SCANNER_EXECUTABLE)
    if(EXISTS "${SONAR_SCANNER_EXECUTABLE}" AND NOT IS_DIRECTORY "${SONAR_SCANNER_EXECUTABLE}")
      set(${OUT_VAR} "${SONAR_SCANNER_EXECUTABLE}" PARENT_SCOPE)
    else()
      message(WARNING "Configured SONAR_SCANNER_EXECUTABLE does not name a file: ${SONAR_SCANNER_EXECUTABLE}")
    endif()
    return()
  endif()

  if(DEFINED ENV{SONAR_SCANNER_EXECUTABLE} AND NOT "$ENV{SONAR_SCANNER_EXECUTABLE}" STREQUAL "")
    if(EXISTS "$ENV{SONAR_SCANNER_EXECUTABLE}" AND NOT IS_DIRECTORY "$ENV{SONAR_SCANNER_EXECUTABLE}")
      set(${OUT_VAR} "$ENV{SONAR_SCANNER_EXECUTABLE}" PARENT_SCOPE)
    else()
      message(WARNING "SONAR_SCANNER_EXECUTABLE does not name a file: $ENV{SONAR_SCANNER_EXECUTABLE}")
    endif()
    return()
  endif()

  find_program(_SC_EXE NAMES sonar-scanner sonar-scanner.bat)
  set(${OUT_VAR} "${_SC_EXE}" PARENT_SCOPE)
endfunction()

function(scopetimer_current_branch OUT_VAR)
  if(DEFINED ENV{GITHUB_REF_NAME} AND NOT "$ENV{GITHUB_REF_NAME}" STREQUAL "")
    set(${OUT_VAR} "$ENV{GITHUB_REF_NAME}" PARENT_SCOPE)
    return()
  endif()

  find_program(_GIT_EXE NAMES git)
  if(NOT _GIT_EXE)
    set(${OUT_VAR} "" PARENT_SCOPE)
    return()
  endif()

  execute_process(
    COMMAND ${_GIT_EXE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE _git_result
    OUTPUT_VARIABLE _git_branch
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(_git_result EQUAL 0 AND NOT _git_branch STREQUAL "HEAD")
    set(${OUT_VAR} "${_git_branch}" PARENT_SCOPE)
  else()
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()

if(ENABLE_SONAR)
  scopetimer_current_branch(SONAR_BRANCH)
  if(NOT SONAR_BRANCH STREQUAL "main")
    if(SONAR_BRANCH)
      set(_SONAR_BRANCH_DETAIL "current branch is '${SONAR_BRANCH}'")
    else()
      set(_SONAR_BRANCH_DETAIL "the current branch could not be identified")
    endif()
    add_custom_target(sonar_scan
      COMMENT "ERROR: Sonar scanning is restricted to the main branch"
      COMMAND ${CMAKE_COMMAND}
              -DSCOPETIMER_SOURCE_DIR:PATH=${CMAKE_SOURCE_DIR}
              -P ${CMAKE_SOURCE_DIR}/cmake/RequireSonarMainBranch.cmake
      COMMAND ${CMAKE_COMMAND} -E echo "[sonar_scan] ERROR: ${_SONAR_BRANCH_DETAIL}; free-tier analysis is main-only."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  else()
    scopetimer_find_scanner(SONAR_SCANNER_EXE)
  endif()
  if(SONAR_BRANCH STREQUAL "main" AND SONAR_SCANNER_EXE)
    add_custom_target(sonar_scan
      COMMENT "Run sonar build-wrapper, generate coverage, then run sonar-scanner"
      COMMAND ${CMAKE_COMMAND}
              -DSCOPETIMER_SOURCE_DIR:PATH=${CMAKE_SOURCE_DIR}
              -P ${CMAKE_SOURCE_DIR}/cmake/RequireSonarMainBranch.cmake
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sonar_bw
      COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage
      COMMAND ${SONAR_SCANNER_EXE}
              --define sonar.projectBaseDir=${CMAKE_SOURCE_DIR}
              --define sonar.cfamily.compile-commands=${SONAR_BW_OUT}/compile_commands.json
              --define sonar.coverageReportPaths=${CMAKE_BINARY_DIR}/sonarqube-coverage.xml
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      VERBATIM
    )
  elseif(SONAR_BRANCH STREQUAL "main")
    message(STATUS "sonar-scanner is not installed. 'sonar_scan' will print a helpful error.")
    add_custom_target(sonar_scan
      COMMENT "ERROR: sonar-scanner missing — install it or set SONAR_SCANNER_EXECUTABLE"
      COMMAND ${CMAKE_COMMAND}
              -DSCOPETIMER_SOURCE_DIR:PATH=${CMAKE_SOURCE_DIR}
              -P ${CMAKE_SOURCE_DIR}/cmake/RequireSonarMainBranch.cmake
      COMMAND ${CMAKE_COMMAND} -E echo "[sonar_scan] ERROR: install sonar-scanner or set SONAR_SCANNER_EXECUTABLE to its executable."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endif()
else()
  add_custom_target(sonar_scan
    COMMENT "ERROR: Sonar scanner disabled — configure with -DENABLE_SONAR=ON"
    COMMAND ${CMAKE_COMMAND} -E echo "[sonar_scan] ERROR: configure with -DENABLE_SONAR=ON to enable Sonar scanner tooling."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )
endif()
