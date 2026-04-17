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
#include <cstdlib>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace workload = ::xyzzy::scopetimer::example_workload;
using TelemetryEvent = workload::TelemetryEvent;
using TelemetryTotals = workload::TelemetryTotals;

enum class BenchmarkScenario {
    HotPathBench,
};

enum class BenchSinkMode {
    Default,
    Buffered,
    Async,
    Null,
};

enum class BenchTimerMode {
    Default,
    HotPath,
};

struct BenchmarkOptions {
    int iterations{1};
    BenchmarkScenario scenario{BenchmarkScenario::HotPathBench};
};

static std::atomic<std::uint64_t>& hotPathSink() {
    static std::atomic<std::uint64_t> sink{0};
    return sink;
}

static inline void ingestTelemetryRecord(
    const TelemetryEvent& event,
    TelemetryTotals& totals,
    std::uint64_t salt
) {
    SCOPE_TIMER("hotPath:record");
    workload::ingestTelemetryRecordBody(event, totals, salt);
}

static inline void ingestTelemetryRecordHotPath(
    const TelemetryEvent& event,
    TelemetryTotals& totals,
    std::uint64_t salt
) {
    SCOPE_TIMER_HOT_PATH("hotPath:record");
    workload::ingestTelemetryRecordBody(event, totals, salt);
}

static int positiveEnvOrDefault(const char* envName, int defaultValue) {
    if (const char* env = std::getenv(envName)) {
        try {
            return std::max(1, std::stoi(env));
        } catch (const std::invalid_argument&) {
            return defaultValue;
        } catch (const std::out_of_range&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

static std::size_t positiveSizeEnvOrDefault(const char* envName, std::size_t defaultValue) {
    if (const char* env = std::getenv(envName)) {
        try {
            return std::max<std::size_t>(1U, static_cast<std::size_t>(std::stoull(env)));
        } catch (const std::invalid_argument&) {
            return defaultValue;
        } catch (const std::out_of_range&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

static BenchSinkMode benchSinkMode() {
    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_SINK")) {
        const std::string value(env);
        if (value == "BUFFERED" || value == "buffered") {
            return BenchSinkMode::Buffered;
        }
        if (value == "ASYNC" || value == "async") {
            return BenchSinkMode::Async;
        }
        if (value == "NULL" || value == "null" || value == "NOOP" || value == "noop") {
            return BenchSinkMode::Null;
        }
    }
    return BenchSinkMode::Default;
}

static BenchTimerMode benchTimerMode() {
    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_TIMER")) {
        const std::string value(env);
        if (value == "HOTPATH" || value == "hotpath" || value == "FAST" || value == "fast") {
            return BenchTimerMode::HotPath;
        }
    }
    return BenchTimerMode::Default;
}

class BenchSinkScope {
public:
    class NullLogSink final : public ::xyzzy::scopetimer::ScopeTimer::LogSink {
    public:
        void write(const char*, std::size_t) noexcept override {
            // Intentionally drop benchmark output so null-sink runs measure framework cost only.
        }
        void flush() noexcept override {
            // Intentionally empty: the null sink never buffers data.
        }
    };

    BenchSinkScope() {
        const std::size_t sinkBytes = positiveSizeEnvOrDefault("SCOPE_TIMER_BENCH_SINK_BYTES", 256U * 1024U);
        switch (benchSinkMode()) {
            case BenchSinkMode::Buffered:
                SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(sinkBytes);
                buffered_ = true;
                break;
            case BenchSinkMode::Async:
                SCOPE_TIMER_ENABLE_ASYNC_SINK(sinkBytes);
                async_ = true;
                break;
            case BenchSinkMode::Null:
                ::xyzzy::scopetimer::ScopeTimer::setLogSink(nullSink_);
                null_ = true;
                break;
            case BenchSinkMode::Default:
                break;
        }
    }

    ~BenchSinkScope() {
        if (buffered_) {
            SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK();
        }
        if (async_) {
            SCOPE_TIMER_DISABLE_ASYNC_SINK();
        }
        if (null_) {
            ::xyzzy::scopetimer::ScopeTimer::resetLogSink();
        }
    }

    BenchSinkScope(const BenchSinkScope&) = delete;
    BenchSinkScope& operator=(const BenchSinkScope&) = delete;

private:
    NullLogSink nullSink_{};
    bool buffered_{false};
    bool async_{false};
    bool null_{false};
};

static void hotPathBenchmarkWorker(int rounds, BenchTimerMode timerMode) {
    const std::size_t batchSize = 256U;
    const auto batch = workload::makeTelemetryBatch(batchSize);
    TelemetryTotals totals{};

    for (int round = 0; round < rounds; ++round) {
        for (const auto& event : batch) {
            const std::uint64_t salt = static_cast<std::uint64_t>(round) + totals.checksum;
            if (timerMode == BenchTimerMode::HotPath) {
                ingestTelemetryRecordHotPath(event, totals, salt);
            } else {
                ingestTelemetryRecord(event, totals, salt);
            }
        }
        totals.checksum ^= static_cast<std::uint64_t>(round) * 0x9e3779b97f4a7c15ULL;
    }

    hotPathSink().fetch_xor(totals.checksum + totals.retries + totals.routeBytes[0]);
}

static void hotPathBenchmark(int iterations) {
    const int rounds = std::max(1, iterations) * 12;
    const int threadCount = positiveEnvOrDefault("SCOPE_TIMER_BENCH_THREADS", 1);
    const BenchTimerMode timerMode = benchTimerMode();

    SCOPE_TIMER("hotPath:benchmark");
    if (threadCount == 1) {
        hotPathBenchmarkWorker(rounds, timerMode);
        return;
    }

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    for (int i = 0; i < threadCount; ++i) {
        workers.emplace_back([rounds, timerMode] {
            hotPathBenchmarkWorker(rounds, timerMode);
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
}

static BenchmarkOptions parseOptions(int argc, char** argv) {
    SCOPE_TIMER("Benchmark::parseOptions");

    BenchmarkOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: Benchmark [--iterations=N] [--scenario=hotpath-bench]\n"
                         "The dedicated benchmark executable drives a CPU-bound ScopeTimer\n"
                         "stress workload used by the benchmark scripts and CMake targets.\n"
                         "Benchmark env knobs: SCOPE_TIMER_BENCH_SINK=BUFFERED|ASYNC|NULL,\n"
                         "SCOPE_TIMER_BENCH_SINK_BYTES=<bytes>, SCOPE_TIMER_BENCH_THREADS=<n>,\n"
                         "and SCOPE_TIMER_BENCH_TIMER=HOTPATH.\n";
            std::exit(0);
        } else if (arg.rfind("--iterations=", 0) == 0) {
            options.iterations = std::max(1, std::stoi(arg.substr(13)));
        } else if (arg.rfind("--scenario=", 0) == 0) {
            if (const std::string value = arg.substr(11); value != "hotpath-bench") {
                std::cerr << "Unknown benchmark scenario: " << value << '\n';
                std::exit(2);
            }
            options.scenario = BenchmarkScenario::HotPathBench;
        } else {
            options.iterations = std::max(1, std::stoi(arg));
        }
    }
    return options;
}

int main(int argc, char** argv) {
    BenchSinkScope sinkScope;
    SCOPE_TIMER("Benchmark::main");

    const BenchmarkOptions options = parseOptions(argc, argv);

    // Preserve the existing benchmark scaling behavior so historical results
    // remain comparable when the dedicated executable replaces the old
    // benchmark-only path inside Demo.cpp.
    for (int i = 0; i < options.iterations; ++i) {
        if (options.scenario == BenchmarkScenario::HotPathBench) {
            hotPathBenchmark(options.iterations);
        }
    }
    return 0;
}
