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

Development note: this codebase has been developed primarily on macOS, with
support for Linux and Windows in the implementation. Linux is exercised in
GitHub Actions. Windows support has not been fully tested under Windows.

The concept for this class & macro originated from a software project I worked
on in the early 1990's while at Merril Lynch. It owed a lot to James O.
Coplien's Advanced C++: Programming Styles and Idioms (Addison-Wesley, first
ed. 1991). A very smart guy that I had the pleaseure to work with when I was
at Bloomberg in 2016. This version is a re-imagining of the original.
Rewritten from scratch for C++17/20.

---

<!-- markdownlint-disable MD033 MD013 -->
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
<!-- markdownlint-enable MD033 MD013 -->

---

## What it's for ##

- **Fast, frictionless profiling** of functions, blocks, and object lifetimes.
- **Production-safe toggling**: enabled in Debug, compiled out in Release (via
  `NDEBUG`).
- **Consistent, parseable output** to an append-only log file or custom sink
  for ad-hoc analysis.

### Design rationale ###

- **Zero friction**: a macro expands to a short-lived RAII object; no manual
  start/stop.
- **Hot-path friendly**: elapsed-unit selection is a **function pointer chosen
  once** from `SCOPE_TIMER_FORMAT`; per-use formatting uses fixed buffers and
  manual assembly rather than repeated environment parsing or `snprintf`.
- **Predictable output**: timestamped start/end + elapsed, suitable for
  grepping or ingestion.
- **Safe by default**: thread-safe direct file appends with periodic flush, plus
  optional buffered and async sink modes when you need lower caller-thread
  overhead.
- **Portable**: uses `localtime_s` (Windows) or `localtime_r` (POSIX).
- **Release-friendly**: expands to no-ops under `NDEBUG` so you can leave calls
  in code.

## Key features ##

- **RAII timing**: starts on construction, logs on destruction.
- **Unique per-use macro**: safe to place multiple timers in the same scope.
- **Thread-safe logging** with periodic flush plus optional thread-buffered and
  async sink modes.
- **One-time-selected formatter** (functor) for elapsed units via
  `SCOPE_TIMER_FORMAT`. No per-call branching.
- **Pluggable log sink** via a small public `ScopeTimer::LogSink` interface.
- **Portable time formatting** (uses `localtime_s`/`localtime_r`).

### How it compares ###

Minimal `ScopedTimer` examples from books such as *C++ High Performance*
usually show the core RAII idiom only: capture a start time in the
constructor and print elapsed milliseconds in the destructor. Boost's
`auto_cpu_timer` is a more polished library utility that automatically
reports wall, user, and system CPU time when a scope ends.

`ScopeTimer` is aimed at a broader use case. It keeps the same RAII model,
but adds macro-based insertion, Release-build no-ops, unique macro
expansion, optional labels, conditional and hot-path timers, cached
environment-driven configuration, thread IDs, formatted start/end
timestamps, multiple sink backends, and test hooks.

In short, this project is less a single timer helper and more a lightweight
instrumentation and logging subsystem built around the scope-timer idiom.

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
- `SCOPE_TIMER_FLUSH_N` - Flush every N lines (default 4096, max 1,000,000).
- `SCOPE_TIMER_FORMAT` - Elapsed units: `SECONDS`, `MILLIS`, `MICROS`, or
  `NANOS` (case-insensitive). If unset/invalid, auto-selects a readable unit.
- `SCOPE_TIMER_WALLTIME` - Set to `"OFF"`, `"FALSE"`, `"NO"`, or `"0"` to omit
  `start=` and `end=` timestamps from each log line and reduce timer overhead.

## Quick start ##

```cpp
#include "ScopeTimer.hpp"

void foo() {
    SCOPE_TIMER("foo");        // Logs when foo() exits
    // ... work ...
}
```

## Integrating into a new project ##

You do not need to copy this repo's full `CMakeLists.txt` into your own
project. The repo CMake drives the demo app, benchmark app, tests, coverage,
Sonar, and doc generation. For normal use, `ScopeTimer` is just a header-only
dependency.

### Simplest integration ###

1. Copy `include/ScopeTimer.hpp` into your project, or vendor this repo under
   something like `third_party/ScopeTimer`.
2. Add the header directory to your target's include path.
3. Compile your target as C++17 or newer.
4. Include `ScopeTimer.hpp` wherever you want to time a scope.
5. Run your app with `SCOPE_TIMER=1` in debug builds if you want logging
   explicitly enabled.

There is no library to link and no install step required for basic use.

### Minimal CMake example ###

If you vendor this repo at `third_party/ScopeTimer`, a consuming target can be
as simple as:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp LANGUAGES CXX)

add_executable(my_app
    src/main.cpp
)

target_compile_features(my_app PRIVATE cxx_std_17)
target_include_directories(my_app PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/ScopeTimer/include
)
```

Then in your code:

```cpp
#include "ScopeTimer.hpp"

int main() {
    SCOPE_TIMER("main");
    // app code
}
```

### Non-CMake example ###

```bash
g++ -std=c++17 -I./third_party/ScopeTimer/include src/main.cpp -o my_app
```

### Important build behavior ###

- In builds where `NDEBUG` is defined, `SCOPE_TIMER(...)` expands to a no-op.
  That means a typical Release build will compile the timers out completely.
- In builds where `NDEBUG` is not defined, the timers are active and can be
  controlled with environment variables such as `SCOPE_TIMER`,
  `SCOPE_TIMER_DIR`, `SCOPE_TIMER_FORMAT`, and `SCOPE_TIMER_WALLTIME`.
- If you want to try the lower-overhead sink modes in your own app, use the
  public macros directly:
  `SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(...)`,
  `SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK()`,
  `SCOPE_TIMER_ENABLE_ASYNC_SINK(...)`,
  `SCOPE_TIMER_DISABLE_ASYNC_SINK()`, `SCOPE_TIMER_HOT_PATH(...)`, and
  `ScopeTimer::setLogSink(...)` / `ScopeTimer::resetLogSink()`.

### Conditional timing ###

```cpp
void bar(bool enabled) {
    SCOPE_TIMER_IF(enabled, "bar"); // Only logs when enabled == true
    // ... work ...
}
```

### Thread-buffered logging ###

```cpp
void hotPath() {
    SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(64 * 1024);
    SCOPE_TIMER("hotPath");
    // ... work ...
    SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK();
}
```

Use the thread-buffered sink when mutex contention on the default logger shows up
in profiling. Buffered entries flush when the per-thread buffer reaches the
configured threshold, when you disable the buffered sink, when a worker thread
exits, and during process shutdown.
Threshold handoffs publish the completed batch immediately, but defer the
expensive final sink flush until an explicit flush or teardown point.
Enable or disable it around the setup or teardown of the code path you are
profiling, after any worker threads using it have quiesced. Switching sink mode
is synchronized, but it is still a configuration step rather than something to
toggle for every individual timer.

### Async logging ###

```cpp
void fanOut() {
    SCOPE_TIMER_ENABLE_ASYNC_SINK(64 * 1024);
    SCOPE_TIMER("fanOut");
    // ... work ...
    SCOPE_TIMER_DISABLE_ASYNC_SINK();
}
```

Use the async sink when the buffered sink still spends too much time flushing on
the caller thread. Async mode keeps the cheap thread-local buffering path, then
hands full buffers to a background writer thread. Larger handoff sizes such as
`64 * 1024` reduce queue churn when you care more about throughput than
tail-latency of the final write.

### Plug-in logger sink ###

```cpp
#include <iostream>
#include "ScopeTimer.hpp"

class CoutLogSink final : public ::xyzzy::scopetimer::ScopeTimer::LogSink {
public:
    void write(const char* data, std::size_t len) noexcept override {
        std::cout.write(data, static_cast<std::streamsize>(len));
    }

    void flush() noexcept override {
        std::cout.flush();
    }
};

void emitToStdout() {
    CoutLogSink sink;
    ::xyzzy::scopetimer::ScopeTimer::setLogSink(sink);
    SCOPE_TIMER("emitToStdout");
    // ... work ...
    ::xyzzy::scopetimer::ScopeTimer::resetLogSink();
}
```

Use a custom sink when you want ScopeTimer to write to an existing logging path
instead of the default logfile. The sink object must outlive the registration.
With a custom sink registered, direct timers write to it immediately, and the
built-in buffered and async modes use it as their final output target too.
A no-op implementation is also useful when you want to benchmark ScopeTimer's
own overhead without measuring output I/O.

### Hot-path timing ###

```cpp
void ingestRecord() {
    SCOPE_TIMER_HOT_PATH("ingestRecord");
    // ... very busy code ...
}
```

`SCOPE_TIMER_HOT_PATH` is the minimal-overhead option for extremely hot code.
It skips function signatures, thread ids, and wall-clock timestamps, and logs a
compact `elapsed=<n>ns` line for the supplied label.

### Using benchmark profiles in your app ###

The benchmark profiles in [`BENCHMARK.md`](BENCHMARK.md) are not separate
library modes that you enable by profile name. They are just shorthand for
combinations of the public macros and runtime settings shown below.

Do not use `SCOPE_TIMER_BENCH_*` environment variables in your own app. Those
exist only so `example/Benchmark.cpp` can switch benchmark profiles without
editing code.

1. `Standard timer, default sink`

   Use plain `SCOPE_TIMER(...)`.

   ```cpp
   void handleRequest() {
       SCOPE_TIMER("handleRequest");
       // work
   }
   ```

2. `Standard timer, wall time disabled`

   Keep `SCOPE_TIMER(...)` in code, but start the process with wall time
   formatting disabled.

   ```bash
   SCOPE_TIMER_WALLTIME=0 ./my_app
   ```

   ```cpp
   void handleRequest() {
       SCOPE_TIMER("handleRequest");
       // work
   }
   ```

3. `Standard timer, buffered sink`

   Enable the thread-buffered sink around the profiled phase, then keep using
   `SCOPE_TIMER(...)`.

   ```cpp
   int main() {
       SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(64 * 1024);
       runServer();
       SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK();
   }
   ```

4. `Standard timer, buffered sink (threaded stress)`

   This is the same public API as buffered sink. The benchmark name just means
   the workload is multi-threaded while buffered sink is enabled.

   ```cpp
   void workerLoop() {
       SCOPE_TIMER("workerLoop");
       // threaded work
   }
   ```

5. `Standard timer, async sink`

   Enable async sink around the profiled phase, then keep using
   `SCOPE_TIMER(...)`.

   ```cpp
   int main() {
       SCOPE_TIMER_ENABLE_ASYNC_SINK(64 * 1024);
       runServer();
       SCOPE_TIMER_DISABLE_ASYNC_SINK();
   }
   ```

6. `Standard timer, null sink`

   Register a no-op custom sink, then keep using `SCOPE_TIMER(...)`.

   ```cpp
   struct NullSink final : ::xyzzy::scopetimer::ScopeTimer::LogSink {
       void write(const char*, std::size_t) noexcept override {}
   };

   int main() {
       NullSink sink;
       ::xyzzy::scopetimer::ScopeTimer::setLogSink(sink);
       runServer();
       ::xyzzy::scopetimer::ScopeTimer::resetLogSink();
   }
   ```

7. `Hot-path timer, async sink`

   Enable async sink, but switch the hottest code to `SCOPE_TIMER_HOT_PATH(...)`
   instead of `SCOPE_TIMER(...)`.

   ```cpp
   int main() {
       SCOPE_TIMER_ENABLE_ASYNC_SINK(64 * 1024);
       runIngestion();
       SCOPE_TIMER_DISABLE_ASYNC_SINK();
   }

   void ingestRecord() {
       SCOPE_TIMER_HOT_PATH("ingestRecord");
       // very busy code
   }
   ```

8. `Hot-path timer, null sink`

   Combine the no-op sink with `SCOPE_TIMER_HOT_PATH(...)` to measure the
   framework floor without output I/O.

   ```cpp
   struct NullSink final : ::xyzzy::scopetimer::ScopeTimer::LogSink {
       void write(const char*, std::size_t) noexcept override {}
   };

   int main() {
       NullSink sink;
       ::xyzzy::scopetimer::ScopeTimer::setLogSink(sink);
       runIngestion();
       ::xyzzy::scopetimer::ScopeTimer::resetLogSink();
   }

   void ingestRecord() {
       SCOPE_TIMER_HOT_PATH("ingestRecord");
       // very busy code
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

## Build and verification ##

Build, coverage, Sonar, and benchmark-target usage now live in
[BUILD.md](BUILD.md).

Leak checking is platform-specific in this repo: `leak_check` uses
macOS `leaks` for local runs on the MacBook and Valgrind for Linux runs
in GitHub Actions. Benchmarks are kept local-only, so performance runs
should be done on the MacBook before pushing rather than in GitHub
Actions.

## Tests and log-format examples ##

Elapsed-format examples and the log-summary pipeline now live in
[TESTS.md](TESTS.md).

## Benchmarks ##

Current benchmark results, profile guidance, and reproducible per-profile
commands now live in [BENCHMARK.md](BENCHMARK.md).

---

<!-- markdownlint-disable MD034 -->
© 2025 Steve Clarke · https://xyzzy.tools · Released under the [AGPL-3.0 License](https://www.gnu.org/licenses/agpl-3.0.html)
<!-- markdownlint-enable MD034 -->

---
