/*
 * ScopeTimer - lightweight C++20 scope timing utility
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
#include "ExampleOptions.hpp"
#include "TelemetryWorkload.hpp"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace workload = ::xyzzy::scopetimer::example_workload;
namespace options = ::xyzzy::scopetimer::example_options;
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

struct BenchmarkRuntimeOptions {
    BenchSinkMode sinkMode{BenchSinkMode::Default};
    BenchTimerMode timerMode{BenchTimerMode::Default};
    int threadCount{1};
    std::size_t sinkBytes{256U * 1024U};
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

static BenchSinkMode benchSinkMode() {
    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_SINK")) {
        const std::string value = options::uppercaseAscii(env);
        if (value == "DEFAULT") {
            return BenchSinkMode::Default;
        }
        if (value == "BUFFERED") {
            return BenchSinkMode::Buffered;
        }
        if (value == "ASYNC") {
            return BenchSinkMode::Async;
        }
        if (value == "NULL" || value == "NOOP") {
            return BenchSinkMode::Null;
        }
        throw options::OptionError(
            "SCOPE_TIMER_BENCH_SINK must be DEFAULT, BUFFERED, ASYNC, NULL, or NOOP; got '" +
            std::string(env) + "'"
        );
    }
    return BenchSinkMode::Default;
}

static BenchTimerMode benchTimerMode() {
    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_TIMER")) {
        const std::string value = options::uppercaseAscii(env);
        if (value == "DEFAULT" || value == "STANDARD") {
            return BenchTimerMode::Default;
        }
        if (value == "HOTPATH" || value == "FAST") {
            return BenchTimerMode::HotPath;
        }
        throw options::OptionError(
            "SCOPE_TIMER_BENCH_TIMER must be DEFAULT, STANDARD, HOTPATH, or FAST; got '" +
            std::string(env) + "'"
        );
    }
    return BenchTimerMode::Default;
}

static BenchmarkRuntimeOptions parseRuntimeOptions() {
    BenchmarkRuntimeOptions runtimeOptions;
    runtimeOptions.sinkMode = benchSinkMode();
    runtimeOptions.timerMode = benchTimerMode();

    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_THREADS")) {
        runtimeOptions.threadCount = static_cast<int>(options::parseBoundedUnsigned(
            env,
            "SCOPE_TIMER_BENCH_THREADS",
            1U,
            options::MaxBenchmarkThreads
        ));
    }
    if (const char* env = std::getenv("SCOPE_TIMER_BENCH_SINK_BYTES")) {
        runtimeOptions.sinkBytes = options::parseBoundedSize(
            env,
            "SCOPE_TIMER_BENCH_SINK_BYTES",
            1U,
            options::MaxBenchmarkSinkBytes
        );
    }
    return runtimeOptions;
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

    explicit BenchSinkScope(const BenchmarkRuntimeOptions& runtimeOptions) {
        switch (runtimeOptions.sinkMode) {
            case BenchSinkMode::Buffered:
                SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(runtimeOptions.sinkBytes);
                buffered_ = true;
                break;
            case BenchSinkMode::Async:
                SCOPE_TIMER_ENABLE_ASYNC_SINK(runtimeOptions.sinkBytes);
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

static void hotPathBenchmark(int iterations, const BenchmarkRuntimeOptions& runtimeOptions) {
    const int rounds = std::max(1, iterations) * 12;
    const int threadCount = runtimeOptions.threadCount;
    const BenchTimerMode timerMode = runtimeOptions.timerMode;

    const auto timerRecords = static_cast<std::uint64_t>(iterations)
        * static_cast<std::uint64_t>(rounds)
        * 256U
        * static_cast<std::uint64_t>(threadCount);
    if (timerRecords > options::MaxBenchmarkTimerRecords) {
        throw options::OptionError(
            "benchmark workload would emit " + std::to_string(timerRecords) +
            " timer records; reduce --iterations or SCOPE_TIMER_BENCH_THREADS "
            "(maximum " + std::to_string(options::MaxBenchmarkTimerRecords) + ")"
        );
    }

    SCOPE_TIMER("hotPath:benchmark");
    if (threadCount == 1) {
        hotPathBenchmarkWorker(rounds, timerMode);
        return;
    }

    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(threadCount));
    for (int i = 0; i < threadCount; ++i) {
        workers.emplace_back([rounds, timerMode] {
            hotPathBenchmarkWorker(rounds, timerMode);
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }
}

static void printUsage() {
    std::cout << "Usage: Benchmark [--iterations=N] [--scenario=hotpath-bench]\n"
                 "  N must be between 1 and " << options::MaxBenchmarkIterations << ".\n"
                 "The dedicated benchmark executable drives a CPU-bound ScopeTimer\n"
                 "stress workload used by the benchmark scripts and CMake targets.\n"
                 "Benchmark env knobs: SCOPE_TIMER_BENCH_SINK=DEFAULT|BUFFERED|ASYNC|NULL,\n"
                 "SCOPE_TIMER_BENCH_SINK_BYTES=<1.." << options::MaxBenchmarkSinkBytes << ">,\n"
                 "SCOPE_TIMER_BENCH_THREADS=<1.." << options::MaxBenchmarkThreads << ">,\n"
                 "and SCOPE_TIMER_BENCH_TIMER=DEFAULT|HOTPATH.\n"
                 "Harness probe: --instrumentation-status\n";
}

static bool instrumentationStatusRequested(int argc, char** argv) {
    return argc == 2 && argv[1] != nullptr &&
        std::string_view{argv[1]} == "--instrumentation-status";
}

static void printInstrumentationStatus() {
#ifndef NDEBUG
    std::cout << "ScopeTimerBenchmark protocol=1 instrumentation=enabled\n";
#else
    std::cout << "ScopeTimerBenchmark protocol=1 instrumentation=disabled\n";
#endif
}

static bool helpRequested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i] ? std::string_view{argv[i]} : std::string_view{};
        if (arg == "-h" || arg == "--help") {
            return true;
        }
    }
    return false;
}

static BenchmarkOptions parseOptions(int argc, char** argv) {
    SCOPE_TIMER("Benchmark::parseOptions");

    BenchmarkOptions parsedOptions;
    bool iterationsSet = false;
    bool scenarioSet = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            continue;
        } else if (arg.rfind("--iterations=", 0) == 0) {
            if (iterationsSet) {
                throw options::OptionError("iterations may only be specified once");
            }
            parsedOptions.iterations = static_cast<int>(options::parseBoundedUnsigned(
                std::string_view(arg).substr(13),
                "iterations",
                1U,
                options::MaxBenchmarkIterations
            ));
            iterationsSet = true;
        } else if (arg.rfind("--scenario=", 0) == 0) {
            if (scenarioSet) {
                throw options::OptionError("scenario may only be specified once");
            }
            if (const std::string value = arg.substr(11); value != "hotpath-bench") {
                throw options::OptionError("unknown benchmark scenario '" + value + "'");
            }
            parsedOptions.scenario = BenchmarkScenario::HotPathBench;
            scenarioSet = true;
        } else if (!arg.empty() && arg.front() == '-') {
            throw options::OptionError("unknown option '" + arg + "'");
        } else {
            if (iterationsSet) {
                throw options::OptionError("iterations may only be specified once");
            }
            parsedOptions.iterations = static_cast<int>(options::parseBoundedUnsigned(
                arg,
                "iterations",
                1U,
                options::MaxBenchmarkIterations
            ));
            iterationsSet = true;
        }
    }
    return parsedOptions;
}

int main(int argc, char** argv) {
    if (instrumentationStatusRequested(argc, argv)) {
        printInstrumentationStatus();
        return 0;
    }
    if (helpRequested(argc, argv)) {
        printUsage();
        return 0;
    }

    try {
        const BenchmarkRuntimeOptions runtimeOptions = parseRuntimeOptions();
        BenchSinkScope sinkScope(runtimeOptions);
        {
            SCOPE_TIMER("Benchmark::main");
            const BenchmarkOptions parsedOptions = parseOptions(argc, argv);

            // Preserve the existing benchmark scaling behavior so historical results
            // remain comparable when the dedicated executable replaces the old
            // benchmark-only path inside Demo.cpp.
            for (int i = 0; i < parsedOptions.iterations; ++i) {
                if (parsedOptions.scenario == BenchmarkScenario::HotPathBench) {
                    hotPathBenchmark(parsedOptions.iterations, runtimeOptions);
                }
            }
        }
        return 0;
    } catch (const options::OptionError& error) {
        std::cerr << "Benchmark: " << error.what() << "\nTry 'Benchmark --help' for usage.\n";
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
