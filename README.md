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
- **One-time-selected formatter** for elapsed units via `SCOPE_TIMER_FORMAT`.
  No per-call branching.
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
    ::ewm::scopetimer::ScopeTimer lifetimeTimer_;
};
```

## Building the test harness ##

```bash
> cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=17 -DENABLE_COVERAGE=ON && cmake --build build -j && cmake --build build --target coverage
-- Building example: Demo (example/Demo.cpp)
-- Unit tests enabled (test/ScopeTimerTest.cpp)
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/sclarke/github/scopetimer/build
[100%] Built target Demo
[100%] Built target scopetimer_tests
[100%] Running tests and generating code coverage report with gcovr
Test project /Users/sclarke/github/scopetimer/build
    Start 1: run_scopetimer_tests
1/1 Test #1: run_scopetimer_tests .............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
(INFO) Reading coverage data...
(INFO) Writing coverage report...
lines: 0.0% (0 out of 0)
functions: 0.0% (0 out of 0)
branches: 0.0% (0 out of 0)
[100%] Built target coverage
```

## Testing with default elapsed time formatting ##

```bash
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. build/ScopeTimerTest ; cat ./ScopeTimer.log
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
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS build/ScopeTimerTest ; cat ./ScopeTimer.log
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
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS build/ScopeTimerTest ; cat ./ScopeTimer.log
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
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS build/ScopeTimerTest ; cat ./ScopeTimer.log
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
> rm ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS build/ScopeTimerTest ; cat ./ScopeTimer.log
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
