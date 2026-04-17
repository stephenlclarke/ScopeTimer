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
 *
 * ScopeTimer Overview:
 * --------------------
 * The ScopeTimer class is a lightweight, high-resolution timing utility designed to measure
 * the execution duration of functions and code blocks with minimal overhead. It is intended
 * for use in performance diagnostics and profiling during development (Debug builds), with
 * zero runtime cost in Release builds.
 *
 * Design Goals:
 * -------------
 * 1. **Low Overhead in Debug Builds**: Use stack-allocated buffers and avoid heap allocations
 *    for logging. Capture timestamps efficiently using `std::chrono::steady_clock` for duration
 *    measurement and `std::chrono::system_clock` for human-readable wall clock times.
 * 2. ** Zero Overhead in Release Builds**: In Release mode, ScopeTimer is a complete no-op,
 *    ensuring no performance impact or binary bloat.
 * 3. **Thread Safety**: Logging is protected by a mutex to ensure non-interleaved log entries
 *    while keeping timing logic lock-free.
 * 4. **Buffered High-Throughput Logging**: Uses a large user buffer with periodic flushes to
 *    reduce I/O overhead while preserving timely log visibility.
 * 5. **Runtime Configurability**: Environment variables allow enabling/disabling timing,
 *    specifying the log directory, and controlling flush frequency without recompilation.
 *
 * Environment Variables:
 * ----------------------
 * - SCOPE_TIMER:
 *     Controls whether timing is enabled in Debug builds.
 *     Set to "OFF", "FALSE", "NO", or "0" (case-insensitive) to disable timing.
 *     Any other value or unset enables timing.
 *
 * - SCOPE_TIMER_DIR:
 *     Specifies the directory path where the log file `ScopeTimer.log` is created.
 *     Defaults to `/tmp` if unset.
 *
 * - SCOPE_TIMER_FLUSH_N:
 *     Specifies the number of log lines between flushes to the log file.
 *     Must be a positive integer; defaults to 256 if unset or invalid.
 *
 * - SCOPE_TIMER_FORMAT:
 *     Controls the units used for displaying elapsed time. Accepted values:
 *     "SECONDS", "MILLIS", "MICROS", or "NANOS" (case-insensitive). If unset or invalid,
 *     the timer uses an automatic format (seconds with ms decimals, else ms with
 *     us decimals, else microseconds).
 *
 * - SCOPE_TIMER_WALLTIME:
 *     Controls whether start/end wall-clock timestamps are included in each record.
 *     Set to "OFF", "FALSE", "NO", or "0" (case-insensitive) to log elapsed time only.
 * 
 * Usage Example 1:
 * ---------------
 * #include "ScopeTimer.hpp"
 *
 * void exampleFunction() {
 *     SCOPE_TIMER(); // Starts timing this scope
 *     // ... code to be timed ...
 * }
 *
 * Output:
 * ------
 * [ScopeTimer] TID=001 | int exampleFunction() | start=2025-08-13 11:57:21.832 | end=2025-08-13 11:57:35.885 | elapsed=14.052s
 *
 * Usage Example 2:
 * ---------------
 * void anotherExampleFunction() {
 *     SCOPE_TIMER("Useful logging info"); // Starts timing this scope
 *     // ... code to be timed ...
 * }
 *
 * Output:
 * ------
 * [Useful logging info] TID=001 | int anotherExampleFunction() | start=2025-08-13 12:00:12.659 | end=2025-08-13 12:00:26.714 | elapsed=14.055s
 */
#pragma once

#include <atomic>
#include <algorithm>  // for std::transform
#include <array>
#include <cctype>     // for std::toupper
#include <chrono>
#include <charconv>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <sys/stat.h>
#if !defined(_WIN32)
#include <sys/uio.h>
#endif
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#define SCOPE_FUNCTION __PRETTY_FUNCTION__
#else
#define SCOPE_FUNCTION __func__
#endif

// Always open the namespaces unconditionally, so they are present in all builds.
namespace xyzzy::scopetimer {

    class ScopeTimer_TestFriend; // Forward declaration

#ifndef NDEBUG // Debug build only

    namespace detail {
        template <typename T>
        class NoDestroy {
        public:
            NoDestroy() {
                ::new (static_cast<void*>(storage_.data())) T();
            }

            template <typename First, typename... Rest,
                      typename = std::enable_if_t<!std::is_same_v<NoDestroy, std::decay_t<First>>>>
            explicit NoDestroy(First&& first, Rest&&... rest) {
                ::new (static_cast<void*>(storage_.data())) T(
                    std::forward<First>(first),
                    std::forward<Rest>(rest)...
                );
            }

            T& get() noexcept {
                return *std::launder(static_cast<T*>(static_cast<void*>(storage_.data())));
            }

        private:
            alignas(T) std::array<std::byte, sizeof(T)> storage_{};
        };

        template <typename Tag, typename T, typename... Args>
        inline T& singletonStorage(Args&&... args) {
            static NoDestroy<T> value(std::forward<Args>(args)...);
            return value.get();
        }

        struct OutMutexTag {};
        struct LineCounterTag {};
        struct ThreadBufferFlushBytesTag {};
        struct ThreadBufferRegistryMutexTag {};
        struct ThreadBufferRegistryTag {};
        struct SinkConfigMutexTag {};
        struct ActiveSinkStorageTag {};
        struct BufferedSinkTargetModeStorageTag {};
        struct AsyncSinkTargetModeStorageTag {};
        struct CustomSinkWriteStorageTag {};
        struct CustomSinkFlushStorageTag {};
        struct BufferedTestSinkWriteStorageTag {};
        struct AsyncSinkStateTag {};
    } // namespace detail

    inline std::mutex& outMutex() noexcept {
        return detail::singletonStorage<detail::OutMutexTag, std::mutex>();
    }

    inline std::atomic<unsigned>& lineCounter() noexcept {
        return detail::singletonStorage<detail::LineCounterTag, std::atomic<unsigned>>(0U);
    }

    // Small helper extracted to make branch coverage testable in unit tests
    namespace ScopeTimerDetail {
        inline std::size_t finalize_snprintf_result(int n, char* line, std::size_t lineSize) noexcept {
            if (n < 0) {
                // Formatting error: write nothing
                line[0] = '\0';
                return 0U;
            }

            if (static_cast<std::size_t>(n) >= lineSize) {
                // Truncated: snprintf wrote size-1 chars and a terminating '\0'
                const std::size_t len = lineSize - 1U;
                line[len] = '\0'; // ensure terminator
                return len;
            }

            // Exact number of characters written (excluding '\0')
            return static_cast<std::size_t>(n);
        }
    } // namespace ScopeTimerDetail

    namespace detail {
        enum class LabelStorageKind : std::uint8_t {
            Borrowed,
            Copy,
        };

        struct LabelData {
            std::string storage;
            std::string_view view{ "ScopeTimer" };
            LabelStorageKind storageKind{LabelStorageKind::Borrowed};

            LabelData() = default;

            explicit LabelData(
                std::string_view v,
                std::string&& owned = {},
                LabelStorageKind kind = LabelStorageKind::Copy
            ) noexcept
                : storage(std::move(owned)),
                  storageKind(kind) {
                if (!storage.empty()) {
                    view = storage;
                } else if (!v.empty()) {
                    view = v;
                } else {
                    view = "ScopeTimer";
                    storageKind = LabelStorageKind::Borrowed;
                }
            }

            bool canBorrowView() const noexcept {
                return storage.empty() && storageKind == LabelStorageKind::Borrowed;
            }
        };

        class LabelArg {
        public:
            LabelArg() = default;

            template <std::size_t N>
            explicit LabelArg(const char (&literal)[N]) noexcept
                : view_(literal, N ? N - 1 : 0) {
                storageKind_ = LabelStorageKind::Borrowed;
            }

            explicit LabelArg(const char* s) {
                if (s && *s) {
                    view_ = std::string_view{s};
                    storageKind_ = LabelStorageKind::Copy;
                } else {
                    view_ = std::string_view{"ScopeTimer"};
                    storageKind_ = LabelStorageKind::Borrowed;
                }
            }

            explicit LabelArg(std::string_view sv)
                : view_(sv),
                  storageKind_(LabelStorageKind::Copy) {}

            explicit LabelArg(const std::string& s)
                : owned_(s),
                  view_(owned_),
                  storageKind_(LabelStorageKind::Copy) {}

            explicit LabelArg(std::string&& s) noexcept
                : owned_(std::move(s)),
                  view_(owned_),
                  storageKind_(LabelStorageKind::Copy) {}

            LabelData toLabelData() && noexcept {
                return LabelData(view_, std::move(owned_), storageKind_);
            }

        private:
            std::string owned_;
            std::string_view view_{ "ScopeTimer" };
            LabelStorageKind storageKind_{LabelStorageKind::Borrowed};
        };

        inline LabelData makeLabelData() noexcept {
            return LabelData{};
        }

        template <std::size_t N>
        inline LabelData makeLabelData(const char (&literal)[N]) noexcept {
            return LabelData(
                std::string_view{literal, N ? N - 1 : 0},
                {},
                LabelStorageKind::Borrowed
            );
        }

        template <typename CharPtr,
                  typename = std::enable_if_t<
                      std::is_pointer_v<std::remove_reference_t<CharPtr>> &&
                      std::is_same_v<
                          std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<CharPtr>>>,
                          char>>>
        inline LabelData makeLabelData(CharPtr s) {
            return LabelArg{s}.toLabelData();
        }

        inline LabelData makeLabelData(std::string_view sv) {
            return LabelArg{sv}.toLabelData();
        }

        inline LabelData makeLabelData(const std::string& s) {
            return LabelArg{s}.toLabelData();
        }

        inline LabelData makeLabelData(std::string&& s) noexcept {
            return LabelArg{std::move(s)}.toLabelData();
        }
    } // namespace detail

    /**
     * @brief A high-resolution scope timer for measuring execution time of code blocks.
     *
     * This class uses C++17 features such as std::string_view for lightweight string parameters,
     * thread_local storage and std::atomic for lock-free thread ID assignment,
     * and fixed-size stack buffers for efficient formatting without heap allocation.
     * It logs timing information to a configurable sink with buffered options for
     * high throughput.
     */
    class ScopeTimer {
    public:
        struct HotPathTag {};

        /**
         * @brief Interface for user-supplied log sinks.
         *
         * Register an implementation with setLogSink() to redirect ScopeTimer
         * output away from the default logfile. The sink object must outlive the
         * registration and remain valid until resetLogSink() is called.
         */
        class LogSink {
        public:
            virtual ~LogSink() = default;
            virtual void write(const char* data, std::size_t len) noexcept = 0;
            virtual void flush() noexcept {
                // Default sinks may be purely streaming and need no flush hook.
            }
        };

        /**
         * @brief Constructs a ScopeTimer instance and records the start time.
         *
         * @param where A std::string_view describing the scope or function being timed.
         * @param labelData A helper struct conveying the label string and any owned storage.
         */
        inline explicit ScopeTimer(std::string_view where, detail::LabelData labelData = detail::LabelData{}) noexcept {
            if(isDisabled()) {
                disabled_ = true;
                return;
            }

            where_ = where;
            assignLabel(std::move(labelData));
            threadNum_ = getThreadIdNumber();
            startSteady_ = std::chrono::steady_clock::now();
            if (includeWallTime()) {
                startWall_ = std::chrono::system_clock::now();
                startWallFormattedLen_ = static_cast<std::uint8_t>(formatTime(startWall_, startWallFormatted_, sizeof(startWallFormatted_)));
            }
        }

        /**
         * @brief Convenience overload that accepts a plain string_view label.
         */
        inline explicit ScopeTimer(std::string_view where, std::string_view label) noexcept
            : ScopeTimer(where, detail::LabelData{label}) {}

        template <std::size_t N>
        inline explicit ScopeTimer(std::string_view where, const char (&label)[N]) noexcept
            : ScopeTimer(where, detail::LabelData{
                std::string_view{label, N ? N - 1 : 0},
                {},
                detail::LabelStorageKind::Borrowed
            }) {}

        inline explicit ScopeTimer(HotPathTag, detail::LabelData labelData = detail::LabelData{}) noexcept {
            if (isDisabled()) {
                disabled_ = true;
                return;
            }

            hotPathMode_ = true;
            assignLabel(std::move(labelData));
            startSteady_ = std::chrono::steady_clock::now();
        }

        inline explicit ScopeTimer(HotPathTag, std::string_view label) noexcept
            : ScopeTimer(HotPathTag{}, detail::LabelData{label}) {}

        template <std::size_t N>
        inline explicit ScopeTimer(HotPathTag, const char (&label)[N]) noexcept
            : ScopeTimer(HotPathTag{}, detail::LabelData{
                std::string_view{label, N ? N - 1 : 0},
                {},
                detail::LabelStorageKind::Borrowed
            }) {}

        ScopeTimer(const ScopeTimer&) = delete; ///< Deleted copy constructor for safety.
        ScopeTimer& operator=(const ScopeTimer&) = delete; ///< Deleted copy assignment operator.

        /**
         * @brief Destructor that records the end time, computes elapsed duration,
         * formats timing information, and writes a log entry.
         *
         * Logs include thread ID, scope name, start and end timestamps, and elapsed time.
         */
        inline ~ScopeTimer() noexcept {
            if (disabled_) {
                return;
            }

            const auto endSteady = std::chrono::steady_clock::now();
            const auto elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endSteady - startSteady_).count();

            auto& fmtBufs = formatBuffers();
            std::size_t len = 0;

            // Final line buffer reused per thread to avoid repeated stack allocation.
            auto& lineBuf = lineBuffer();
            if (hotPathMode_) {
                fmtBufs.elapsedLen = static_cast<std::uint8_t>(formatElapsedNanos(elapsedNs, fmtBufs.elapsedBuf, sizeof(fmtBufs.elapsedBuf)));
                len = buildHotPathLogLine(
                    lineBuf.data,
                    sizeof(lineBuf.data),
                    label_,
                    fmtBufs.elapsedBuf,
                    fmtBufs.elapsedLen
                );
            } else {
                const bool wallTimeEnabled = includeWallTime();
                if (wallTimeEnabled) {
                    const auto endWall = std::chrono::system_clock::now();
                    fmtBufs.endLen = static_cast<std::uint8_t>(formatTime(endWall, fmtBufs.endBuf, sizeof(fmtBufs.endBuf)));
                } else {
                    fmtBufs.endLen = 0;
                }
                fmtBufs.elapsedLen = static_cast<std::uint8_t>(formatElapsed(elapsedNs, fmtBufs.elapsedBuf, sizeof(fmtBufs.elapsedBuf)));

                len = buildLogLine(lineBuf.data, sizeof(lineBuf.data), LogLineFields{
                    label_,
                    threadNum_,
                    where_,
                    std::string_view{startWallFormatted_, startWallFormattedLen_},
                    std::string_view{fmtBufs.endBuf, fmtBufs.endLen},
                    std::string_view{fmtBufs.elapsedBuf, fmtBufs.elapsedLen},
                    wallTimeEnabled
                });
            }

            // These sink-state atomics are intentionally acquire/release or relaxed
            // instead of seq_cst. They publish configuration chosen under
            // sinkConfigMutex(), and stronger global ordering would add fences on the
            // steady-state timer path without improving correctness. Sonar's blanket
            // seq_cst rule is suppressed for this header in sonar-project.properties.
            const auto activeSink = activeSinkStorage().load(std::memory_order_acquire);
            if (len) {
                if (activeSink != ActiveSink::ThreadBuffered) {
                    std::lock_guard lock(outMutex());
                    writeToActiveSink(activeSink, lineBuf.data, len);
                } else {
                    writeToActiveSink(activeSink, lineBuf.data, len);
                }
            }

            // Periodic flush to keep logs visible even with large buffers
            unsigned cnt = lineCounter().fetch_add(1, std::memory_order_relaxed) + 1;

            // Thread-buffered sink flushes on size; avoid periodic flush to keep the hot path cheap.
            const bool flushViaInterval = activeSink != ActiveSink::ThreadBuffered;
            if (flushViaInterval && (cnt % flushInterval() == 0)) { // configurable via SCOPE_TIMER_FLUSH_N
                flushActiveSink(activeSink);
            }
        }

        static inline void enableThreadBufferedSink(std::size_t flushBytes = 16U * 1024U) noexcept {
            if (flushBytes == 0) {
                flushBytes = 16U * 1024U;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            closeLogFd();
            threadBufferFlushBytesStorage().store(flushBytes);
            activeSinkStorage().store(ActiveSink::ThreadBuffered, std::memory_order_release);
            bufferedSinkTargetModeStorage().store(hasCustomSink() ? BufferedSinkTargetMode::Custom
                                                                  : BufferedSinkTargetMode::Default,
                                                  std::memory_order_release);
        }

        static inline void disableThreadBufferedSink() noexcept {
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            closeLogFd();
            restoreDefaultSinkState();
        }

        static inline void enableAsyncSink(std::size_t flushBytes = 16U * 1024U) noexcept {
            if (flushBytes == 0) {
                flushBytes = 16U * 1024U;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            closeLogFd();
            threadBufferFlushBytesStorage().store(flushBytes);
            activeSinkStorage().store(ActiveSink::ThreadBuffered, std::memory_order_release);
            bufferedSinkTargetModeStorage().store(BufferedSinkTargetMode::Async, std::memory_order_release);
            asyncSinkTargetModeStorage().store(hasCustomSink() ? AsyncSinkTargetMode::Custom
                                                               : AsyncSinkTargetMode::Default,
                                               std::memory_order_release);
            ensureAsyncSinkRunning();
        }

        static inline void disableAsyncSink() noexcept {
            disableThreadBufferedSink();
        }

        static inline void setLogSink(LogSink& sink) {
            auto* sinkPtr = &sink;
            setCustomSinkCallbacks(
                [sinkPtr](const char* data, std::size_t len) {
                    sinkPtr->write(data, len);
                },
                [sinkPtr]() {
                    sinkPtr->flush();
                }
            );
        }

        static inline void resetLogSink() {
            setCustomSinkCallbacks();
        }

    private:
        friend class xyzzy::scopetimer::ScopeTimer_TestFriend; // Allow unit tests to access private members
        
        /**
         * @brief Checks if the ScopeTimer is disabled based on the SCOPE_TIMER environment variable.
         *
         * Reads the environment variable once and caches the result for subsequent calls.
         * Returns true if the value is "OFF", "FALSE", "NO", or "0" (case-insensitive).
         *
         * @return true if disabled, false otherwise.
         */
        static inline bool isDisabled() noexcept {
            static const bool disabled = []() {
                const char* env = std::getenv("SCOPE_TIMER");

                if(!env) {
                    return false;
                }

                std::string val(env);

                for(auto& c : val) {
                    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                }

                return val == "OFF" || val == "FALSE" || val == "NO" || val == "0";
            }();

            return disabled;
        }

        static inline bool isTruthySetting(const char* envName, bool defaultValue) noexcept {
            if (const char* env = std::getenv(envName); env && *env) {
                std::string value(env);
                std::transform(value.begin(), value.end(), value.begin(),
                               [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
                if (value == "OFF" || value == "FALSE" || value == "NO" || value == "0") {
                    return false;
                }
                return true;
            }
            return defaultValue;
        }

        static inline bool includeWallTime() noexcept {
            static const bool enabled = isTruthySetting("SCOPE_TIMER_WALLTIME", true);
            return enabled;
        }

        /**
         * @brief Retrieves a unique thread ID number in a lock-free manner.
         *
         * Uses thread_local storage to cache the thread ID, and an atomic counter
         * to assign unique IDs starting from 1.
         *
         * @return uint32_t The unique thread ID number.
         */
        static inline uint32_t getThreadIdNumber() noexcept {
            thread_local uint32_t tid = 0;

            if(tid == 0) {
                static std::atomic<uint32_t> next{ 1 };
                // Only uniqueness matters here; callers do not depend on any cross-thread
                // ordering relationship with the assigned numeric IDs.
                tid = next.fetch_add(1, std::memory_order_relaxed);
            }

            return tid;
        }

        /**
         * @brief Returns the periodic flush interval for the ScopeTimer log.
         *
         * Controlled by the environment variable `SCOPE_TIMER_FLUSH_N`.
         * If unset, non-numeric, or <= 0, defaults to 4096. Parsed once and cached.
         *
         * @return unsigned The number of lines between flushes.
         */
        static inline unsigned flushInterval() noexcept {
            static const auto interval = []() noexcept {
                if (const auto* p = std::getenv("SCOPE_TIMER_FLUSH_N")) {
                    auto* end = static_cast<char*>(nullptr);
                    const auto v = std::strtoul(p, &end, 10);

                    if (end != p && *end == '\0' && v > 0UL && v <= 1000000UL) {
                        return static_cast<unsigned>(v);
                    }
                }
                return 4096U; // default
            }();
            return interval;
        }

        /**
         * @brief Resolves the log directory from environment overrides.
         *
         * Controlled solely by SCOPE_TIMER_DIR; defaults to /tmp when unset/empty.
         */
        static inline const std::string& logDirectory() {
            if (!logDirInitialized_) {
                resetLogDirectoryForTests();
                logDirInitialized_ = true;
            }
            return logDirCache_;
        }

        static inline void resetLogDirectoryForTests(std::string_view newDir = {}) {
            std::string normalized;
            if (!newDir.empty()) {
                normalized.assign(newDir.begin(), newDir.end());
            } else {
                if (const char* primary = std::getenv("SCOPE_TIMER_DIR"); primary && *primary) {
                    normalized = primary;
                }
            }
            if (normalized.empty()) {
                normalized = "/tmp";
            }
            if (!normalized.empty() && normalized.back() != '/') {
                normalized.push_back('/');
            }
            logDirCache_ = std::move(normalized);
            logDirInitialized_ = true;
        }

        // One-time-selected elapsed-time formatter infrastructure
        // I call through a cached function pointer to avoid branching in the hot path.
        enum class TimeFormat { Auto, Seconds, Millis, Micros, Nanos };
        using FormatterFn = std::size_t(*)(long long ns, char* out, size_t outSz) noexcept;

        // Portable localtime shim
#if defined(_WIN32)
#  define LOCALTIME(tm_ptr, time_ptr) localtime_s((tm_ptr), (time_ptr))
#else
#  define LOCALTIME(tm_ptr, time_ptr) localtime_r((time_ptr), (tm_ptr))
#endif

        static inline bool appendFixedDigits(char*& out, const char* end, unsigned value, unsigned width) noexcept {
            std::array<char, 10> tmp{};
            if (width > tmp.size() || static_cast<std::size_t>(end - out) < width) {
                return false;
            }
            for (unsigned i = 0; i < width; ++i) {
                const unsigned place = width - 1U - i;
                unsigned divisor = 1U;
                for (unsigned j = 0; j < place; ++j) {
                    divisor *= 10U;
                }
                tmp[i] = static_cast<char>('0' + ((value / divisor) % 10U));
            }
            std::memcpy(out, tmp.data(), width);
            out += width;
            return true;
        }

        /**
         * @brief Elapsed time formatters selected once for speed.
         *
         * Each formatter accepts elapsed nanoseconds and writes a formatted
         * string into the provided buffer. The active formatter is chosen once
         * from the SCOPE_TIMER_FORMAT environment variable and cached via a
         * function pointer to avoid branching in the hot path.
         */
        static inline std::size_t fmtSeconds(long long ns, char* out, size_t outSz) noexcept {
            const long long sec   = ns / 1000000000LL;
            const auto remMs = static_cast<unsigned>((ns / 1000000LL) % 1000LL); // 3 decimals
            return formatElapsedImpl(static_cast<unsigned long long>(sec), remMs, 's', out, outSz);
        }

        static inline std::size_t fmtMillis(long long ns, char* out, size_t outSz) noexcept {
            const long long ms    = ns / 1000000LL;
            const auto remUs = static_cast<unsigned>((ns / 1000LL) % 1000LL);   // 3 decimals for ms
            return formatElapsedImpl(static_cast<unsigned long long>(ms), remUs, 'm', out, outSz); // suffix handled inside
        }

        static inline std::size_t fmtMicros(long long ns, char* out, size_t outSz) noexcept {
            const long long us    = ns / 1000LL;
            const auto remNs = static_cast<unsigned>(ns % 1000LL); // nanoseconds remainder
            return formatElapsedImpl(static_cast<unsigned long long>(us), remNs, 'u', out, outSz); // suffix handled inside
        }

        static inline std::size_t fmtNanos(long long ns, char* out, size_t outSz) noexcept {
            char* cur = out;
            const char* end = out + outSz;
            if (auto* next = appendUnsignedToBuffer(cur, end, static_cast<unsigned long long>(ns)); next) {
                cur = next;
                (void)appendStrToBuffer(cur, end, "ns", 2);
            }
            if (cur < end) {
                *cur = '\0';
            } else if (outSz) {
                out[outSz - 1] = '\0';
            }
            return static_cast<std::size_t>(cur - out);
        }

        /**
         * @brief Automatic elapsed time formatter.
         *
         * Selects the most appropriate unit: seconds (>=1s), milliseconds (>=1ms), microseconds (>=1us), or nanoseconds (<1us).
         * For explicit microseconds, use SCOPE_TIMER_FORMAT=MICROS.
         */
        static inline std::size_t fmtAuto(long long ns, char* out, size_t outSz) noexcept {
            if (ns >= 1000000000LL) {
                return fmtSeconds(ns, out, outSz);
            }
            if (ns >= 1000000LL) {
                return fmtMillis(ns, out, outSz);
            }
            if (ns >= 1000LL) {
                return fmtMicros(ns, out, outSz);
            }
            return fmtNanos(ns, out, outSz);
        }

        /**
         * @brief One-time initialization of the elapsed-time formatter.
         *
         * Reads SCOPE_TIMER_FORMAT (case-insensitive). If it matches
         * SECONDS/MILLIS/MICROS/NANOS, picks the corresponding formatter; otherwise
         * defaults to the auto formatter.
         */
        static inline FormatterFn initFormatter() noexcept {
            if (const char* env = std::getenv("SCOPE_TIMER_FORMAT"); env && *env) {
                std::string s(env);
                std::transform(s.begin(), s.end(), s.begin(),
                            [](unsigned char c){ return static_cast<char>(std::toupper(c)); });

                if (s == "SECONDS") return &fmtSeconds;
                if (s == "MILLIS")  return &fmtMillis;
                if (s == "MICROS")  return &fmtMicros;
                if (s == "NANOS")   return &fmtNanos;
            }
            return &fmtAuto;
        }        

        /**
         * @brief Accessor for the cached formatter function pointer.
         *
         * This ensures I pay the environment parsing cost once; no per-call branching
         * occurs on the hot path - I just call through the function pointer.
         */
        static inline FormatterFn& getFormatter() noexcept {
            static FormatterFn fn = initFormatter();
            return fn;
        }

        // Small helpers for fast formatting (avoid snprintf).
        static inline char* appendUnsignedToBuffer(char* out, const char* end, unsigned long long v) noexcept {
            std::array<char, 32> tmp{};
            int pos = 0;
            do {
                tmp[static_cast<std::size_t>(pos)] = static_cast<char>('0' + (v % 10ULL));
                ++pos;
                v /= 10ULL;
            } while (v && pos < static_cast<int>(tmp.size()));
            if (static_cast<std::size_t>(end - out) < static_cast<std::size_t>(pos)) {
                return nullptr;
            }
            while (pos--) {
                *out++ = tmp[static_cast<std::size_t>(pos)];
            }
            return out;
        }

        static inline char* appendThreeDigitsToBuffer(char* out, const char* end, unsigned v) noexcept {
            const std::array<char, 3> digits{
                static_cast<char>('0' + ((v / 100U) % 10U)),
                static_cast<char>('0' + ((v / 10U) % 10U)),
                static_cast<char>('0' + (v % 10U))
            };
            if (static_cast<std::size_t>(end - out) < digits.size()) {
                return nullptr;
            }
            std::memcpy(out, digits.data(), digits.size());
            return out + digits.size();
        }

        static inline bool appendCharToBuffer(char*& out, const char* end, char c) noexcept {
            if (out >= end) return false;
            *out++ = c;
            return true;
        }

        static inline bool appendStrToBuffer(char*& out, const char* end, const char* s, std::size_t n) noexcept {
            if (static_cast<std::size_t>(end - out) < n) return false;
            std::memcpy(out, s, n);
            out += n;
            return true;
        }

        static inline std::size_t formatElapsedImpl(unsigned long long whole, unsigned frac3, char unit, char* out, size_t outSz) noexcept {
            char* cur = out;
            const char* end = out + outSz;

            if (auto* next = appendUnsignedToBuffer(cur, end, whole)) {
                cur = next;
            } else {
                if (outSz) out[0] = '\0';
                return 0;
            }

            if (!appendCharToBuffer(cur, end, '.')) {
                if (outSz) {
                    out[0] = '\0';
                }
                return 0;
            }

            if (auto* next = appendThreeDigitsToBuffer(cur, end, frac3)) {
                cur = next;
            } else {
                if (outSz) out[0] = '\0';
                return 0;
            }

            const char suffix1 = unit;
            const char suffix2 = (unit == 's') ? '\0' : 's'; // "s" for seconds, "ms"/"us"
            if (!appendCharToBuffer(cur, end, suffix1)) {
                if (outSz) {
                    out[0] = '\0';
                }
                return 0;
            }
            if (suffix2 && !appendCharToBuffer(cur, end, suffix2)) {
                if (outSz) {
                    out[0] = '\0';
                }
                return 0;
            }

            if (cur < end) {
                *cur = '\0';
            } else if (outSz) {
                out[outSz - 1] = '\0';
            }
            return static_cast<std::size_t>(cur - out);
        }

        /**
         * @brief Formats elapsed time using a one-time-selected formatter.
         *
         * The formatter is chosen once from SCOPE_TIMER_FORMAT ("SECONDS" | "MILLIS" | "NANOS"),
         * defaulting to automatic behavior if unset/invalid.
         *
         * @param ns   Elapsed time in nanoseconds.
         * @param out  Output buffer.
         * @param outSz Size of the output buffer.
         */
        static inline std::size_t formatElapsed(long long ns, char* out, size_t outSz) noexcept {
            return getFormatter()(ns, out, outSz);
        }

        static inline std::size_t formatElapsedNanos(long long ns, char* out, size_t outSz) noexcept {
            return fmtNanos(ns, out, outSz);
        }

        static inline void appendBytesTruncating(char*& out, const char* end, const char* s, std::size_t n) noexcept {
            if (n == 0 || out >= end) {
                return;
            }
            const auto available = static_cast<std::size_t>(end - out);
            const auto toCopy = std::min(available, n);
            std::memcpy(out, s, toCopy);
            out += toCopy;
        }

        static inline void appendCharTruncating(char*& out, const char* end, char c) noexcept {
            if (out < end) {
                *out++ = c;
            }
        }

        /**
         * @brief Formats a system_clock time_point into a human-readable timestamp string.
         *
         * The format is "YYYY-MM-DD HH:MM:SS.mmm" with millisecond precision.
         * The calendar portion is cached per-thread for the current second.
         */
        static inline std::size_t formatTime(std::chrono::system_clock::time_point tp, char* out, size_t outSz) noexcept {
            if (outSz == 0) {
                return 0;
            }

            const std::time_t tt = std::chrono::system_clock::to_time_t(tp);
            const auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
            const auto ms3 = static_cast<unsigned>(ms_since_epoch % 1000);

            struct TimestampPrefixCache {
                std::time_t second{-1};
                std::array<char, 20> prefix{};
                std::uint8_t len{0};
            };
            thread_local TimestampPrefixCache cache;

            if (cache.second != tt) {
                auto tm = std::tm{};
                LOCALTIME(&tm, &tt);

                char* prefixOut = cache.prefix.data();
                if (const char* prefixEnd = cache.prefix.data() + cache.prefix.size() - 1U;
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(1900 + tm.tm_year), 4) ||
                    !appendCharToBuffer(prefixOut, prefixEnd, '-') ||
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(1 + tm.tm_mon), 2) ||
                    !appendCharToBuffer(prefixOut, prefixEnd, '-') ||
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(tm.tm_mday), 2) ||
                    !appendCharToBuffer(prefixOut, prefixEnd, ' ') ||
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(tm.tm_hour), 2) ||
                    !appendCharToBuffer(prefixOut, prefixEnd, ':') ||
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(tm.tm_min), 2) ||
                    !appendCharToBuffer(prefixOut, prefixEnd, ':') ||
                    !appendFixedDigits(prefixOut, prefixEnd, static_cast<unsigned>(tm.tm_sec), 2)) {
                    out[0] = '\0';
                    return 0;
                }
                *prefixOut = '\0';
                cache.second = tt;
                cache.len = static_cast<std::uint8_t>(prefixOut - cache.prefix.data());
            }

            char* cur = out;
            const char* end = out + outSz - 1U;
            appendBytesTruncating(cur, end, cache.prefix.data(), cache.len);
            appendCharTruncating(cur, end, '.');
            if (auto* next = appendThreeDigitsToBuffer(cur, end, ms3)) {
                cur = next;
            }
            *cur = '\0';
            return static_cast<std::size_t>(cur - out);
        }

        static inline void appendUnsignedTruncating(char*& out, const char* end, unsigned long long v) noexcept {
            std::array<char, 32> tmp{};
            const auto result = std::to_chars(tmp.data(), tmp.data() + tmp.size(), v);
            if (result.ec != std::errc{}) {
                return;
            }
            appendBytesTruncating(out, end, tmp.data(), static_cast<std::size_t>(result.ptr - tmp.data()));
        }

        static inline void appendThreadIdTruncating(char*& out, const char* end, unsigned tid) noexcept {
            if (tid < 1000U) {
                const std::array<char, 3> digits{
                    static_cast<char>('0' + ((tid / 100U) % 10U)),
                    static_cast<char>('0' + ((tid / 10U) % 10U)),
                    static_cast<char>('0' + (tid % 10U))
                };
                appendBytesTruncating(out, end, digits.data(), digits.size());
                return;
            }
            appendUnsignedTruncating(out, end, tid);
        }

        struct LogLineFields {
            std::string_view label;
            unsigned threadNum{0};
            std::string_view where;
            std::string_view startWall;
            std::string_view endWall;
            std::string_view elapsed;
            bool wallTimeEnabled{false};
        };

        static inline std::size_t buildLogLine(
            char* out,
            std::size_t outSz,
            const LogLineFields& fields
        ) noexcept {
            if (outSz == 0) {
                return 0;
            }

            char* cur = out;
            const char* end = out + outSz - 1U; // reserve a byte for a terminator

            appendCharTruncating(cur, end, '[');
            appendBytesTruncating(cur, end, fields.label.data(), fields.label.size());
            appendBytesTruncating(cur, end, "] TID=", sizeof("] TID=") - 1U);
            appendThreadIdTruncating(cur, end, fields.threadNum);
            appendBytesTruncating(cur, end, " | ", sizeof(" | ") - 1U);
            appendBytesTruncating(cur, end, fields.where.data(), fields.where.size());
            if (fields.wallTimeEnabled) {
                appendBytesTruncating(cur, end, " | start=", sizeof(" | start=") - 1U);
                appendBytesTruncating(cur, end, fields.startWall.data(), fields.startWall.size());
                appendBytesTruncating(cur, end, " | end=", sizeof(" | end=") - 1U);
                appendBytesTruncating(cur, end, fields.endWall.data(), fields.endWall.size());
            }
            appendBytesTruncating(cur, end, " | elapsed=", sizeof(" | elapsed=") - 1U);
            appendBytesTruncating(cur, end, fields.elapsed.data(), fields.elapsed.size());
            appendCharTruncating(cur, end, '\n');

            *cur = '\0';
            return static_cast<std::size_t>(cur - out);
        }

        static inline std::size_t buildHotPathLogLine(
            char* out,
            std::size_t outSz,
            std::string_view label,
            const char* elapsed,
            std::size_t elapsedLen
        ) noexcept {
            if (outSz == 0) {
                return 0;
            }

            char* cur = out;
            const char* end = out + outSz - 1U;
            appendCharTruncating(cur, end, '[');
            appendBytesTruncating(cur, end, label.data(), label.size());
            appendBytesTruncating(cur, end, "] elapsed=", sizeof("] elapsed=") - 1U);
            appendBytesTruncating(cur, end, elapsed, elapsedLen);
            appendCharTruncating(cur, end, '\n');

            *cur = '\0';
            return static_cast<std::size_t>(cur - out);
        }

        /**
         * @brief Per-thread scratch space for formatted end/elapsed timestamps.
         */
        struct FormatBuffers {
            char endBuf[32];
            char elapsedBuf[32];
            std::uint8_t endLen;
            std::uint8_t elapsedLen;
        };

        /**
         * @brief Returns the thread-local formatting scratch space.
         */
        static inline FormatBuffers& formatBuffers() noexcept {
            return tlsFormatBuffers_;
        }

        /**
         * @brief Helper used by tests to ensure format buffers are reused.
         */
        static inline const char* endBufferAddressForTests() noexcept {
            return formatBuffers().endBuf;
        }

        /**
         * @brief Per-thread assembly buffer for the full log line.
         */
        struct LineBuffer {
            char data[512];
        };

        /**
         * @brief Accessor for the thread-local line assembly buffer.
         */
        static inline LineBuffer& lineBuffer() noexcept {
            return tlsLineBuffer_;
        }

        /**
         * @brief Per-thread assembly buffer for the full log line.
         */
        static void defaultSinkWrite(const char* data, std::size_t len) noexcept;
        static void defaultSinkFlush() noexcept;
        static void noopSinkFlush() noexcept;
        static void threadBufferedSinkWrite(const char* data, std::size_t len) noexcept;
        static void threadBufferedSinkFlush() noexcept;
        static void asyncSinkWrite(const char* data, std::size_t len) noexcept;
        static void asyncSinkFlush() noexcept;

        enum class ActiveSink {
            Default,
            ThreadBuffered,
            Custom,
        };

        enum class BufferedSinkTargetMode {
            Default,
            Async,
            Custom,
            TestCustom,
        };

        enum class BufferedSinkFlushMode {
            Deferred,
            Forced,
        };

        enum class AsyncSinkTargetMode {
            Default,
            Custom,
        };

        struct ThreadBufferState {
            std::mutex flushMutex;
            std::vector<char> data;
            std::size_t size{0U};
            std::size_t capacity{0U};
        };

        struct ThreadBufferHandle {
            std::shared_ptr<ThreadBufferState> state{std::make_shared<ThreadBufferState>()};

            ThreadBufferHandle() {
                registerThreadBuffer(state);
            }

            ~ThreadBufferHandle() {
                ScopeTimer::flushThreadBuffer(*state);
            }

            ThreadBufferHandle(const ThreadBufferHandle&) = delete;
            ThreadBufferHandle& operator=(const ThreadBufferHandle&) = delete;
            ThreadBufferHandle(ThreadBufferHandle&&) = delete;
            ThreadBufferHandle& operator=(ThreadBufferHandle&&) = delete;
        };

        static inline ThreadBufferState& threadLocalBuffer() noexcept {
            thread_local ThreadBufferHandle handle;
            return *handle.state;
        }
        static inline std::atomic<std::size_t>& threadBufferFlushBytesStorage() noexcept {
            return detail::singletonStorage<detail::ThreadBufferFlushBytesTag, std::atomic<std::size_t>>(16U * 1024U);
        }
        static inline std::size_t threadBufferFlushBytes() noexcept {
            // The threshold is configuration state written under sinkConfigMutex(); a
            // relaxed load is sufficient and avoids an unnecessary fence per append.
            return threadBufferFlushBytesStorage().load(std::memory_order_relaxed);
        }
        static inline std::mutex& threadBufferRegistryMutex() noexcept {
            return detail::singletonStorage<detail::ThreadBufferRegistryMutexTag, std::mutex>();
        }
        static inline std::vector<std::weak_ptr<ThreadBufferState>>& threadBufferRegistry() noexcept {
            return detail::singletonStorage<detail::ThreadBufferRegistryTag, std::vector<std::weak_ptr<ThreadBufferState>>>();
        }
        static inline void registerThreadBuffer(const std::shared_ptr<ThreadBufferState>& state) noexcept {
            std::lock_guard lock(threadBufferRegistryMutex());
            threadBufferRegistry().emplace_back(state);
        }
        static inline std::vector<std::shared_ptr<ThreadBufferState>> snapshotThreadBuffers() noexcept {
            std::vector<std::shared_ptr<ThreadBufferState>> states;
            std::lock_guard lock(threadBufferRegistryMutex());
            auto& registry = threadBufferRegistry();
            registry.erase(
                std::remove_if(registry.begin(), registry.end(), [&states](const auto& weakState) {
                    if (auto state = weakState.lock()) {
                        states.push_back(state);
                        return false;
                    }
                    return true;
                }),
                registry.end()
            );
            return states;
        }
        static inline void ensureThreadBufferCapacity(ThreadBufferState& state, std::size_t flushBytes) noexcept {
            if (state.capacity >= flushBytes) {
                return;
            }

            std::lock_guard lock(state.flushMutex);
            if (state.capacity >= flushBytes) {
                return;
            }

            state.data.resize(flushBytes);
            state.capacity = flushBytes;
        }
        static inline bool bufferedSinkTargetNeedsLock(BufferedSinkTargetMode mode) noexcept {
            return mode != BufferedSinkTargetMode::Async;
        }
        static inline std::size_t drainThreadBuffer(ThreadBufferState& state, const char*& data) noexcept {
            // Buffered sink mode changes are documented as setup/teardown steps.
            // Callers are expected to quiesce profiled worker threads before a
            // cross-thread flush such as disable/atexit walks the registry.
            std::lock_guard lock(state.flushMutex);
            if (state.size == 0U) {
                data = nullptr;
                return 0U;
            }

            data = state.data.data();
            const std::size_t len = state.size;
            state.size = 0U;
            return len;
        }
        static inline void writeBufferedSinkPayload(
            BufferedSinkTargetMode mode,
            const char* data,
            std::size_t len
        ) noexcept {
            if (len == 0U) {
                return;
            }

            writeToBufferedSinkTarget(mode, data, len);
        }
        static inline void publishBufferedSinkPayload(
            const char* data,
            std::size_t len,
            BufferedSinkFlushMode flushMode = BufferedSinkFlushMode::Deferred
        ) noexcept {
            if (len == 0U && flushMode == BufferedSinkFlushMode::Deferred) {
                return;
            }

            const auto bufferedTarget = bufferedSinkTargetModeStorage().load(std::memory_order_acquire);
            if (bufferedSinkTargetNeedsLock(bufferedTarget)) {
                std::lock_guard lock(outMutex());
                writeBufferedSinkPayload(bufferedTarget, data, len);
                if (flushMode == BufferedSinkFlushMode::Forced) {
                    flushBufferedSinkTarget(bufferedTarget);
                }
                return;
            }

            writeBufferedSinkPayload(bufferedTarget, data, len);
            if (flushMode == BufferedSinkFlushMode::Forced) {
                flushBufferedSinkTarget(bufferedTarget);
            }
        }
        static inline void flushThreadBuffer(
            ThreadBufferState& state,
            BufferedSinkFlushMode flushMode = BufferedSinkFlushMode::Deferred
        ) noexcept {
            const char* pendingData = nullptr;
            const std::size_t pendingLen = drainThreadBuffer(state, pendingData);
            publishBufferedSinkPayload(pendingData, pendingLen, flushMode);
        }
        static inline void flushAllThreadBuffers(
            BufferedSinkFlushMode flushMode = BufferedSinkFlushMode::Forced
        ) noexcept {
            const auto states = snapshotThreadBuffers();
            const auto bufferedTarget = bufferedSinkTargetModeStorage().load(std::memory_order_acquire);
            const bool needsLock = bufferedSinkTargetNeedsLock(bufferedTarget);
            std::unique_lock sinkLock(outMutex(), std::defer_lock);
            if (needsLock) {
                sinkLock.lock();
            }

            for (const auto& state : states) {
                const char* pendingData = nullptr;
                const std::size_t pendingLen = drainThreadBuffer(*state, pendingData);
                writeBufferedSinkPayload(bufferedTarget, pendingData, pendingLen);
            }

            if (flushMode == BufferedSinkFlushMode::Forced) {
                flushBufferedSinkTarget(bufferedTarget);
            }
        }
        static inline std::mutex& sinkConfigMutex() noexcept {
            return detail::singletonStorage<detail::SinkConfigMutexTag, std::mutex>();
        }
        static inline void restoreDefaultSinkState() noexcept {
            activeSinkStorage().store(hasCustomSink() ? ActiveSink::Custom : ActiveSink::Default,
                                      std::memory_order_release);
            bufferedSinkTargetModeStorage().store(hasCustomSink() ? BufferedSinkTargetMode::Custom
                                                                  : BufferedSinkTargetMode::Default,
                                                  std::memory_order_release);
            asyncSinkTargetModeStorage().store(hasCustomSink() ? AsyncSinkTargetMode::Custom
                                                               : AsyncSinkTargetMode::Default,
                                               std::memory_order_release);
        }

        static inline std::atomic<ActiveSink>& activeSinkStorage() noexcept {
            return detail::singletonStorage<detail::ActiveSinkStorageTag, std::atomic<ActiveSink>>(ActiveSink::Default);
        }
        static inline std::atomic<BufferedSinkTargetMode>& bufferedSinkTargetModeStorage() noexcept {
            return detail::singletonStorage<detail::BufferedSinkTargetModeStorageTag, std::atomic<BufferedSinkTargetMode>>(BufferedSinkTargetMode::Default);
        }
        static inline std::atomic<AsyncSinkTargetMode>& asyncSinkTargetModeStorage() noexcept {
            return detail::singletonStorage<detail::AsyncSinkTargetModeStorageTag, std::atomic<AsyncSinkTargetMode>>(AsyncSinkTargetMode::Default);
        }
        static inline std::function<void(const char*, std::size_t)>& customSinkWriteStorage() {
            return detail::singletonStorage<detail::CustomSinkWriteStorageTag, std::function<void(const char*, std::size_t)>>();
        }
        static inline std::function<void()>& customSinkFlushStorage() {
            return detail::singletonStorage<detail::CustomSinkFlushStorageTag, std::function<void()>>();
        }
        static inline std::function<void(const char*, std::size_t)>& bufferedTestSinkWriteStorage() {
            return detail::singletonStorage<detail::BufferedTestSinkWriteStorageTag, std::function<void(const char*, std::size_t)>>();
        }
        static inline bool hasCustomSink() {
            return static_cast<bool>(customSinkWriteStorage());
        }
        static inline void writeToCustomSink(const char* data, std::size_t len) noexcept {
            if (const auto& writeFn = customSinkWriteStorage(); writeFn) {
                writeFn(data, len);
            }
        }
        static inline void flushCustomSink() noexcept {
            if (const auto& flushFn = customSinkFlushStorage(); flushFn) {
                flushFn();
            }
        }
        static inline void writeToActiveSink(ActiveSink sink, const char* data, std::size_t len) noexcept {
            switch (sink) {
                case ActiveSink::ThreadBuffered:
                    threadBufferedSinkWrite(data, len);
                    break;
                case ActiveSink::Custom:
                    writeToCustomSink(data, len);
                    break;
                case ActiveSink::Default:
                    defaultSinkWrite(data, len);
                    break;
            }
        }
        static inline void flushActiveSink(ActiveSink sink) noexcept {
            switch (sink) {
                case ActiveSink::ThreadBuffered:
                    threadBufferedSinkFlush();
                    break;
                case ActiveSink::Custom:
                    flushCustomSink();
                    break;
                case ActiveSink::Default:
                    defaultSinkFlush();
                    break;
            }
        }
        static inline void writeToBufferedSinkTarget(
            BufferedSinkTargetMode mode,
            const char* data,
            std::size_t len
        ) noexcept {
            switch (mode) {
                case BufferedSinkTargetMode::Async:
                    asyncSinkWrite(data, len);
                    break;
                case BufferedSinkTargetMode::Custom:
                    writeToCustomSink(data, len);
                    break;
                case BufferedSinkTargetMode::TestCustom:
                    if (const auto& writeFn = bufferedTestSinkWriteStorage(); writeFn) {
                        writeFn(data, len);
                    }
                    break;
                case BufferedSinkTargetMode::Default:
                    defaultSinkWrite(data, len);
                    break;
            }
        }
        static inline void flushBufferedSinkTarget(BufferedSinkTargetMode mode) noexcept {
            switch (mode) {
                case BufferedSinkTargetMode::Async:
                    noopSinkFlush();
                    break;
                case BufferedSinkTargetMode::Custom:
                    flushCustomSink();
                    break;
                case BufferedSinkTargetMode::TestCustom:
                    noopSinkFlush();
                    break;
                case BufferedSinkTargetMode::Default:
                    defaultSinkFlush();
                    break;
            }
        }

        struct AsyncSinkBatch {
            std::vector<char> data;
            std::size_t size{0U};
        };

        struct AsyncSinkState {
            std::mutex mutex;
            std::condition_variable ready;
            std::condition_variable drained;
            std::deque<AsyncSinkBatch> queue;
            std::vector<AsyncSinkBatch> recycled;
            std::thread worker;
            bool running{false};
            bool stop{false};
            bool writing{false};
        };

        static inline AsyncSinkState& asyncSinkState() noexcept {
            return detail::singletonStorage<detail::AsyncSinkStateTag, AsyncSinkState>();
        }
        static inline AsyncSinkBatch acquireAsyncSinkBatch(std::size_t len) noexcept {
            AsyncSinkBatch batch;
            auto& state = asyncSinkState();
            {
                std::lock_guard lock(state.mutex);
                if (!state.recycled.empty()) {
                    batch = std::move(state.recycled.back());
                    state.recycled.pop_back();
                }
            }

            if (batch.data.size() < len) {
                batch.data.resize(len);
            }
            batch.size = len;
            return batch;
        }
        static inline void defaultSinkWriteBatches(const std::deque<AsyncSinkBatch>& batches) noexcept {
#if !defined(_WIN32)
            constexpr std::size_t MaxIovecs = 64U;
            std::array<::iovec, MaxIovecs> iovecs{};
            std::size_t count = 0U;

            int fd = logFd();
            if (fd < 0) {
                if (!ensureLogFdOpen()) {
                    return;
                }
                fd = logFd();
                if (fd < 0) {
                    return;
                }
            }

            for (const auto& batch : batches) {
                if (batch.size == 0U) {
                    continue;
                }

                // POSIX writev() exposes iov_base as mutable even for write-only buffers.
                // The payload bytes remain immutable; this cast only adapts the API's
                // legacy type mismatch.
                iovecs[count].iov_base = const_cast<char*>(batch.data.data()); // NOSONAR: writev() reads from iov_base but declares it mutable.
                iovecs[count].iov_len = batch.size;
                ++count;

                if (count == iovecs.size()) {
                    const ssize_t unused = ::writev(fd, iovecs.data(), static_cast<int>(count));
                    (void)unused;
                    count = 0U;
                }
            }

            if (count != 0U) {
                const ssize_t unused = ::writev(fd, iovecs.data(), static_cast<int>(count));
                (void)unused;
            }
#else
            for (const auto& batch : batches) {
                defaultSinkWrite(batch.data.data(), batch.size);
            }
#endif
        }
        static inline void runAsyncSinkWorker() noexcept {
            auto& workerState = asyncSinkState();
            for (;;) {
                std::deque<AsyncSinkBatch> pending;
                {
                    std::unique_lock lock(workerState.mutex);
                    workerState.ready.wait(lock, [&workerState] {
                        return workerState.stop || !workerState.queue.empty();
                    });
                    if (workerState.queue.empty()) {
                        if (workerState.stop) {
                            break;
                        }
                        continue;
                    }
                    pending.swap(workerState.queue);
                    workerState.writing = true;
                }

                switch (asyncSinkTargetModeStorage().load(std::memory_order_acquire)) {
                    case AsyncSinkTargetMode::Custom:
                        for (const auto& batch : pending) {
                            writeToCustomSink(batch.data.data(), batch.size);
                        }
                        break;
                    case AsyncSinkTargetMode::Default:
                        defaultSinkWriteBatches(pending);
                        break;
                }

                {
                    std::lock_guard lock(workerState.mutex);
                    for (auto& batch : pending) {
                        batch.size = 0U;
                        workerState.recycled.emplace_back(std::move(batch));
                    }
                    workerState.writing = false;
                    if (workerState.queue.empty()) {
                        workerState.drained.notify_all();
                    }
                }
            }
            switch (asyncSinkTargetModeStorage().load(std::memory_order_acquire)) {
                case AsyncSinkTargetMode::Custom:
                    flushCustomSink();
                    break;
                case AsyncSinkTargetMode::Default:
                    defaultSinkFlush();
                    break;
            }
        }

        static inline void ensureAsyncSinkRunning() noexcept {
            auto& state = asyncSinkState();
            std::lock_guard lock(state.mutex);
            if (state.running) {
                return;
            }
            state.stop = false;
            state.writing = false;
            state.running = true;
            state.worker = std::thread([] { runAsyncSinkWorker(); });
        }

        static inline void shutdownAsyncSink() noexcept {
            auto& state = asyncSinkState();
            std::unique_lock lock(state.mutex);
            if (!state.running) {
                return;
            }
            state.stop = true;
            lock.unlock();
            state.ready.notify_all();
            if (state.worker.joinable()) {
                state.worker.join();
            }
            lock.lock();
            state.stop = false;
            state.running = false;
            state.writing = false;
        }

        // Test-only sink swap; safe when called during single-threaded setup/teardown.
        static inline void setLogSinkForTests(
            std::function<void(const char*, std::size_t)> writeFn = {},
            std::function<void()> flushFn = {}
        ) {
            setCustomSinkCallbacks(std::move(writeFn), std::move(flushFn));
        }

        static inline void setCustomSinkCallbacks(
            std::function<void(const char*, std::size_t)> writeFn = {},
            std::function<void()> flushFn = {}
        ) {
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            closeLogFd();

            const bool asyncModeActive = activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::ThreadBuffered
                && bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async;

            customSinkWriteStorage() = std::move(writeFn);
            customSinkFlushStorage() = flushFn ? std::move(flushFn) : std::function<void()>{};

            if (hasCustomSink()) {
                if (activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::Default) {
                    activeSinkStorage().store(ActiveSink::Custom, std::memory_order_release);
                }
                if (bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Default) {
                    bufferedSinkTargetModeStorage().store(BufferedSinkTargetMode::Custom, std::memory_order_release);
                }
                asyncSinkTargetModeStorage().store(AsyncSinkTargetMode::Custom, std::memory_order_release);
            } else {
                if (activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::Custom) {
                    activeSinkStorage().store(ActiveSink::Default, std::memory_order_release);
                }
                if (bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Custom) {
                    bufferedSinkTargetModeStorage().store(BufferedSinkTargetMode::Default, std::memory_order_release);
                }
                asyncSinkTargetModeStorage().store(AsyncSinkTargetMode::Default, std::memory_order_release);
            }

            if (asyncModeActive) {
                ensureAsyncSinkRunning();
            }
        }

        // Test-only: allow buffered sink to flush to a custom target (e.g., in-memory sink).
        static inline void setBufferedSinkTargetForTests(
            std::function<void(const char*, std::size_t)> writeFn = {}
        ) {
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            bufferedTestSinkWriteStorage() = std::move(writeFn);
            bufferedSinkTargetModeStorage().store(
                bufferedTestSinkWriteStorage() ? BufferedSinkTargetMode::TestCustom : BufferedSinkTargetMode::Default,
                std::memory_order_release
            );
        }

        inline void assignLabel(detail::LabelData data) noexcept {
            const std::string_view source = !data.storage.empty() ? std::string_view{data.storage} : data.view;
            if (source.empty()) {
                label_ = "ScopeTimer";
                return;
            }

            if (data.canBorrowView()) {
                label_ = source;
                labelHeapStorage_.clear();
                return;
            }

            if (source.size() < labelBuffer_.size()) {
                std::memcpy(labelBuffer_.data(), source.data(), source.size());
                label_ = std::string_view{labelBuffer_.data(), source.size()};
                labelHeapStorage_.clear();
            } else {
                labelHeapStorage_ = !data.storage.empty() ? std::move(data.storage) : std::string(source);
                label_ = labelHeapStorage_;
            }
        }

        std::string_view where_; ///< Description of the scope being timed.
        std::string_view label_{ "ScopeTimer" }; ///< Label for the log output.
        std::array<char, 128> labelBuffer_{};
        std::string labelHeapStorage_;
        uint32_t threadNum_{0}; ///< Unique thread ID number.

        static inline thread_local FormatBuffers tlsFormatBuffers_{};
        static inline thread_local LineBuffer tlsLineBuffer_{};
        static inline std::string logDirCache_{"/tmp/"};
        static inline bool logDirInitialized_{false};

        /**
         * @brief Opens the default log file descriptor on first use (best-effort).
         */
        static inline bool ensureLogFdOpen() noexcept {
            int& fd = logFd();
            if (fd >= 0) {
                return true;
            }

            static std::string lastFailedPath;
            static bool lastAttemptFailed = false;

            const std::string path = logDirectory() + "ScopeTimer.log";

            if (lastAttemptFailed && path == lastFailedPath) {
                return false;
            }

            int openFlags = O_CREAT | O_WRONLY | O_APPEND;
#ifdef O_CLOEXEC
            openFlags |= O_CLOEXEC;
#endif

            if (int newFd = ::open(path.c_str(), openFlags, 0600); newFd >= 0) {
#ifndef O_CLOEXEC
                (void)::fcntl(newFd, F_SETFD, FD_CLOEXEC);
#endif
                fd = newFd;
                lastAttemptFailed = false;
                lastFailedPath.clear();
                registerLogFdCleanup();
                return true;
            }

            lastFailedPath = path;
            lastAttemptFailed = true;
            return false;
        }

        /**
         * @brief Registers the atexit handler that closes the log descriptor.
         */
        static inline void registerLogFdCleanup() noexcept {
            static bool registered = false;
            if (!registered) {
                std::atexit([]() noexcept {
                    std::lock_guard sinkStateLock(sinkConfigMutex());
                    flushAllThreadBuffers();
                    asyncSinkFlush();
                    shutdownAsyncSink();
                    closeLogFd();
                });
                registered = true;
            }
        }

        /**
         * @brief Singleton storage for the log descriptor.
         */
        static inline int logFdStorage_{-1};
        static inline int& logFd() noexcept {
            return logFdStorage_;
        }

        /**
         * @brief Resets the log descriptor so it will be reopened on demand.
         */
        static inline void closeLogFd() noexcept {
            int& fd = logFd();
            if (fd >= 0) {
                ::close(fd);
                fd = -1;
            }
        }

        /**
         * @brief Test-only accessor to observe the current log descriptor.
         */
        static inline int defaultLogFdForTests() noexcept {
            return logFd();
        }

        /**
         * @brief Test-only helper that forces the log descriptor closed.
         */
        static inline void closeLogFdForTests() noexcept {
            closeLogFd();
        }

        static inline bool labelUsesLocalBufferForTests(const ScopeTimer& timer) noexcept {
            const char* ptr = timer.label_.data();
            const char* begin = timer.labelBuffer_.data();
            const char* end = begin + timer.labelBuffer_.size();
            return ptr >= begin && ptr < end;
        }

        static inline bool labelUsesHeapForTests(const ScopeTimer& timer) noexcept {
            return !timer.labelHeapStorage_.empty() && timer.label_.data() == timer.labelHeapStorage_.data();
        }

        static inline bool labelUsesBorrowedStorageForTests(const ScopeTimer& timer) noexcept {
            return !labelUsesLocalBufferForTests(timer) && !labelUsesHeapForTests(timer);
        }

        /**
         * I store both a steady_clock (startSteady_) and a system_clock (startWall_) timestamp:
         * - startSteady_: Used for computing precise elapsed durations, immune to system clock changes.
         * - startWall_: Used for logging human-readable absolute start times for contextual information.
         * This dual tracking ensures logs contain meaningful wall times while preserving accurate duration measurements.
         */
        static constexpr std::size_t TimestampBufSize = 32;

        std::chrono::steady_clock::time_point startSteady_; ///< Start time for high-resolution elapsed duration.

        /**
         * @brief Stores the real-world wall clock timestamp at timer start.
         *
         * This is useful for logging absolute times in human-readable form (e.g., for log file timestamps).
         * Unlike startSteady_ (which uses steady_clock for precise elapsed timing unaffected by system clock changes),
         * startWall_ captures the actual wall clock time for contextual/logging purposes.
         */
        std::chrono::system_clock::time_point startWall_;
        char startWallFormatted_[TimestampBufSize]{};
        std::uint8_t startWallFormattedLen_{0};

        /**
         * @brief Indicates if this timer instance is disabled.
         *
         * In Debug builds, this is set to true if the SCOPE_TIMER environment variable disables timing
         * (i.e., set to "OFF", "FALSE", "NO", or "0", case-insensitive). This check is performed once
         * in the constructor to avoid repeated environment variable lookups for every timing event.
         *
         * In Release builds, this field is always unused and the timer is a no-op.
         *
         * This mechanism allows runtime control of timing/logging overhead in Debug builds, while ensuring
         * no performance impact in Release builds.
         */
        bool disabled_{ false };
        bool hotPathMode_{ false };
    };

    namespace detail {
        class ConditionalScopeTimer {
        public:
            template <typename LabelFactory>
            ConditionalScopeTimer(bool enabled, std::string_view where, LabelFactory&& labelFactory) noexcept {
                if (enabled) {
                    timer_.emplace(where, labelFactory());
                } else {
                    (void)where;
                }
            }

            ~ConditionalScopeTimer() = default;
            ConditionalScopeTimer(const ConditionalScopeTimer&) = delete;
            ConditionalScopeTimer& operator=(const ConditionalScopeTimer&) = delete;
            ConditionalScopeTimer(ConditionalScopeTimer&&) = delete;
            ConditionalScopeTimer& operator=(ConditionalScopeTimer&&) = delete;

        private:
            std::optional<ScopeTimer> timer_;
        };
    } // namespace detail


// -----------------------------------------------------------------------------
// Macro helpers for generating unique variable names inside macros
//
// Why two-level CAT?  The token-pasting operator (##) does *not* expand its
// arguments before pasting. I therefore use a two-step expansion so that
// macro arguments like __LINE__ or __COUNTER__ are expanded first, then pasted.
//
// Why __COUNTER__?  __LINE__ can collide if multiple macro expansions occur on
// the same source line (e.g., via a macro that emits multiple SCOPE_TIMER calls).
// __COUNTER__ is a monotonically increasing integer within a translation unit,
// so each expansion gets a distinct number regardless of line, creating a
// globally unique identifier for the variable name in this TU. If __COUNTER__
// is not available, I fall back to __LINE__.
// -----------------------------------------------------------------------------
#define ST_CAT2(a, b) a##b
#define ST_CAT(a, b)  ST_CAT2(a, b)

#if defined(__COUNTER__)
#  define ST_UNIQ __COUNTER__
#else
#  define ST_UNIQ __LINE__
#endif


#ifndef SCOPE_TIMER
#define SCOPE_TIMER(...)                                                             \
    ::xyzzy::scopetimer::ScopeTimer ST_CAT(scopeTimerInstance__, ST_UNIQ)( \
        SCOPE_FUNCTION, ::xyzzy::scopetimer::detail::makeLabelData(__VA_ARGS__))
#endif

/**
 * @brief Conditionally starts a ScopeTimer for the current scope.
 *
 * This macro is similar to SCOPE_TIMER(...) but will only create the timer
 * and record timing information if the provided boolean condition evaluates to true.
 *
 * @param cond Boolean expression; if true, timing starts for this scope.
 * @param ...  Optional label string for the log entry.
 *
 * @note This macro is only active in debug builds (NDEBUG not defined). In release builds it compiles to a no-op.
 *
 * @code
 * void foo() {
 *     SCOPE_TIMER_IF(debugMode, "Debug block");
 *     // ... code ...
 * }
 * @endcode
 */
#ifndef SCOPE_TIMER_IF
#define SCOPE_TIMER_IF(cond, ...)                                                          \
    ::xyzzy::scopetimer::detail::ConditionalScopeTimer                                       \
        ST_CAT(scopeTimerConditional__, ST_UNIQ)((cond), SCOPE_FUNCTION, [&]() noexcept {  \
            return ::xyzzy::scopetimer::detail::makeLabelData(__VA_ARGS__);                  \
        })
#endif

#ifndef SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(...) \
    do { ::xyzzy::scopetimer::ScopeTimer::enableThreadBufferedSink(__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK() \
    do { ::xyzzy::scopetimer::ScopeTimer::disableThreadBufferedSink(); } while(0)
#endif

#ifndef SCOPE_TIMER_ENABLE_ASYNC_SINK
#define SCOPE_TIMER_ENABLE_ASYNC_SINK(...) \
    do { ::xyzzy::scopetimer::ScopeTimer::enableAsyncSink(__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_ASYNC_SINK
#define SCOPE_TIMER_DISABLE_ASYNC_SINK() \
    do { ::xyzzy::scopetimer::ScopeTimer::disableAsyncSink(); } while(0)
#endif

#ifndef SCOPE_TIMER_HOT_PATH
#define SCOPE_TIMER_HOT_PATH(...)                                                            \
    ::xyzzy::scopetimer::ScopeTimer ST_CAT(scopeTimerHotPathInstance__, ST_UNIQ)(            \
        ::xyzzy::scopetimer::ScopeTimer::HotPathTag{},                                       \
        ::xyzzy::scopetimer::detail::makeLabelData(__VA_ARGS__))
#endif

#else // Release build -> no-op

    /**
     * @brief No-op ScopeTimer class for release builds.
     *
     * Provides a matching interface to the debug ScopeTimer but performs no timing or logging.
     * This class uses C++17 std::string_view for parameter compatibility.
     */
    class ScopeTimer {
    public:
        class LogSink {
        public:
            virtual ~LogSink() = default;
            virtual void write(const char*, std::size_t) noexcept = 0;
            virtual void flush() noexcept {}
        };

        /**
         * @brief Constructs a no-op ScopeTimer.
         *
         * @param where Unused parameter describing the scope.
         * @param label Unused parameter for compatibility.
         */
        inline explicit ScopeTimer(std::string_view, std::string_view = "ScopeTimer") noexcept {}
        static inline void setLogSink(LogSink&) noexcept {}
        static inline void resetLogSink() noexcept {}
    };

 #ifndef SCOPE_TIMER
#define SCOPE_TIMER(...) \
    do { (void)sizeof(#__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_IF
// Do not evaluate 'cond' or variadic args (avoid side effects); silences unused warnings
#define SCOPE_TIMER_IF(cond, ...) \
    do { (void)sizeof(cond); (void)sizeof(#__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(...) \
    do { (void)sizeof(#__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK() \
    do { } while(0)
#endif

#ifndef SCOPE_TIMER_ENABLE_ASYNC_SINK
#define SCOPE_TIMER_ENABLE_ASYNC_SINK(...) \
    do { (void)sizeof(#__VA_ARGS__); } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_ASYNC_SINK
#define SCOPE_TIMER_DISABLE_ASYNC_SINK() \
    do { } while(0)
#endif

#ifndef SCOPE_TIMER_HOT_PATH
#define SCOPE_TIMER_HOT_PATH(...) \
    do { (void)sizeof(#__VA_ARGS__); } while(0)
#endif

#endif // NDEBUG

} // namespace xyzzy::scopetimer

#ifndef NDEBUG
inline void xyzzy::scopetimer::ScopeTimer::defaultSinkWrite(const char* data, std::size_t len) noexcept {
    // Fast path: nothing to write.
    if (len == 0) {
        return;
    }

    int fd = logFd();
    if (fd < 0) {
        // Attempt to open/create the log file lazily; if that fails we silently drop the line.
        if (!ensureLogFdOpen()) {
            return;
        }
        fd = logFd();
        if (fd < 0) {
            return;
        }
    }

    // write(2) can legitimately return fewer bytes than requested. Since ScopeTimer logging is
    // best-effort, we intentionally ignore the return code — we never want to throw or retry from here.
    ssize_t unused = ::write(fd, data, len); 
    (void)unused; // Explicitly mark the variable as "intentionally unused"
}

inline void xyzzy::scopetimer::ScopeTimer::defaultSinkFlush() noexcept {
    int fd = logFd();
    if (fd >= 0) {
#if defined(_POSIX_SYNCHRONIZED_IO) && _POSIX_SYNCHRONIZED_IO >= 0 && !defined(__APPLE__)
        ::fdatasync(fd);
#else
        ::fsync(fd);
#endif
    }
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
inline void xyzzy::scopetimer::ScopeTimer::noopSinkFlush() noexcept {
    // Intentionally blank: used when tests inject a sink but do not need flush semantics.
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline void xyzzy::scopetimer::ScopeTimer::threadBufferedSinkWrite(const char* data, std::size_t len) noexcept {
    if (len == 0) {
        return;
    }

    auto& buffer = threadLocalBuffer();
    const std::size_t flushBytes = threadBufferFlushBytes();
    ensureThreadBufferCapacity(buffer, flushBytes);

    if (len >= flushBytes) {
        flushThreadBuffer(buffer);
        publishBufferedSinkPayload(data, len);
        return;
    }

    if (buffer.size + len > flushBytes) {
        flushThreadBuffer(buffer);
    }

    std::memcpy(buffer.data.data() + buffer.size, data, len);
    buffer.size += len;
    if (buffer.size >= flushBytes) {
        flushThreadBuffer(buffer);
    }
}

inline void xyzzy::scopetimer::ScopeTimer::threadBufferedSinkFlush() noexcept {
    flushThreadBuffer(threadLocalBuffer(), BufferedSinkFlushMode::Forced);
}

inline void xyzzy::scopetimer::ScopeTimer::asyncSinkWrite(const char* data, std::size_t len) noexcept {
    if (len == 0) {
        return;
    }

    auto batch = acquireAsyncSinkBatch(len);
    std::memcpy(batch.data.data(), data, len);

    auto& state = asyncSinkState();
    bool notifyWorker = false;
    {
        std::lock_guard lock(state.mutex);
        notifyWorker = state.queue.empty();
        state.queue.emplace_back(std::move(batch));
    }
    if (notifyWorker) {
        state.ready.notify_one();
    }
}

inline void xyzzy::scopetimer::ScopeTimer::asyncSinkFlush() noexcept {
    auto& state = asyncSinkState();
    std::unique_lock lock(state.mutex);
    if (!state.running) {
        return;
    }
    state.drained.wait(lock, [&state] {
        return state.queue.empty() && !state.writing;
    });
    lock.unlock();
    switch (asyncSinkTargetModeStorage().load(std::memory_order_acquire)) {
        case AsyncSinkTargetMode::Custom:
            flushCustomSink();
            break;
        case AsyncSinkTargetMode::Default:
            defaultSinkFlush();
            break;
    }
}
#endif // NDEBUG
