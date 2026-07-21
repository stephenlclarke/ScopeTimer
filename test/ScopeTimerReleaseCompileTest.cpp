/*
 * ScopeTimer - lightweight C++20 scope timing utility
 * Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef NDEBUG
#error "ScopeTimerReleaseCompileTest must be compiled with NDEBUG defined"
#endif

#include "ScopeTimer.hpp"

#include <cstddef>
#include <type_traits>

namespace {

class ReleaseSink final : public ::xyzzy::scopetimer::ScopeTimer::LogSink {
public:
    void write(const char*, std::size_t) noexcept override {}
};

} // namespace

int main() {
    using ScopeTimer = ::xyzzy::scopetimer::ScopeTimer;

    static_assert(!std::is_copy_constructible_v<ScopeTimer>);
    static_assert(!std::is_copy_assignable_v<ScopeTimer>);
    static_assert(!std::is_move_constructible_v<ScopeTimer>);
    static_assert(!std::is_move_assignable_v<ScopeTimer>);

    ReleaseSink sink;
    ScopeTimer::setLogSink(sink);
    ScopeTimer::enableThreadBufferedSink(1024U);
    ScopeTimer::disableThreadBufferedSink();
    ScopeTimer::enableAsyncSink(1024U);
    ScopeTimer::disableAsyncSink();
    ScopeTimer::resetLogSink();

    ScopeTimer regularTimer("release:where", "release:label");
    ScopeTimer hotPathTimer(ScopeTimer::HotPathTag{}, "release:hot-path");

    int sideEffects = 0;
    struct Flags {
        unsigned enabled : 1;
    } flags{1U};
    const char* instrumentationOnlyLabel = "release:instrumentation-only";
    const auto lambdaCondition = [&flags] { return flags.enabled != 0U; };

    SCOPE_TIMER();
    SCOPE_TIMER(++sideEffects);
    SCOPE_TIMER_IF(++sideEffects, ++sideEffects);
    SCOPE_TIMER(instrumentationOnlyLabel);
    SCOPE_TIMER_IF(flags.enabled, instrumentationOnlyLabel);
    SCOPE_TIMER_IF(lambdaCondition());
    SCOPE_TIMER_HOT_PATH(++sideEffects);
    SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(++sideEffects);
    SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK();
    SCOPE_TIMER_ENABLE_ASYNC_SINK(++sideEffects);
    SCOPE_TIMER_DISABLE_ASYNC_SINK();

    return sideEffects;
}
