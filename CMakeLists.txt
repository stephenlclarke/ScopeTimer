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

# --- Prefer GCC by default (for coverage) --------------------------------
# Set CC/CXX/GCOV before the project() call so CMake configures with GCC
# unless the user has explicitly chosen something else.
if(NOT DEFINED ENV{CC})
  find_program(_GCC15 gcc-15 PATHS /opt/homebrew/bin /usr/local/bin /usr/bin)
  if(_GCC15)
    set(ENV{CC} "${_GCC15}")
  else()
    find_program(_GCC gcc PATHS /opt/homebrew/bin /usr/local/bin /usr/bin)
    if(_GCC)
      set(ENV{CC} "${_GCC}")
    endif()
  endif()
endif()
if(NOT DEFINED ENV{CXX})
  find_program(_GPP15 g++-15 PATHS /opt/homebrew/bin /usr/local/bin /usr/bin)
  if(_GPP15)
    set(ENV{CXX} "${_GPP15}")
  else()
    find_program(_GPP g++ PATHS /opt/homebrew/bin /usr/local/bin /usr/bin)
    if(_GPP)
      set(ENV{CXX} "${_GPP}")
    endif()
  endif()
endif()
if(NOT DEFINED ENV{GCOV})
  find_program(_GCOV15 gcov-15 PATHS /opt/homebrew/bin /usr/local/bin /usr/bin)
  if(_GCOV15)
    set(ENV{GCOV} "${_GCOV15}")
  endif()
endif()

if(NOT CMAKE_C_COMPILER AND DEFINED ENV{CC})
  set(CMAKE_C_COMPILER "$ENV{CC}" CACHE FILEPATH "C compiler" FORCE)
endif()
if(NOT CMAKE_CXX_COMPILER AND DEFINED ENV{CXX})
  set(CMAKE_CXX_COMPILER "$ENV{CXX}" CACHE FILEPATH "C++ compiler" FORCE)
endif()

project(ScopeTimer LANGUAGES CXX)

# --- Basics ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Export compile DB (used by Sonar via build-wrapper)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
target_include_directories(Demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

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
target_include_directories(Benchmark PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Unit tests (optional) -------------------------------------------------
set(TEST_TARGET "")
if(EXISTS "${CMAKE_SOURCE_DIR}/test/ScopeTimerTest.cpp")
  add_executable(scopetimer_tests test/ScopeTimerTest.cpp)
  target_include_directories(scopetimer_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_compile_definitions(scopetimer_tests PRIVATE SCOPETIMER_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(scopetimer_tests PRIVATE --coverage -O0 -g)
    target_link_options(scopetimer_tests PRIVATE --coverage)
  endif()
  set(TEST_TARGET scopetimer_tests)
endif()

# --- CTest registration -----------------------------------------------------
include(CTest)
enable_testing()

if(TARGET Demo)
  add_test(NAME run_demo COMMAND Demo)
  add_test(NAME run_demo_iterations COMMAND Demo --iterations=1)
  add_test(NAME run_demo_help COMMAND Demo --help)
  set_tests_properties(run_demo PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_tests_properties(run_demo_iterations PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_tests_properties(run_demo_help PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(TARGET Benchmark)
  add_test(NAME run_benchmark_default COMMAND Benchmark --iterations=1 --scenario=hotpath-bench)
  add_test(NAME run_benchmark_help COMMAND Benchmark --help)
  add_test(NAME run_benchmark_buffered_hotpath COMMAND Benchmark --iterations=1)
  add_test(NAME run_benchmark_async COMMAND Benchmark --iterations=1)
  set_tests_properties(run_benchmark_default PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_tests_properties(run_benchmark_help PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_tests_properties(
    run_benchmark_buffered_hotpath
    PROPERTIES
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      ENVIRONMENT "SCOPE_TIMER_BENCH_SINK=BUFFERED;SCOPE_TIMER_BENCH_TIMER=HOTPATH;SCOPE_TIMER_BENCH_THREADS=2"
  )
  set_tests_properties(
    run_benchmark_async
    PROPERTIES
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      ENVIRONMENT "SCOPE_TIMER_BENCH_SINK=ASYNC"
  )
endif()

if(TARGET scopetimer_tests)
  add_test(NAME run_scopetimer_tests COMMAND scopetimer_tests)
  set_tests_properties(run_scopetimer_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
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
set(DEMO_BENCH_BUILD_DIR "${CMAKE_SOURCE_DIR}/build-bench" CACHE PATH
    "Dedicated build directory used by the benchmark target")
set(DEMO_BENCH_ITERATIONS "5" CACHE STRING
    "Number of Benchmark iterations per benchmark run")
set(DEMO_BENCH_RUNS "8" CACHE STRING
    "Number of alternating enabled/disabled benchmark runs")
set(DEMO_BENCH_CXX_FLAGS "-O2" CACHE STRING
    "Extra C++ flags used by the dedicated benchmark build")
set(DEMO_BENCH_THREADS "4" CACHE STRING
    "Worker thread count used by the benchmark matrix target")
set(DEMO_BENCH_SINK_BYTES "4096" CACHE STRING
    "Flush threshold used by buffered and async benchmark matrix runs")
set(DEMO_BENCH_HISTORY_FILE "${CMAKE_SOURCE_DIR}/benchmarks/demo_benchmark_history.json" CACHE FILEPATH
    "History file updated by the benchmark matrix target")
set(DEMO_BENCH_REPORT_FILE "${CMAKE_SOURCE_DIR}/BENCHMARK.md" CACHE FILEPATH
    "Markdown report updated by the benchmark matrix target")
set(DOCS_BUILD_DIR "${CMAKE_SOURCE_DIR}/build-docs" CACHE PATH
    "Dedicated build directory used by the automatic markdown refresh target")
option(AUTO_REFRESH_DOCS "Automatically refresh BUILD.md and TESTS.md during the default build" ON)
find_package(Python3 COMPONENTS Interpreter QUIET)

if(Python3_Interpreter_FOUND)
  add_custom_target(demo_benchmark
    COMMENT "Build a clean optimized Benchmark binary and benchmark ScopeTimer enabled vs disabled"
    COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${DEMO_BENCH_BUILD_DIR}
            -DENABLE_COVERAGE=OFF
            -DENABLE_SONAR=OFF
            -DAUTO_REFRESH_DOCS=OFF
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

  if(AUTO_REFRESH_DOCS)
    add_custom_target(docs_refresh ALL
      COMMENT "Regenerate BUILD.md and TESTS.md from live build artifacts"
      COMMAND ${Python3_EXECUTABLE}
              ${CMAKE_SOURCE_DIR}/scripts/refresh_docs.py
              --docs-build-dir ${DOCS_BUILD_DIR}
              --build-doc ${CMAKE_SOURCE_DIR}/BUILD.md
              --tests-doc ${CMAKE_SOURCE_DIR}/TESTS.md
      VERBATIM
    )
  else()
    add_custom_target(docs_refresh
      COMMENT "Regenerate BUILD.md and TESTS.md from live build artifacts"
      COMMAND ${Python3_EXECUTABLE}
              ${CMAKE_SOURCE_DIR}/scripts/refresh_docs.py
              --docs-build-dir ${DOCS_BUILD_DIR}
              --build-doc ${CMAKE_SOURCE_DIR}/BUILD.md
              --tests-doc ${CMAKE_SOURCE_DIR}/TESTS.md
      VERBATIM
    )
  endif()
else()
  add_custom_target(demo_benchmark
    COMMENT "ERROR: Python 3 is required for the benchmark helper"
    COMMAND ${CMAKE_COMMAND} -E echo "[demo_benchmark] ERROR: Python 3 interpreter not found."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )

  add_custom_target(demo_benchmark_matrix
    COMMENT "ERROR: Python 3 is required for the benchmark matrix helper"
    COMMAND ${CMAKE_COMMAND} -E echo "[demo_benchmark_matrix] ERROR: Python 3 interpreter not found."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )

  if(AUTO_REFRESH_DOCS)
    add_custom_target(docs_refresh ALL
      COMMENT "ERROR: Python 3 is required for automatic markdown refresh"
      COMMAND ${CMAKE_COMMAND} -E echo "[docs_refresh] ERROR: Python 3 interpreter not found."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  else()
    add_custom_target(docs_refresh
      COMMENT "ERROR: Python 3 is required for automatic markdown refresh"
      COMMAND ${CMAKE_COMMAND} -E echo "[docs_refresh] ERROR: Python 3 interpreter not found."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endif()
endif()

# Link threads if available (portable)
find_package(Threads)
if(Threads_FOUND)
  target_link_libraries(Demo PRIVATE Threads::Threads)
  target_link_libraries(Benchmark PRIVATE Threads::Threads)
endif()

# --- Coverage (GCC + gcovr) ----------------------------------------------
# Keep it minimal: GCC only. Generate SonarQube XML + HTML (viewable locally).
option(ENABLE_COVERAGE "Enable GCC gcov coverage flags for Demo" ON)
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
  file(REMOVE ${gcda})
endif()
")

if(COVERAGE_READY)
  add_custom_target(coverage DEPENDS Demo Benchmark ${TEST_TARGET}
    COMMENT "Run tests once and generate gcovr reports (SonarQube XML + HTML)"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake/CleanGcda.cmake
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${GCOVR_EXECUTABLE}
            --gcov-executable ${GCOV_EXE}
            --root ${CMAKE_SOURCE_DIR}
            --object-directory ${CMAKE_BINARY_DIR}
            --sonarqube ${CMAKE_BINARY_DIR}/sonarqube-coverage.xml
            --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
            --print-summary
            --delete
            --gcov-ignore-errors=no_working_dir_found
            --gcov-ignore-parse-errors=negative_hits.warn_once_per_file
            --exclude '.*_deps/.*'
            --exclude '.*bw-output/.*'
            --exclude '.*CMakeFiles/.*'
            --exclude '.*build-wrapper-dump.json'
            ${CMAKE_BINARY_DIR}
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
option(ENABLE_SONAR "Enable Sonar build-wrapper target" ON)
set(SONAR_BW_OUT ${CMAKE_BINARY_DIR}/bw-output CACHE PATH "build-wrapper output directory")
# Optional explicit override to the executable; if empty we auto-detect / auto-install.
set(SONAR_BUILD_WRAPPER "" CACHE FILEPATH "Path to build-wrapper executable (optional; auto-detect if empty)")

# Where we place downloaded tools if missing
set(SONAR_TOOLS_DIR ${CMAKE_BINARY_DIR}/tools)

function(_detect_or_install_build_wrapper OUT_VAR)
  # 1) explicit cache var wins
  if(SONAR_BUILD_WRAPPER)
    if(EXISTS "${SONAR_BUILD_WRAPPER}")
      set(${OUT_VAR} "${SONAR_BUILD_WRAPPER}" PARENT_SCOPE)
      return()
    endif()
  endif()
  # 2) environment overrides
  if(DEFINED ENV{SONAR_BUILD_WRAPPER} AND EXISTS "$ENV{SONAR_BUILD_WRAPPER}")
    set(${OUT_VAR} "$ENV{SONAR_BUILD_WRAPPER}" PARENT_SCOPE)
    return()
  endif()
  # 3) search PATH
  if(APPLE)
    find_program(_BW_EXE NAMES build-wrapper-macosx-arm64 build-wrapper-macosx-x86)
  elseif(WIN32)
    find_program(_BW_EXE NAMES build-wrapper-win-x86-64.exe)
  else()
    find_program(_BW_EXE NAMES build-wrapper-linux-x86-64)
  endif()
  if(_BW_EXE)
    set(${OUT_VAR} "${_BW_EXE}" PARENT_SCOPE)
    return()
  endif()

  # 4) Not found: download + install locally under ${SONAR_TOOLS_DIR}
  file(MAKE_DIRECTORY "${SONAR_TOOLS_DIR}")
  if(APPLE)
    # Prefer arm64 if available; fall back to x86 (Rosetta). Some setups only have x86.
    set(_BW_ZIP_URL "https://sonarcloud.io/static/cpp/build-wrapper-macosx-arm64.zip")
    set(_BW_DIR "${SONAR_TOOLS_DIR}/build-wrapper-macosx-arm64")
    set(_BW_EXE_PATH "${_BW_DIR}/build-wrapper-macosx-arm64")
    # We'll attempt arm64 first; if download fails, try x86
    set(_BW_FALLBACK_URL "https://sonarcloud.io/static/cpp/build-wrapper-macosx-x86.zip")
    set(_BW_FALLBACK_DIR "${SONAR_TOOLS_DIR}/build-wrapper-macosx-x86")
    set(_BW_FALLBACK_EXE "${_BW_FALLBACK_DIR}/build-wrapper-macosx-x86")
  elseif(WIN32)
    set(_BW_ZIP_URL "https://sonarcloud.io/static/cpp/build-wrapper-win-x86.zip")
    set(_BW_DIR "${SONAR_TOOLS_DIR}/build-wrapper-win-x86")
    set(_BW_EXE_PATH "${_BW_DIR}/build-wrapper-win-x86-64.exe")
  else()
    set(_BW_ZIP_URL "https://sonarcloud.io/static/cpp/build-wrapper-linux-x86-64.zip")
    set(_BW_DIR "${SONAR_TOOLS_DIR}/build-wrapper-linux-x86-64")
    set(_BW_EXE_PATH "${_BW_DIR}/build-wrapper-linux-x86-64")
  endif()

  set(_BW_ZIP_FILE "${SONAR_TOOLS_DIR}/build-wrapper.zip")
  message(STATUS "Downloading Sonar build-wrapper from: ${_BW_ZIP_URL}")
  file(DOWNLOAD "${_BW_ZIP_URL}" "${_BW_ZIP_FILE}" STATUS _dl_stat)
  list(GET _dl_stat 0 _dl_code)
  if(NOT _dl_code EQUAL 0)
    if(APPLE)
      message(STATUS "Arm64 build-wrapper download failed; trying x86 fallback: ${_BW_FALLBACK_URL}")
      file(DOWNLOAD "${_BW_FALLBACK_URL}" "${_BW_ZIP_FILE}" STATUS _dl_stat2)
      list(GET _dl_stat2 0 _dl_code2)
      if(NOT _dl_code2 EQUAL 0)
        message(WARNING "Failed to download build-wrapper (arm64 & x86). Configure SONAR_BUILD_WRAPPER manually.")
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
      endif()
      set(_BW_DIR "${_BW_FALLBACK_DIR}")
      set(_BW_EXE_PATH "${_BW_FALLBACK_EXE}")
    else()
      message(WARNING "Failed to download build-wrapper. Configure SONAR_BUILD_WRAPPER manually.")
      set(${OUT_VAR} "" PARENT_SCOPE)
      return()
    endif()
  endif()

  # Extract zip
  file(MAKE_DIRECTORY "${_BW_DIR}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${_BW_ZIP_FILE}"
                  WORKING_DIRECTORY "${SONAR_TOOLS_DIR}"
                  RESULT_VARIABLE _unzip_res)
  if(NOT _unzip_res EQUAL 0)
    message(WARNING "Failed to extract build-wrapper archive at ${_BW_ZIP_FILE}.")
    set(${OUT_VAR} "" PARENT_SCOPE)
    return()
  endif()

  if(NOT EXISTS "${_BW_EXE_PATH}")
    # Windows archives sometimes extract into a nested folder; try to glob for the exe
    if(WIN32)
      file(GLOB_RECURSE _BW_WIN_EXE "${_BW_DIR}/build-wrapper-win-x86-64.exe")
      if(_BW_WIN_EXE)
        list(GET _BW_WIN_EXE 0 _BW_EXE_PATH)
      endif()
    endif()
  endif()

  if(EXISTS "${_BW_EXE_PATH}")
    message(STATUS "Installed build-wrapper locally: ${_BW_EXE_PATH}")
    set(${OUT_VAR} "${_BW_EXE_PATH}" PARENT_SCOPE)
  else()
    message(WARNING "build-wrapper executable not found after extraction: ${_BW_EXE_PATH}")
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()

if(ENABLE_SONAR)
  _detect_or_install_build_wrapper(SONAR_BUILD_WRAPPER_EXE)
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

    message(STATUS "Sonar build-wrapper not available (download failed). 'sonar_bw' will print a helpful error.")
    add_custom_target(sonar_bw
      COMMENT "ERROR: Sonar build-wrapper missing — set SONAR_BUILD_WRAPPER to skip auto-download and use a custom path"
      COMMAND ${CMAKE_COMMAND} -E echo "[sonar_bw] ERROR: Sonar build-wrapper not found."
      COMMAND ${CMAKE_COMMAND} -E false
      VERBATIM
    )
  endif()
endif()

# --- Sonar scanner ---------------------------------------------------------
# Runs sonar-scanner using sonar-project.properties at repo root.
# Depends on coverage and sonar_bw so the active build directory always
# provides the compile database and SonarQube coverage report.
function(_detect_or_install_scanner OUT_VAR)
  # 0) Prefer an installed scanner already on PATH
  find_program(_SC_EXE NAMES sonar-scanner sonar-scanner.bat)
  if(_SC_EXE)
    set(${OUT_VAR} "${_SC_EXE}" PARENT_SCOPE)
    return()
  endif()

  # 1) Candidate URLs as provided (fixed version 7.2.0.5079)
  set(_URL_LINUX_AARCH64  "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.2.0.5079-linux-aarch64.zip")
  set(_URL_LINUX_X64      "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.2.0.5079-linux-x64.zip")
  set(_URL_MAC_AARCH64    "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.2.0.5079-macosx-aarch64.zip")
  set(_URL_MAC_X64        "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.2.0.5079-macosx-x64.zip")
  set(_URL_WIN_X64        "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.2.0.5079-windows-x64.zip")

  # 2) Select best candidates by platform/arch, with sensible fallbacks
  set(_SC_URLS)
  if(WIN32)
    list(APPEND _SC_URLS ${_URL_WIN_X64})
  elseif(APPLE)
    # Detect Apple Silicon vs Intel
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
      list(APPEND _SC_URLS ${_URL_MAC_AARCH64} ${_URL_MAC_X64})
    else()
      list(APPEND _SC_URLS ${_URL_MAC_X64} ${_URL_MAC_AARCH64})
    endif()
  else() # Linux/Unix
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
      list(APPEND _SC_URLS ${_URL_LINUX_AARCH64} ${_URL_LINUX_X64})
    else()
      list(APPEND _SC_URLS ${_URL_LINUX_X64} ${_URL_LINUX_AARCH64})
    endif()
  endif()

  # 3) Download first working archive
  set(SONAR_TOOLS_DIR ${CMAKE_BINARY_DIR}/tools)
  file(MAKE_DIRECTORY "${SONAR_TOOLS_DIR}")
  set(_SC_ZIP_FILE "${SONAR_TOOLS_DIR}/sonar-scanner.zip")
  set(_DOWNLOADED_ZIP "")
  foreach(_URL IN LISTS _SC_URLS)
    message(STATUS "Attempting to download sonar-scanner from: ${_URL}")
    file(DOWNLOAD "${_URL}" "${_SC_ZIP_FILE}" STATUS _sc_dl TLS_VERIFY ON)
    list(GET _sc_dl 0 _sc_code)
    if(_sc_code EQUAL 0)
      set(_DOWNLOADED_ZIP "${_SC_ZIP_FILE}")
      set(_SC_URL_USED "${_URL}")
      break()
    endif()
  endforeach()

  if(NOT _DOWNLOADED_ZIP)
    message(WARNING "Failed to download sonar-scanner for this platform. Install sonar-scanner in PATH or set SONAR_SCANNER_EXECUTABLE manually.")
    set(${OUT_VAR} "" PARENT_SCOPE)
    return()
  endif()

  # 4) Extract the archive to tools dir
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${_DOWNLOADED_ZIP}"
                  WORKING_DIRECTORY "${SONAR_TOOLS_DIR}"
                  RESULT_VARIABLE _sc_unzip)
  if(NOT _sc_unzip EQUAL 0)
    message(WARNING "Failed to extract sonar-scanner archive at ${_DOWNLOADED_ZIP}")
    set(${OUT_VAR} "" PARENT_SCOPE)
    return()
  endif()

  # 5) Locate the scanner binary (handles both *nix and Windows layouts)
  file(GLOB_RECURSE _SC_BIN_CAND
       "${SONAR_TOOLS_DIR}/sonar-scanner*/bin/sonar-scanner"
       "${SONAR_TOOLS_DIR}/sonar-scanner*/bin/sonar-scanner.bat")
  if(_SC_BIN_CAND)
    list(GET _SC_BIN_CAND 0 _SC_EXE_FOUND)
    message(STATUS "Installed sonar-scanner locally from ${_SC_URL_USED}: ${_SC_EXE_FOUND}")
    set(${OUT_VAR} "${_SC_EXE_FOUND}" PARENT_SCOPE)
  else()
    message(WARNING "sonar-scanner executable not found after extraction in ${SONAR_TOOLS_DIR}")
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()

set(_scan_deps coverage)
if(TARGET sonar_bw)
  list(APPEND _scan_deps sonar_bw)
endif()

_detect_or_install_scanner(SONAR_SCANNER_EXECUTABLE)
if(SONAR_SCANNER_EXECUTABLE)
  add_custom_target(sonar_scan
    COMMENT "Run sonar build-wrapper, generate coverage, then run sonar-scanner"
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sonar_bw
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage
    COMMAND ${SONAR_SCANNER_EXECUTABLE}
            --define sonar.projectBaseDir=${CMAKE_SOURCE_DIR}
            --define sonar.cfamily.compile-commands=${SONAR_BW_OUT}/compile_commands.json
            --define sonar.coverageReportPaths=${CMAKE_BINARY_DIR}/sonarqube-coverage.xml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
  )
else()
  message(STATUS "sonar-scanner not available (download failed). 'sonar_scan' will print a helpful error.")
  add_custom_target(sonar_scan
    COMMENT "ERROR: sonar-scanner missing — install it or set PATH"
    COMMAND ${CMAKE_COMMAND} -E echo "[sonar_scan] ERROR: sonar-scanner not found."
    COMMAND ${CMAKE_COMMAND} -E false
    VERBATIM
  )
endif()
