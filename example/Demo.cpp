/*
 * ScopeTimer - lightweight C++17 scope timing utility
 * Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * In accordance with section 13 of the AGPL, if you modify this program,
 * your modified version must prominently offer all users interacting with it
 * remotely through a computer network an opportunity to receive the source
 * code of your version.
 */

#include "ScopeTimer.hpp"
#include "TelemetryWorkload.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

namespace workload = ::xyzzy::scopetimer::example_workload;
using TelemetryEvent = workload::TelemetryEvent;
using TelemetryTotals = workload::TelemetryTotals;

struct DemoOptions {
    int iterations{1};
};

class DemoStdoutLogSink final : public ::xyzzy::scopetimer::ScopeTimer::LogSink {
public:
    void write(const char* data, std::size_t len) noexcept override {
        std::cout.write(data, static_cast<std::streamsize>(len));
    }

    void flush() noexcept override {
        std::cout.flush();
    }
};

static std::atomic<std::uint64_t>& hotPathSink() {
    static std::atomic<std::uint64_t> sink{0};
    return sink;
}

// Simulate some work for a given duration.
static void busyFor(std::chrono::microseconds us) {
    std::this_thread::sleep_for(us);
}

static std::uint64_t processTelemetryBatchExample(
    const std::vector<TelemetryEvent>& batch,
    TelemetryTotals& totals
) {
    SCOPE_TIMER("hotPath:processBatch");
    return workload::processTelemetryBatchBody(batch, totals);
}

// CPU-bound example: simulate a telemetry ingestion hot path.
static void hotPathIngestion(int intensity) {
    const std::size_t eventsPerBatch = 1024U + static_cast<std::size_t>(intensity * 256U);
    const int batches = 4 + intensity;
    auto batch = workload::makeTelemetryBatch(eventsPerBatch);
    TelemetryTotals totals{};

    SCOPE_TIMER("hotPath:total");
    for (int batchIndex = 0; batchIndex < batches; ++batchIndex) {
        totals.checksum ^= processTelemetryBatchExample(batch, totals) + static_cast<std::uint64_t>(batchIndex);

        const auto pivot = static_cast<std::size_t>((batchIndex * 131U) % batch.size());
        batch[pivot].statusCode = batch[pivot].statusCode == 200U ? 503U : 200U;
        batch[pivot].flags ^= 0x05U;
        batch[pivot].bytes += static_cast<std::uint32_t>(batchIndex & 0x07U);
    }

    hotPathSink().fetch_xor(totals.checksum + totals.retries + totals.routeBytes[0]);
}

// Example 1: Simple function timing.
static void simpleWork() {
    SCOPE_TIMER("simpleWork");
    busyFor(2500us);
}

// Example 2: Nested scopes.
static void nestedScopes() {
    SCOPE_TIMER("nestedScopes:outer");
    busyFor(1000us);
    {
        SCOPE_TIMER("nestedScopes:inner 1");
        busyFor(1500us);
    }
    {
        SCOPE_TIMER("nestedScopes:inner 2");
        busyFor(2500us);
    }
    busyFor(500us);
}

// Example 3: Multiple timers in the same scope.
static void multipleTimersSameScope() {
    SCOPE_TIMER("multi:first");
    busyFor(600us);
    SCOPE_TIMER("multi:second");
    busyFor(700us);
    SCOPE_TIMER("multi:third"); busyFor(800us); SCOPE_TIMER("multi:fourth");
    busyFor(900us);
}

// Example 4: Conditional timing.
static void conditionalWork(bool enabled) {
    SCOPE_TIMER_IF(enabled, "conditionalWork");
    busyFor(1200us);
}

// Example 5: Loop with per-iteration timing labels.
static void loopedWork(int iterations) {
    SCOPE_TIMER("loopedWork:total");
    for (int i = 0; i < iterations; ++i) {
        SCOPE_TIMER("loopedWork:iteration");
        busyFor(300us);
    }
}

// Example 6: Multithreaded timing.
static void threadedWork(int threads) {
    SCOPE_TIMER("threadedWork:total");
    std::vector<std::thread> tg;
    tg.reserve(threads);
    for (int i = 0; i < threads; ++i) {
        tg.emplace_back([i] {
            SCOPE_TIMER("threadedWork:worker");
            busyFor(std::chrono::microseconds{500 + (i * 200)});
        });
    }
    for (auto& t : tg) {
        t.join();
    }
}

// Example 7: Thread-buffered sink around a burst of small timers.
static void bufferedSinkExample() {
    SCOPE_TIMER("bufferedSinkExample");
    SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(4U * 1024U);
    for (int i = 0; i < 3; ++i) {
        SCOPE_TIMER("bufferedSinkExample:iteration");
        busyFor(150us);
    }
    SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK();
}

// Example 8: Async sink for caller-thread-sensitive logging.
static void asyncSinkExample() {
    SCOPE_TIMER("asyncSinkExample");
    SCOPE_TIMER_ENABLE_ASYNC_SINK(4U * 1024U);
    for (int i = 0; i < 3; ++i) {
        SCOPE_TIMER("asyncSinkExample:iteration");
        busyFor(120us);
    }
    SCOPE_TIMER_DISABLE_ASYNC_SINK();
}

// Example 9: Public plug-in sink API using a user-supplied logger class.
static void pluginSinkExample() {
    DemoStdoutLogSink sink;
    ::xyzzy::scopetimer::ScopeTimer::setLogSink(sink);
    {
        SCOPE_TIMER("pluginSinkExample");
        busyFor(900us);
    }
    ::xyzzy::scopetimer::ScopeTimer::resetLogSink();
}

// Example 10: Hot-path timer for a compact elapsed-only record.
static void hotPathMacroExample() {
    const auto batch = workload::makeTelemetryBatch(24U);
    TelemetryTotals totals{};

    SCOPE_TIMER("hotPathMacroExample:total");
    for (const auto& event : batch) {
        SCOPE_TIMER_HOT_PATH("hotPathMacroExample:record");
        workload::ingestTelemetryRecordBody(event, totals, totals.checksum);
    }

    hotPathSink().fetch_xor(totals.checksum + totals.retries + totals.routeBytes[0]);
}

// Example 11: Using SCOPE_TIMER in a class.
class Worker {
public:
    Worker() {
        SCOPE_TIMER("Worker:constructor");
        busyFor(500us);
    }

    Worker(const Worker&) = default;
    Worker& operator=(const Worker&) = default;
    Worker(Worker&&) noexcept = default;
    Worker& operator=(Worker&&) noexcept = default;
    ~Worker() = default;

    void doTask([[maybe_unused]] const std::string_view name) const {
        SCOPE_TIMER(name);
        busyFor(1000us);
    }

    void doMultipleTasks(int count, bool timed) const {
        SCOPE_TIMER_IF(timed, "Worker:doMultipleTasks");
        for (int i = 0; i < count; ++i) {
            doTask("Worker:task");
        }
    }
};

// Example 12: Track an object's lifetime with a member timer.
class LifetimeTracked {
public:
    LifetimeTracked() {
        busyFor(500us);
    }

    ~LifetimeTracked() {
        busyFor(500us);
    }

    LifetimeTracked(const LifetimeTracked&) = delete;
    LifetimeTracked& operator=(const LifetimeTracked&) = delete;
    LifetimeTracked(LifetimeTracked&&) noexcept = delete;
    LifetimeTracked& operator=(LifetimeTracked&&) noexcept = delete;

private:
    ::xyzzy::scopetimer::ScopeTimer lifetimeTimer_{"LifetimeTracked"};
};

static void runDemoSuite(int intensity) {
    SCOPE_TIMER("Demo::runDemoSuite");

    simpleWork();
    nestedScopes();
    multipleTimersSameScope();
    conditionalWork(false);
    conditionalWork(true);
    loopedWork(5 * intensity);
    threadedWork(std::clamp(intensity, 1, 8));
    bufferedSinkExample();
    asyncSinkExample();
    pluginSinkExample();
    hotPathMacroExample();
    hotPathIngestion(intensity);
    Worker w;
    for (int i = 0; i < intensity; ++i) {
        w.doTask("Worker:singleTask");
    }
    w.doMultipleTasks(3 * intensity, true);
    w.doMultipleTasks(2 * intensity, false);
    {
        LifetimeTracked obj;
        busyFor(1500us);
    }
}

static DemoOptions parseOptions(int argc, char** argv) {
    SCOPE_TIMER("Demo::parseOptions");

    DemoOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: Demo [--iterations=N]\n"
                         "The demo executable showcases ScopeTimer usage in educational\n"
                         "scenarios, including nested scopes, conditional timing,\n"
                         "threaded work, buffered logging, async logging, a plug-in\n"
                         "logger sink, and the compact hot-path timer.\n";
            std::exit(0);
        } else if (arg.rfind("--iterations=", 0) == 0) {
            options.iterations = std::max(1, std::stoi(arg.substr(13)));
        } else {
            options.iterations = std::max(1, std::stoi(arg));
        }
    }
    return options;
}

int main(int argc, char** argv) {
    SCOPE_TIMER("Demo::main");

    const DemoOptions options = parseOptions(argc, argv);
    for (int i = 0; i < options.iterations; ++i) {
        runDemoSuite(options.iterations);
    }
    return 0;
}
