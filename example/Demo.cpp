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
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>

using namespace std::chrono_literals;

// Simulate some work for a given duration
static void busyFor(std::chrono::microseconds us) {
    // Intentional small sleep to keep the example simple
    std::this_thread::sleep_for(us);
}

// Example 1: Simple function timing
static void simpleWork() {
    SCOPE_TIMER("simpleWork");
    busyFor(2500us);
}

// Example 2: Nested scopes
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

// Example 3: Multiple timers in the same scope (demonstrates unique macro var names)
static void multipleTimersSameScope() {
    SCOPE_TIMER("multi:first");
    busyFor(600us);
    SCOPE_TIMER("multi:second");
    busyFor(700us);
    SCOPE_TIMER("multi:third"); busyFor(800us); SCOPE_TIMER("multi:fourth");
    busyFor(900us);
}

// Example 4: Conditional timing (only enabled when condition is true)
static void conditionalWork(bool enabled) {
    SCOPE_TIMER_IF(enabled, "conditionalWork");
    // Work always runs; the timer only records when 'enabled' is true
    busyFor(1200us);
}

// Example 5: Loop with per-iteration timing label
static void loopedWork(int iterations) {
    SCOPE_TIMER("loopedWork:total");
    for (int i = 0; i < iterations; ++i) {
        // Per-iteration scope timer; shows repeated log lines with the same label
        SCOPE_TIMER("loopedWork:iteration");
        busyFor(300us);
    }
}

// Example 6: Multithreaded timing
static void threadedWork(int threads) {
    SCOPE_TIMER("threadedWork:total");
    std::vector<std::thread> tg;
    tg.reserve(threads);
    for (int i = 0; i < threads; ++i) {
        tg.emplace_back([i] {
            SCOPE_TIMER("threadedWork:worker");
            // Each worker does a bit of variable time work
            busyFor(std::chrono::microseconds{ 500 + (i * 200) });
        });
    }
    for (auto &t : tg) t.join();
}

// Example 7: Using SCOPE_TIMER in a class
// Demonstrates usage of SCOPE_TIMER inside class methods.
class Worker {
public:
    Worker() {
        SCOPE_TIMER("Worker:constructor");
        busyFor(500us); // simulate setup
    }

    // Explicit special members: Worker is trivially copyable/movable
    Worker(const Worker&) = default;
    Worker& operator=(const Worker&) = default;
    Worker(Worker&&) noexcept = default;
    Worker& operator=(Worker&&) noexcept = default;
    ~Worker() = default;

    void doTask([[maybe_unused]] const std::string_view name) const { // in a Release build this will be unused
        SCOPE_TIMER(name);
        busyFor(1000us); // simulate work
    }

    void doMultipleTasks(int count, bool timed) const {
        SCOPE_TIMER_IF(timed, "Worker:doMultipleTasks");
        for (int i = 0; i < count; ++i) {
            doTask("Worker:task");
        }
    }
};

// Example 8: Tracking an object's lifetime using SCOPE_TIMER as a member
class LifetimeTracked {
public:
    LifetimeTracked() {
        busyFor(500us); // simulate some setup work
    }

    ~LifetimeTracked() {
        busyFor(500us); // simulate some cleanup work
    }

    // Explicitly non-copyable and non-movable (matches ScopeTimer member)
    LifetimeTracked(const LifetimeTracked&) = delete;
    LifetimeTracked& operator=(const LifetimeTracked&) = delete;
    LifetimeTracked(LifetimeTracked&&) noexcept = delete;
    LifetimeTracked& operator=(LifetimeTracked&&) noexcept = delete;

private:
    // The ScopeTimer member starts timing on construction and logs on destruction,
    // effectively measuring the entire lifetime of this object.
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

static int parseIterations(int argc, char** argv) {
    SCOPE_TIMER("Demo::parseIterations");

    int iterations = 1;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: Demo [--iterations=N]\n"
                         "When --iterations > 1 the demo repeats the workload N times\n"
                         "and scales per-scope loops so it can be used for benchmarking.\n";
            std::exit(0);
        } else if (arg.rfind("--iterations=", 0) == 0) {
            iterations = std::max(1, std::stoi(arg.substr(13)));
        } else {
            iterations = std::max(1, std::stoi(arg));
        }
    }
    return iterations;
}

int main(int argc, char** argv) {
    SCOPE_TIMER("Demo::main");

    const int iterations = parseIterations(argc, argv);
    for (int i = 0; i < iterations; ++i) {
        runDemoSuite(iterations);
    }
    return 0;
}
