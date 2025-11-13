# ScopeTimer #

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=bugs)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=coverage)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=sqale_index)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=stephenlclarke_ScopeTimer&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=stephenlclarke_ScopeTimer)
![Repo Visitors](https://visitor-badge.laobi.icu/badge?page_id=stephenlclarke.ScopeTimer)

A tiny, zero-intrusion C++17 **RAII scope timer** for high-performance code
paths. Drop a single macro into a scope and get a structured log line on exit
with start, end, and elapsed time. In release builds it compiles to a no-op; in
debug builds it's lock-light and allocation-free on the hot path.

The concept for this class & macro originated from a software project I worked
on in the early 1990's while at Merril Lynch. It owed a lot to James O.
Coplien's Advanced C++: Programming Styles and Idioms (Addison-Wesley, first
ed. 1991). A very smart guy that I had the pleaseure to work with when I was
at Bloomberg in 2016. This version is a re-imagining of the original.
Rewritten from scratch for C++17/20.

---

<p align="center">
  <a href="https://buy.stripe.com/8x23cvaHjaXzdg30Ni77O00">
    <img src="https://img.shields.io/badge/Buy%20Me%20a%20Coffee-❤️-brightgreen?style=for-the-badge&logo=buymeacoffee&logoColor=white" alt="Buy Me a Coffee">
  </a>
  &nbsp;
  <a href="https://github.com/stephenlclarke/scopetimer/discussions">
    <img src="https://img.shields.io/badge/Leave%20a%20Comment-💬-blue?style=for-the-badge" alt="Leave a Comment">
  </a>
</p>

<p align="center">
  <sub>☕ If you found this project useful, consider buying me a coffee or dropping a comment — it keeps the caffeine and ideas flowing! 😄</sub>
</p>

---

## What it's for ##

- **Fast, frictionless profiling** of functions, blocks, and object lifetimes.
- **Production-safe toggling**: enabled in Debug, compiled out in Release (via
  `NDEBUG`).
- **Consistent, parseable output** to a rolling log file for ad-hoc analysis.

### Design rationale ###

- **Zero friction**: a macro expands to a short-lived RAII object; no manual
  start/stop.
- **Hot-path friendly**: elapsed-unit selection is a **function pointer chosen
  once** from `SCOPE_TIMER_FORMAT`; per-use cost is a single indirect call +
  `snprintf`.
- **Predictable output**: timestamped start/end + elapsed, suitable for
  grepping or ingestion.
- **Safe by default**: thread-safe, buffered file I/O with periodic flush to
  limit syscalls.
- **Portable**: uses `localtime_s` (Windows) or `localtime_r` (POSIX).
- **Release-friendly**: expands to no-ops under `NDEBUG` so you can leave calls
  in code.

## Key features ##

- **RAII timing**: starts on construction, logs on destruction.
- **Unique per-use macro**: safe to place multiple timers in the same scope.
- **Thread-safe logging** with buffered I/O and periodic flush.
- **One-time-selected formatter** (functor) for elapsed units via
  `SCOPE_TIMER_FORMAT`. No per-call branching.
- **Portable time formatting** (uses `localtime_s`/`localtime_r`).

### Macro uniqueness: `__COUNTER__` and `__LINE__` ###

`SCOPE_TIMER(...)` expands to a short-lived stack variable whose name must be
unique per expansion. I generate that name with a two-stage token-pasting
helper:

```cpp
#define ST_CAT2(a,b) a##b
#define ST_CAT(a,b)  ST_CAT2(a,b)
```

Then I suffix the variable with **`__COUNTER__` when available**, which is a
translation-unit-global monotonically increasing integer, ensuring **global
uniqueness** even if multiple timers are emitted on the **same source line**
(e.g., from another macro). If a compiler doesn't support `__COUNTER__`,
I **fall back to `__LINE__`**, which is unique per line **but can collide**
if multiple expansions end up on the same line. The two-stage concat ensures
the macro arguments are expanded **before** token pasting.

### Environment variables ###

- `SCOPE_TIMER` - Controls whether timing/logging is enabled in **Debug**
  builds. Set to `"OFF"`, `"FALSE"`, `"NO"`, or `"0"` (case-insensitive;
  surrounding whitespace ignored) to **disable** logging entirely. Any other
  value, or if unset, leaves logging **enabled**. In **Release** builds
  (`NDEBUG` defined) this variable has no effect because `SCOPE_TIMER` calls
  compile to no-ops.
- `SCOPE_TIMER_DIR` - Directory for `ScopeTimer.log` (default `/tmp`).
- `SCOPE_TIMER_FLUSH_N` - Flush every N lines (default 256, max 1,000,000).
- `SCOPE_TIMER_FORMAT` - Elapsed units: `SECONDS`, `MILLIS`, `MICROS`, or
  `NANOS` (case-insensitive). If unset/invalid, auto-selects a readable unit.

## Quick start ##

```cpp
#include "ScopeTimer.hpp"

void foo() {
    SCOPE_TIMER("foo");        // Logs when foo() exits
    // ... work ...
}
```

### Conditional timing ###

```cpp
void bar(bool enabled) {
    SCOPE_TIMER_IF(enabled, "bar"); // Only logs when enabled == true
    // ... work ...
}
```

### Multiple timers in one scope (safe) ###

```cpp
void baz() {
    SCOPE_TIMER("baz:first");
    // ... work A ...
    SCOPE_TIMER("baz:second");
    // ... work B ...
}
```

### Nested scopes ###

```cpp
void nested() {
    SCOPE_TIMER("nested:outer");
    {
        SCOPE_TIMER("nested:inner");
        // ... inner work ...
    }
}
```

### Track an object's lifetime (member timer) ###

```cpp
class LifetimeTracked {
public:
    LifetimeTracked() : lifetimeTimer_("LifetimeTracked") { /* setup */ }
    ~LifetimeTracked() { /* teardown */ }
private:
    ::xyzzy::scopetimer::ScopeTimer lifetimeTimer_;
};
```

## Building the test harness ##

```bash
> rm -rf build && \
  cmake -S . -B build && \
  cmake --build build -j && \
  cmake --build build --target sonar_bw -j && \
  cmake --build build --target coverage && \
  sonar-scanner
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
-- Installed build-wrapper locally: /Users/sclarke/github/ScopeTimer/build/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Using Sonar build-wrapper: /Users/sclarke/github/ScopeTimer/build/tools/build-wrapper-macosx-x86/build-wrapper-macosx-x86
-- Configuring done (2.4s)
-- Generating done (0.1s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer/build
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 50%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 75%] Linking CXX executable Demo
[ 75%] Built target Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
[100%] Capture clean build with Sonar build-wrapper -> /Users/sclarke/github/ScopeTimer/build/bw-output
[ 25%] Building CXX object CMakeFiles/Demo.dir/example/Demo.cpp.o
[ 50%] Building CXX object CMakeFiles/scopetimer_tests.dir/test/ScopeTimerTest.cpp.o
[ 75%] Linking CXX executable Demo
[100%] Linking CXX executable scopetimer_tests
[100%] Built target scopetimer_tests
[100%] Built target Demo
Built target sonar_bw
[ 40%] Built target scopetimer_tests
[ 80%] Built target Demo
[100%] Run tests once and generate gcovr reports (SonarQube XML + HTML)
Test project /Users/sclarke/github/ScopeTimer/build
    Start 1: run_demo
1/2 Test #1: run_demo .........................   Passed    0.34 sec
    Start 2: run_scopetimer_tests
2/2 Test #2: run_scopetimer_tests .............   Passed    0.61 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.96 sec
(INFO) Reading coverage data...
(INFO) Writing coverage report...
lines: 99.1% (447 out of 451)
functions: 100.0% (67 out of 67)
branches: 54.4% (307 out of 564)
[100%] Built target coverage
18:45:45.468 INFO  Scanner configuration file: /opt/homebrew/Cellar/sonar-scanner/7.2.0.5079/libexec/conf/sonar-scanner.properties
18:45:45.471 INFO  Project root configuration file: /Users/sclarke/github/ScopeTimer/sonar-project.properties
18:45:45.482 INFO  SonarScanner CLI 7.2.0.5079
18:45:45.483 INFO  Mac OS X 26.0.1 aarch64
18:45:47.260 INFO  Communicating with SonarQube Cloud
18:45:47.260 INFO  JRE provisioning: os[macos], arch[arm64]
18:45:48.433 INFO  Starting SonarScanner Engine...
18:45:48.433 INFO  Java 17.0.11 Eclipse Adoptium (64-bit)
18:45:50.352 INFO  Load global settings
18:45:50.889 INFO  Load global settings (done) | time=537ms
18:45:50.926 INFO  Server id: 1BD809FA-AWHW8ct9-T_TB3XqouNu
18:45:51.027 INFO  Loading required plugins
18:45:51.027 INFO  Load plugins index
18:45:51.145 INFO  Load plugins index (done) | time=118ms
18:45:51.145 INFO  Load/download plugins
18:45:51.181 INFO  Load/download plugins (done) | time=36ms
18:45:51.331 INFO  Loaded core extensions: a3s, architecture, sca
18:45:51.471 INFO  Process project properties
18:45:51.481 INFO  Project key: stephenlclarke_ScopeTimer
18:45:51.481 INFO  Base dir: /Users/sclarke/github/ScopeTimer
18:45:51.481 INFO  Working dir: /Users/sclarke/github/ScopeTimer/.scannerwork
18:45:51.489 INFO  Load project branches
18:45:51.757 INFO  Load project branches (done) | time=267ms
18:45:51.759 INFO  Load project settings for component key: 'stephenlclarke_ScopeTimer'
18:45:52.237 INFO  Load project settings for component key: 'stephenlclarke_ScopeTimer' (done) | time=478ms
18:45:52.616 INFO  Check ALM binding of project 'stephenlclarke_ScopeTimer'
18:45:52.824 INFO  Detected project binding: BOUND
18:45:52.824 INFO  Check ALM binding of project 'stephenlclarke_ScopeTimer' (done) | time=208ms
18:45:52.828 INFO  Load project pull requests
18:45:53.005 INFO  Load project pull requests (done) | time=176ms
18:45:53.009 INFO  Load branch configuration
18:45:53.010 INFO  Load branch configuration (done) | time=1ms
18:45:53.013 INFO  Load quality profiles
18:45:53.469 INFO  Load quality profiles (done) | time=456ms
18:45:53.481 INFO  Create analysis
18:45:53.726 INFO  Create analysis (done) | time=244ms
18:45:53.820 INFO  Load active rules
18:45:54.808 INFO  Load active rules (done) | time=988ms
18:45:54.940 INFO  Organization key: stephenlclarke
18:45:54.955 INFO  Preprocessing files...
18:45:55.050 INFO  4 languages detected in 16 preprocessed files (done) | time=94ms
18:45:55.050 INFO  253 files ignored because of inclusion/exclusion patterns
18:45:55.050 INFO  122 files ignored because of scm ignore settings
18:45:55.120 INFO  Loading plugins for detected languages
18:45:55.121 INFO  Load/download plugins
18:45:55.151 INFO  Load/download plugins (done) | time=31ms
18:45:55.230 INFO  Load metrics repository
18:45:55.341 INFO  Load metrics repository (done) | time=112ms
18:45:55.346 INFO  Sensor cache enabled
18:45:55.348 INFO  Load sensor cache
18:45:55.994 INFO  Load sensor cache (21 KB) | time=646ms
18:45:56.041 INFO  Load project repositories
18:45:56.224 INFO  Load project repositories (done) | time=183ms
18:45:56.227 INFO  Indexing files...
18:45:56.227 INFO  Project configuration:
18:45:56.227 INFO    Excluded sources: **/build/**, **/_deps/**, **/bw-output/**, **/compile_commands.json, **/build-wrapper-dump.json, **/test/**, **/*Test.cpp, **/*Tests.cpp
18:45:56.227 INFO    Included tests: **/test/**, **/*Test.cpp, **/*Tests.cpp
18:45:56.232 INFO  16 files indexed (done) | time=5ms
18:45:56.235 INFO  Quality profile for cpp: Sonar way
18:45:56.235 INFO  Quality profile for json: Sonar way
18:45:56.235 INFO  Quality profile for shell: Sonar way
18:45:56.235 INFO  Quality profile for yaml: Sonar way
18:45:56.236 INFO  ------------- Run sensors on module ScopeTimer
18:45:56.512 INFO  Sensor Declarative Rule Engine for Shell [dre]
18:45:56.560 WARN  Unsupported platform detected: macOs-aarch64, skipping analysis.
18:45:56.560 INFO  Sensor Declarative Rule Engine for Shell [dre] (done) | time=47ms
18:45:56.560 INFO  Sensor JaCoCo XML Report Importer [jacoco]
18:45:56.560 INFO  'sonar.coverage.jacoco.xmlReportPaths' is not defined. Using default locations: target/site/jacoco/jacoco.xml,target/site/jacoco-it/jacoco.xml,build/reports/jacoco/test/jacocoTestReport.xml
18:45:56.560 INFO  No report imported, no coverage information will be imported by JaCoCo XML Report Importer
18:45:56.561 INFO  Sensor JaCoCo XML Report Importer [jacoco] (done) | time=0ms
18:45:56.561 INFO  Sensor IaC GitHub Actions Sensor [iacenterprise]
18:45:56.568 INFO  1 source file to be analyzed
18:45:56.630 INFO  1/1 source file has been analyzed
18:45:56.630 INFO  Sensor IaC GitHub Actions Sensor [iacenterprise] (done) | time=69ms
18:45:56.630 INFO  Sensor IaC Shell Sensor [iacenterprise]
18:45:56.632 INFO  3 source files to be analyzed
18:45:56.642 INFO  3/3 source files have been analyzed
18:45:56.642 INFO  Sensor IaC Shell Sensor [iacenterprise] (done) | time=12ms
18:45:56.642 INFO  Sensor IaC CloudFormation Sensor [iac]
18:45:56.644 INFO  There are no files to be analyzed for the CloudFormation language
18:45:56.644 INFO  Sensor IaC CloudFormation Sensor [iac] (done) | time=2ms
18:45:56.644 INFO  Sensor IaC cfn-lint report Sensor [iac]
18:45:56.645 INFO  Sensor IaC cfn-lint report Sensor [iac] (done) | time=0ms
18:45:56.645 INFO  Sensor IaC Docker Sensor [iac]
18:45:56.645 INFO  There are no files to be analyzed for the Docker language
18:45:56.645 INFO  Sensor IaC Docker Sensor [iac] (done) | time=0ms
18:45:56.645 INFO  Sensor IaC hadolint report Sensor [iac]
18:45:56.645 INFO  Sensor IaC hadolint report Sensor [iac] (done) | time=0ms
18:45:56.645 INFO  Sensor IaC Azure Resource Manager Sensor [iac]
18:45:56.645 INFO  There are no files to be analyzed for the Azure Resource Manager language
18:45:56.646 INFO  Sensor IaC Azure Resource Manager Sensor [iac] (done) | time=0ms
18:45:56.646 INFO  Sensor Java Config Sensor [iac]
18:45:56.647 INFO  There are no files to be analyzed for the Java language
18:45:56.647 INFO  Sensor Java Config Sensor [iac] (done) | time=2ms
18:45:56.647 INFO  Sensor CSS Rules [javascript]
18:45:56.647 INFO  No CSS, PHP, HTML or VueJS files are found in the project. CSS analysis is skipped.
18:45:56.647 INFO  Sensor CSS Rules [javascript] (done) | time=0ms
18:45:56.647 INFO  Sensor IaC JSON Sensor [iacenterprise]
18:45:56.647 INFO  Sensor for language "JSON" is disabled by a feature flag. You can enable it by setting "sonar.featureflag.cloud-security-enable-generic-yaml-and-json-analyzer" to true.
18:45:56.647 INFO  Sensor IaC JSON Sensor [iacenterprise] (done) | time=0ms
18:45:56.647 INFO  Sensor Generic Coverage Report
18:45:56.647 INFO  Parsing /Users/sclarke/github/ScopeTimer/build/sonarqube-coverage.xml
18:45:56.683 INFO  Imported coverage data for 3 files
18:45:56.683 INFO  Sensor Generic Coverage Report (done) | time=35ms
18:45:56.683 INFO  Sensor EnterpriseTextAndSecretsSensor [textenterprise]
18:45:56.693 INFO  Available processors: 12
18:45:56.693 INFO  Using 12 threads for analysis.
18:45:56.889 INFO  Start fetching files for the text and secrets analysis
18:45:56.898 INFO  Using Git CLI to retrieve untracked files
18:45:56.913 INFO  Retrieving language associated files and files included via "sonar.text.inclusions" that are tracked by git
18:45:56.914 INFO  Starting the text and secrets analysis
18:45:56.915 INFO  10 source files to be analyzed for the text and secrets analysis
18:45:56.948 INFO  10/10 source files have been analyzed for the text and secrets analysis
18:45:56.950 INFO  Start fetching files for the binary file analysis
18:45:56.950 INFO  There are no files to be analyzed for the binary file analysis
18:45:56.950 INFO  2 files are ignored because they are untracked by git
18:45:56.951 INFO  Sensor EnterpriseTextAndSecretsSensor [textenterprise] (done) | time=268ms
18:45:56.951 INFO  Sensor CFamily [cpp]
18:45:56.971 INFO  CFamily plugin version: 6.74.0.91793 (b828a9364d6778e337590775598d969f96b9514c)
18:45:56.972 INFO  CFamily analysis configuration mode: Compile-Commands
18:45:56.972 INFO  Incremental symbolic execution is enabled
18:45:56.972 INFO  Use incremental symbolic execution's path length implementation
18:45:56.974 INFO  Using compile commands: /Users/sclarke/github/ScopeTimer/build/bw-output/compile_commands.json
18:45:56.974 INFO  Available processors: 12
18:45:56.975 INFO  Using 12 threads for analysis.
18:45:58.606 INFO  Loading cache from: server
18:45:59.817 INFO  Detected compilers: g++-15=2
18:45:59.818 INFO  Number of compilation units in the provided Compilation Database: 2
18:45:59.818 INFO  Number of skipped unsupported compilation units: 0
18:45:59.818 INFO  Number of skipped non-indexed compilation units: 1
18:45:59.818 INFO  Number of skipped duplicate compilation units: 0
18:45:59.819 INFO  Number of remaining compilation units to be analyzed: 1
18:45:59.819 INFO  Detected standards: c++17=1
18:45:59.820 INFO  Starting the analysis of 1 CFamily compilation units
18:45:59.885 INFO  [1/1] Cache hit for: /Users/sclarke/github/ScopeTimer/example/Demo.cpp
18:45:59.902 INFO  2/2 files marked as unchanged
18:45:59.902 INFO  0/1 test files marked as unchanged
18:45:59.902 INFO  Analysis cache: 1/1 hits, 50553 bytes
18:45:59.902 INFO  Incremental symbolic execution cache: 0 hits out of 0 total analyzed targets (hit rate 0%)
18:45:59.902 INFO  CFamily analysis done in: 2931ms
18:45:59.902 INFO  1 compilation units were fully analyzed
18:45:59.902 INFO  0 compilation units were partially analyzed: 0 stopped on Config, 0 stopped on Parsing, 0 stopped on AST, 0 stopped on Symbolic Execution, and 0 stopped on Symbolic Execution with Parsing Error
18:45:59.902 INFO  54 rules with 'symbolic-execution' tag raised 1 out of 15 issues
18:45:59.904 INFO  Performance statistics per analysis stage (Total, Average, Percentage) across 1 compilation units:
annotationStage: 6ms, 6ms, 0.07%
astRulesStage: 28ms, 28ms, 0.32%
configStage: 8ms, 8ms, 0.09%
parsingStage: 556ms, 556ms, 6.33%
symbolicExecutionStage: 8184ms, 8184ms, 93.19%
18:45:59.904 INFO  PCH: unique=0 use=0 (forceInclude=0,throughHeader=0,firstInclude=0) out of 1 (forceInclude=0,throughHeader=0)
18:45:59.904 INFO  Z3 refutation rate: 0 out of 0
18:45:59.904 INFO  Percentage of files indexed with CFamily languages: 42.86% (C: 0, C++: 3, ObjC: 0, AnyLang: 7)
18:45:59.904 INFO  C and C++ analysis quality score: 100/100
18:45:59.905 INFO  Analysis measures statistics:
100% of classes were parsed successfully (0 out of 4 have parsing errors)
100% of functions were parsed successfully (0 out of 46 have parsing errors)
100% of statements were parsed successfully (0 out of 181 have parsing errors)
100% of the project includes directives were resolved (0 out of 16 were not resolved)
0 external includes directives were not resolved
0 modules imports were not resolved
0 header units were not resolved
18:45:59.907 INFO  Incremental symbolic execution internal implementation statistics (across analyzed compilation units):
Successful cache reads, writes: 0, 0 for 0 target compilation unit(s) (success rates 0%, 0%)
Unsupported cases in USR conversion decoding, encoding: 0, 0
Unsupported cases in issue-relocation decoding, encoding: 0, 0
Unsupported cases in abs. line no. decoding, encoding: 0, 0
Top-level declarations dropped decoding, encoding: 0, 0
Replayed path diagnostics: 0
Recorded path diagnostics: 0
Overwritten path diagnostic targets: 0
18:45:59.909 INFO  Sensor CFamily [cpp] (done) | time=2958ms
18:45:59.912 INFO  ------------- Run sensors on project
18:45:59.985 INFO  Sensor Zero Coverage Sensor
18:45:59.985 INFO  Sensor Zero Coverage Sensor (done) | time=0ms
18:46:00.092 INFO  ------------- Gather SCA dependencies on project
18:46:00.093 INFO  Checking if SCA is enabled for organization stephenlclarke
18:46:00.475 INFO  Dependency analysis skipped
18:46:00.479 INFO  CPD Executor Calculating CPD for 2 files
18:46:00.484 INFO  CPD Executor CPD calculation finished (done) | time=5ms
18:46:00.488 INFO  SCM Publisher SCM provider for this project is: git
18:46:00.489 INFO  SCM Publisher 2 source files to be analyzed
18:46:00.628 INFO  SCM Publisher 0/2 source files have been analyzed (done) | time=138ms
18:46:00.628 WARN  Missing blame information for the following files:
18:46:00.628 WARN    * include/ScopeTimer.hpp
18:46:00.628 WARN    * test/ScopeTimerTest.cpp
18:46:00.628 WARN  This may lead to missing/broken features in SonarCloud
18:46:00.702 INFO  Analysis report generated in 70ms, dir size=390 KB
18:46:00.721 INFO  Analysis report compressed in 19ms, zip size=89 KB
18:46:01.212 INFO  Analysis report uploaded in 491ms
18:46:01.215 INFO  ANALYSIS SUCCESSFUL, you can find the results at: https://sonarcloud.io/dashboard?id=stephenlclarke_ScopeTimer
18:46:01.216 INFO  Note that you will be able to access the updated dashboard once the server has processed the submitted analysis report
18:46:01.216 INFO  More about the report processing at https://sonarcloud.io/api/ce/task?id=AZp5Y5cSj55DiMS2CVYM
18:46:01.217 INFO  ------------- Upload SCA dependency files
18:46:01.681 INFO  Sensor cache published successfully
18:46:01.723 INFO  Analysis total time: 10.391 s
18:46:01.724 INFO  SonarScanner Engine completed successfully
18:46:02.120 INFO  EXECUTION SUCCESS
18:46:02.121 INFO  Total time: 16.654s
```

## Testing with default elapsed time formatting ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. build/scopetimer_tests ; cat ./ScopeTimer.log
[simpleWork] TID=001 | void simpleWork() | start=2025-08-14 12:21:01.165 | end=2025-08-14 12:21:01.168 | elapsed=3.136ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2025-08-14 12:21:01.172 | end=2025-08-14 12:21:01.173 | elapsed=1.883ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2025-08-14 12:21:01.173 | end=2025-08-14 12:21:01.177 | elapsed=3.136ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2025-08-14 12:21:01.170 | end=2025-08-14 12:21:01.177 | elapsed=6.944ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:01.178 | end=2025-08-14 12:21:01.179 | elapsed=1.013ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:01.177 | end=2025-08-14 12:21:01.179 | elapsed=1.907ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2025-08-14 12:21:01.181 | end=2025-08-14 12:21:01.181 | elapsed=250ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.182 | end=2025-08-14 12:21:01.183 | elapsed=388.500us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.183 | end=2025-08-14 12:21:01.183 | elapsed=389.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.183 | end=2025-08-14 12:21:01.183 | elapsed=387.833us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.183 | end=2025-08-14 12:21:01.184 | elapsed=386.875us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.184 | end=2025-08-14 12:21:01.184 | elapsed=388.667us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:01.182 | end=2025-08-14 12:21:01.184 | elapsed=1.981ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:01.184 | end=2025-08-14 12:21:01.185 | elapsed=649.375us
[threadedWork:worker] TID=003 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:01.184 | end=2025-08-14 12:21:01.185 | elapsed=892.416us
[threadedWork:worker] TID=004 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:01.184 | end=2025-08-14 12:21:01.185 | elapsed=1.144ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2025-08-14 12:21:01.184 | end=2025-08-14 12:21:01.186 | elapsed=1.330ms
[Worker:constructor] TID=001 | Worker::Worker() | start=2025-08-14 12:21:01.186 | end=2025-08-14 12:21:01.186 | elapsed=644.458us
[Worker:singleTask] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.186 | end=2025-08-14 12:21:01.187 | elapsed=1.266ms
[Worker:doMultipleTasks] TID=001 | void Worker::doMultipleTasks(int, bool) | start=2025-08-14 12:21:01.187 | end=2025-08-14 12:21:01.187 | elapsed=208ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.187 | end=2025-08-14 12:21:01.189 | elapsed=1.275ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.189 | end=2025-08-14 12:21:01.190 | elapsed=1.142ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.190 | end=2025-08-14 12:21:01.191 | elapsed=1.269ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.191 | end=2025-08-14 12:21:01.193 | elapsed=1.271ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:01.193 | end=2025-08-14 12:21:01.194 | elapsed=1.274ms
[ScopeTimer] TID=001 | LifetimeTracked | start=2025-08-14 12:21:01.194 | end=2025-08-14 12:21:01.197 | elapsed=3.204ms
[ScopeTimer] TID=001 | int main() | start=2025-08-14 12:21:01.165 | end=2025-08-14 12:21:01.197 | elapsed=32.136ms
```

## Testing with elapsed time formatted in seconds ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS build/scopetimer_tests ; cat ./ScopeTimer.log
[simpleWork] TID=001 | void simpleWork() | start=2025-08-14 12:21:29.231 | end=2025-08-14 12:21:29.234 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2025-08-14 12:21:29.237 | end=2025-08-14 12:21:29.239 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2025-08-14 12:21:29.239 | end=2025-08-14 12:21:29.242 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2025-08-14 12:21:29.235 | end=2025-08-14 12:21:29.242 | elapsed=0.006s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:29.243 | end=2025-08-14 12:21:29.244 | elapsed=0.001s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:29.242 | end=2025-08-14 12:21:29.244 | elapsed=0.001s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2025-08-14 12:21:29.246 | end=2025-08-14 12:21:29.246 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.247 | end=2025-08-14 12:21:29.248 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.248 | end=2025-08-14 12:21:29.248 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.248 | end=2025-08-14 12:21:29.248 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.248 | end=2025-08-14 12:21:29.249 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.249 | end=2025-08-14 12:21:29.249 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:29.247 | end=2025-08-14 12:21:29.249 | elapsed=0.001s
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:29.249 | end=2025-08-14 12:21:29.250 | elapsed=0.000s
[threadedWork:worker] TID=003 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:29.249 | end=2025-08-14 12:21:29.250 | elapsed=0.000s
[threadedWork:worker] TID=004 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:29.249 | end=2025-08-14 12:21:29.250 | elapsed=0.001s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2025-08-14 12:21:29.249 | end=2025-08-14 12:21:29.250 | elapsed=0.001s
[Worker:constructor] TID=001 | Worker::Worker() | start=2025-08-14 12:21:29.250 | end=2025-08-14 12:21:29.251 | elapsed=0.000s
[Worker:singleTask] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.251 | end=2025-08-14 12:21:29.252 | elapsed=0.001s
[Worker:doMultipleTasks] TID=001 | void Worker::doMultipleTasks(int, bool) | start=2025-08-14 12:21:29.252 | end=2025-08-14 12:21:29.252 | elapsed=0.000s
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.252 | end=2025-08-14 12:21:29.254 | elapsed=0.001s
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.254 | end=2025-08-14 12:21:29.255 | elapsed=0.001s
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.255 | end=2025-08-14 12:21:29.256 | elapsed=0.001s
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.256 | end=2025-08-14 12:21:29.257 | elapsed=0.001s
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:29.257 | end=2025-08-14 12:21:29.259 | elapsed=0.001s
[ScopeTimer] TID=001 | LifetimeTracked | start=2025-08-14 12:21:29.259 | end=2025-08-14 12:21:29.262 | elapsed=0.003s
[ScopeTimer] TID=001 | int main() | start=2025-08-14 12:21:29.231 | end=2025-08-14 12:21:29.262 | elapsed=0.030s
```

## Testing with elapsed time formatted in millis ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS build/scopetimer_tests ; cat ./ScopeTimer.log
[simpleWork] TID=001 | void simpleWork() | start=2025-08-14 12:21:43.024 | end=2025-08-14 12:21:43.027 | elapsed=3.134ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2025-08-14 12:21:43.030 | end=2025-08-14 12:21:43.032 | elapsed=1.757ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2025-08-14 12:21:43.032 | end=2025-08-14 12:21:43.035 | elapsed=3.154ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2025-08-14 12:21:43.029 | end=2025-08-14 12:21:43.035 | elapsed=6.816ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:43.036 | end=2025-08-14 12:21:43.037 | elapsed=1.004ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:43.036 | end=2025-08-14 12:21:43.037 | elapsed=1.893ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2025-08-14 12:21:43.039 | end=2025-08-14 12:21:43.039 | elapsed=0.000ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.040 | end=2025-08-14 12:21:43.041 | elapsed=0.379ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.041 | end=2025-08-14 12:21:43.041 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.041 | end=2025-08-14 12:21:43.041 | elapsed=0.305ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.041 | end=2025-08-14 12:21:43.042 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.042 | end=2025-08-14 12:21:43.042 | elapsed=0.385ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:43.040 | end=2025-08-14 12:21:43.042 | elapsed=1.853ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:43.042 | end=2025-08-14 12:21:43.043 | elapsed=0.632ms
[threadedWork:worker] TID=003 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:43.042 | end=2025-08-14 12:21:43.043 | elapsed=0.880ms
[threadedWork:worker] TID=004 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:43.042 | end=2025-08-14 12:21:43.043 | elapsed=1.075ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2025-08-14 12:21:43.042 | end=2025-08-14 12:21:43.043 | elapsed=1.145ms
[Worker:constructor] TID=001 | Worker::Worker() | start=2025-08-14 12:21:43.043 | end=2025-08-14 12:21:43.044 | elapsed=0.633ms
[Worker:singleTask] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.044 | end=2025-08-14 12:21:43.045 | elapsed=1.254ms
[Worker:doMultipleTasks] TID=001 | void Worker::doMultipleTasks(int, bool) | start=2025-08-14 12:21:43.045 | end=2025-08-14 12:21:43.045 | elapsed=0.000ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.045 | end=2025-08-14 12:21:43.046 | elapsed=1.255ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.046 | end=2025-08-14 12:21:43.048 | elapsed=1.253ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.048 | end=2025-08-14 12:21:43.049 | elapsed=1.259ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.049 | end=2025-08-14 12:21:43.050 | elapsed=1.260ms
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:43.050 | end=2025-08-14 12:21:43.051 | elapsed=1.255ms
[ScopeTimer] TID=001 | LifetimeTracked | start=2025-08-14 12:21:43.051 | end=2025-08-14 12:21:43.054 | elapsed=3.058ms
[ScopeTimer] TID=001 | int main() | start=2025-08-14 12:21:43.024 | end=2025-08-14 12:21:43.054 | elapsed=30.388ms
```

## Testing with elapsed time formatted in micros ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS build/scopetimer_tests ; cat ./ScopeTimer.log
[simpleWork] TID=001 | void simpleWork() | start=2025-08-14 12:21:55.035 | end=2025-08-14 12:21:55.038 | elapsed=3132.625us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2025-08-14 12:21:55.041 | end=2025-08-14 12:21:55.043 | elapsed=1910.417us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2025-08-14 12:21:55.043 | end=2025-08-14 12:21:55.046 | elapsed=3155.791us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2025-08-14 12:21:55.039 | end=2025-08-14 12:21:55.046 | elapsed=7025.208us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:55.047 | end=2025-08-14 12:21:55.048 | elapsed=1012.209us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:21:55.046 | end=2025-08-14 12:21:55.048 | elapsed=1904.875us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2025-08-14 12:21:55.050 | end=2025-08-14 12:21:55.050 | elapsed=0.083us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.051 | end=2025-08-14 12:21:55.052 | elapsed=380.583us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.052 | end=2025-08-14 12:21:55.052 | elapsed=379.583us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.052 | end=2025-08-14 12:21:55.052 | elapsed=378.709us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.052 | end=2025-08-14 12:21:55.053 | elapsed=380.375us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.053 | end=2025-08-14 12:21:55.053 | elapsed=381.292us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2025-08-14 12:21:55.051 | end=2025-08-14 12:21:55.053 | elapsed=1914.000us
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:55.053 | end=2025-08-14 12:21:55.054 | elapsed=638.292us
[threadedWork:worker] TID=004 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:55.053 | end=2025-08-14 12:21:55.054 | elapsed=887.125us
[threadedWork:worker] TID=003 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:21:55.053 | end=2025-08-14 12:21:55.054 | elapsed=1137.625us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2025-08-14 12:21:55.053 | end=2025-08-14 12:21:55.054 | elapsed=1223.791us
[Worker:constructor] TID=001 | Worker::Worker() | start=2025-08-14 12:21:55.054 | end=2025-08-14 12:21:55.055 | elapsed=641.333us
[Worker:singleTask] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.055 | end=2025-08-14 12:21:55.056 | elapsed=1261.958us
[Worker:doMultipleTasks] TID=001 | void Worker::doMultipleTasks(int, bool) | start=2025-08-14 12:21:55.056 | end=2025-08-14 12:21:55.056 | elapsed=0.083us
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.056 | end=2025-08-14 12:21:55.058 | elapsed=1184.417us
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.058 | end=2025-08-14 12:21:55.059 | elapsed=1261.750us
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.059 | end=2025-08-14 12:21:55.060 | elapsed=1256.500us
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.060 | end=2025-08-14 12:21:55.061 | elapsed=1258.250us
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:21:55.061 | end=2025-08-14 12:21:55.063 | elapsed=1266.375us
[ScopeTimer] TID=001 | LifetimeTracked | start=2025-08-14 12:21:55.063 | end=2025-08-14 12:21:55.066 | elapsed=3165.959us
[ScopeTimer] TID=001 | int main() | start=2025-08-14 12:21:55.035 | end=2025-08-14 12:21:55.066 | elapsed=30940.458us
```

## Testing with elapsed time formatted in nanos ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS build/scopetimer_tests ; cat ./ScopeTimer.log
[simpleWork] TID=001 | void simpleWork() | start=2025-08-14 12:22:07.888 | end=2025-08-14 12:22:07.891 | elapsed=2584708ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2025-08-14 12:22:07.893 | end=2025-08-14 12:22:07.895 | elapsed=1880417ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2025-08-14 12:22:07.895 | end=2025-08-14 12:22:07.898 | elapsed=3135500ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2025-08-14 12:22:07.892 | end=2025-08-14 12:22:07.899 | elapsed=6915667ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:22:07.900 | end=2025-08-14 12:22:07.901 | elapsed=900333ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2025-08-14 12:22:07.899 | end=2025-08-14 12:22:07.901 | elapsed=1792833ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2025-08-14 12:22:07.902 | end=2025-08-14 12:22:07.902 | elapsed=84ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.904 | end=2025-08-14 12:22:07.904 | elapsed=382042ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.904 | end=2025-08-14 12:22:07.904 | elapsed=385583ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.904 | end=2025-08-14 12:22:07.905 | elapsed=380250ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.905 | end=2025-08-14 12:22:07.905 | elapsed=382625ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.905 | end=2025-08-14 12:22:07.906 | elapsed=382083ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2025-08-14 12:22:07.904 | end=2025-08-14 12:22:07.906 | elapsed=1927041ns
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:22:07.906 | end=2025-08-14 12:22:07.906 | elapsed=632125ns
[threadedWork:worker] TID=003 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:22:07.906 | end=2025-08-14 12:22:07.907 | elapsed=880750ns
[threadedWork:worker] TID=004 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2025-08-14 12:22:07.906 | end=2025-08-14 12:22:07.907 | elapsed=1132542ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2025-08-14 12:22:07.906 | end=2025-08-14 12:22:07.907 | elapsed=1198416ns
[Worker:constructor] TID=001 | Worker::Worker() | start=2025-08-14 12:22:07.907 | end=2025-08-14 12:22:07.907 | elapsed=631834ns
[Worker:singleTask] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.907 | end=2025-08-14 12:22:07.909 | elapsed=1255708ns
[Worker:doMultipleTasks] TID=001 | void Worker::doMultipleTasks(int, bool) | start=2025-08-14 12:22:07.909 | end=2025-08-14 12:22:07.909 | elapsed=83ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.909 | end=2025-08-14 12:22:07.910 | elapsed=1259084ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.910 | end=2025-08-14 12:22:07.911 | elapsed=1263041ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.911 | end=2025-08-14 12:22:07.912 | elapsed=1261375ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.912 | end=2025-08-14 12:22:07.914 | elapsed=1259791ns
[Worker:task] TID=001 | void Worker::doTask(const std::string &) | start=2025-08-14 12:22:07.914 | end=2025-08-14 12:22:07.915 | elapsed=1256917ns
[ScopeTimer] TID=001 | LifetimeTracked | start=2025-08-14 12:22:07.915 | end=2025-08-14 12:22:07.918 | elapsed=3143709ns
[ScopeTimer] TID=001 | int main() | start=2025-08-14 12:22:07.888 | end=2025-08-14 12:22:07.918 | elapsed=29906500ns
```

## Summarise the logging ##

```bash
❯ scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[tests:child:probe]static int xyzzy::scopetimer::ScopeTimer_TestFriend::child_probe_main_if_requested()
  count=9  min=131us  avg=134us  max=139us  -

[tests:simple_scope]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_simple_scope()
  count=1  min=72us  avg=72us  max=72us  -

[tests:nested:inner1]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_nested_scopes()
  count=1  min=21us  avg=21us  max=21us  -

[tests:nested:inner2]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_nested_scopes()
  count=1  min=28us  avg=28us  max=28us  -

[tests:nested:outer]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_nested_scopes()
  count=1  min=74us  avg=74us  max=74us  -

[tests:looped:iter]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_looped_work()
  count=3  min=9us  avg=9us  max=9us  -

[tests:looped:total]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_looped_work()
  count=1  min=31us  avg=31us  max=31us  -

[tests:threaded:worker]xyzzy::scopetimer::ScopeTimer_TestFriend::test_threaded()::<lambda()>
  count=2  min=17us  avg=20us  max=23us  -

[tests:threaded:total]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_threaded()
  count=1  min=96us  avg=96us  max=96us  -

[tests:format:micros]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_variants()
  count=1  min=6us  avg=6us  max=6us  -

[tests:format:millis]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_variants()
  count=1  min=6us  avg=6us  max=6us  -

[tests:format:seconds]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_variants()
  count=1  min=5us  avg=5us  max=5us  -

[tests:format:nanos]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_variants()
  count=1  min=4us  avg=4us  max=4us  -

[tests:format:bogus]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_variants()
  count=1  min=7us  avg=7us  max=7us  -

[tests:format:unset]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_unset_and_empty()
  count=1  min=5us  avg=5us  max=5us  -

[tests:format:empty]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_env_format_unset_and_empty()
  count=1  min=4us  avg=4us  max=4us  -

[]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_empty_label()
  count=1  min=4us  avg=4us  max=4us  -

[tests:format:seconds:short]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_seconds_format_branch()
  count=1  min=1.386ms  avg=1.386ms  max=1.386ms  -

[tests:format:seconds:long]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_seconds_format_branch()
  count=1  min=12.508ms  avg=12.508ms  max=12.508ms  -

[tests:logfile_null_branch]static void xyzzy::scopetimer::ScopeTimer_TestFriend::test_logfile_null_branch()
  count=1  min=71us  avg=71us  max=71us  -
```

---

© 2025 Steve Clarke · https://xyzzy.tools · Released under the [AGPL-3.0 License](https://www.gnu.org/licenses/agpl-3.0.html)

---
