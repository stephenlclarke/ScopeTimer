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
 * 4. **Buffered High-Throughput Logging**: Uses a large user buffer with batched handoffs to
 *    reduce caller-thread I/O overhead while preserving timely log visibility.
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
 *     Defaults to `/tmp` on POSIX. On Windows, TEMP, then TMP, then the current
 *     directory is used. The selected directory must already exist and be writable.
 *
 * - SCOPE_TIMER_FLUSH_N:
 *     Specifies the number of log lines between direct custom sink flush hook calls.
 *     Must be a positive integer; defaults to 4096 if unset or invalid.
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
#include <cerrno>
#include <cctype>     // for std::toupper
#include <chrono>
#include <charconv>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fcntl.h>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <sys/stat.h>
#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#else
#include <sys/uio.h>
#include <unistd.h>
#endif
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(_MSVC_LANG)
#define SCOPETIMER_CPLUSPLUS _MSVC_LANG
#else
#define SCOPETIMER_CPLUSPLUS __cplusplus
#endif

#if SCOPETIMER_CPLUSPLUS < 202002L
#error "ScopeTimer requires C++20 or later."
#endif
#undef SCOPETIMER_CPLUSPLUS

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
        struct CustomSinkLineCounterTag {};
        struct ThreadBufferFlushBytesTag {};
        struct ThreadBufferRegistryMutexTag {};
        struct ThreadBufferRegistryTag {};
        struct SinkConfigMutexTag {};
        struct ActiveSinkStorageTag {};
        struct BufferedSinkTargetModeStorageTag {};
        struct AsyncSinkTargetModeStorageTag {};
        struct CustomSinkWriteStorageTag {};
        struct CustomSinkFlushStorageTag {};
        struct CustomLogSinkStorageTag {};
        struct BufferedTestSinkWriteStorageTag {};
        struct AsyncSinkStateTag {};
        struct LocaltimeMutexTag {};
        struct ProcessCleanupStartedTag {};
        struct LogOpenStateTag {};
        struct LogDirectoryStateTag {};
    } // namespace detail

    inline std::mutex& outMutex() noexcept {
        return detail::singletonStorage<detail::OutMutexTag, std::mutex>();
    }

    inline std::mutex& localtimeMutex() noexcept {
        return detail::singletonStorage<detail::LocaltimeMutexTag, std::mutex>();
    }

    inline unsigned& customSinkLineCounter() noexcept {
        // Custom-sink callers hold outMutex(), so an atomic read-modify-write
        // would add overhead without providing any additional synchronization.
        return detail::singletonStorage<detail::CustomSinkLineCounterTag, unsigned>(0U);
    }

    // Small helper extracted to make branch coverage testable in unit tests
    namespace ScopeTimerDetail {
        inline std::size_t finalize_snprintf_result(int n, char* line, std::size_t lineSize) noexcept {
            if (line == nullptr || lineSize == 0U) {
                return 0U;
            }
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
        // Arrays are C strings unless the caller supplies an explicit string_view.
        template <std::size_t N>
        inline std::string_view labelArrayView(const char (&text)[N]) noexcept {
            const auto end = std::find(text, text + N, '\0');
            return {text, static_cast<std::size_t>(end - text)};
        }

        enum class LabelStorageKind : std::uint8_t {
            Borrowed,
            Copy,
        };

        struct BorrowedWhere {
            std::string_view view;
        };

        template <std::size_t N>
        inline BorrowedWhere makeBorrowedWhere(const char (&literal)[N]) noexcept {
            return BorrowedWhere{std::string_view{literal, N ? N - 1U : 0U}};
        }

        inline BorrowedWhere makeBorrowedWhere(const char* stableText) noexcept {
            return BorrowedWhere{stableText ? std::string_view{stableText} : std::string_view{}};
        }

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
            explicit LabelArg(const char (&label)[N]) noexcept
                : view_(labelArrayView(label)) {
                // A char array can be an automatic object as well as a string
                // literal. Keep a timer-local copy so delayed destruction never
                // observes an expired or subsequently modified array.
                storageKind_ = LabelStorageKind::Copy;
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
        inline LabelData makeLabelData(const char (&label)[N]) noexcept {
            return LabelData(
                detail::labelArrayView(label),
                {},
                LabelStorageKind::Copy
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

    namespace detail {
    // Owns sink configuration, delivery, buffers and process/thread teardown.
    // Reconfiguration and cross-thread draining require quiescent producers.
    class SinkRuntime {
    public:
        static inline constexpr std::size_t DefaultSinkFlushBytes = 16U * 1024U;
        static inline constexpr std::size_t MaxSinkFlushBytes = 64U * 1024U * 1024U;

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

        // Call after profiled workers have quiesced, just like sink reconfiguration.
        static inline void flush() noexcept {
            if (customSinkCallbackActive_ || processCleanupStartedStorage().load(std::memory_order_acquire)) {
                return;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
        }

        static inline std::uint64_t droppedRecords() noexcept {
            return droppedRecords_.load(std::memory_order_relaxed);
        }

        static inline void enableThreadBufferedSink(std::size_t flushBytes = DefaultSinkFlushBytes) noexcept {
            if (!canEmit()) {
                return;
            }
            flushBytes = normalizeSinkFlushBytes(flushBytes);
            std::lock_guard sinkStateLock(sinkConfigMutex());
            registerProcessCleanup();
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            {
                std::lock_guard outputLock(outMutex());
                closeLogFd();
                threadBufferFlushBytesStorage().store(flushBytes);
                activeSinkStorage().store(ActiveSink::ThreadBuffered, std::memory_order_release);
                bufferedSinkTargetModeStorage().store(hasCustomSink() ? BufferedSinkTargetMode::Custom
                                                                      : BufferedSinkTargetMode::Default,
                                                      std::memory_order_release);
            }
        }

        static inline void disableThreadBufferedSink() noexcept {
            if (!canEmit()) {
                return;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            {
                std::lock_guard outputLock(outMutex());
                closeLogFd();
                restoreDefaultSinkState();
            }
        }

        static inline void enableAsyncSink(std::size_t flushBytes = DefaultSinkFlushBytes) noexcept {
            if (!canEmit()) {
                return;
            }
            flushBytes = std::min(normalizeSinkFlushBytes(flushBytes), MaxAsyncSinkQueuedBytes);
            std::lock_guard sinkStateLock(sinkConfigMutex());
            registerProcessCleanup();
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();
            {
                std::lock_guard outputLock(outMutex());
                closeLogFd();
                threadBufferFlushBytesStorage().store(flushBytes);
                activeSinkStorage().store(ActiveSink::ThreadBuffered, std::memory_order_release);
                bufferedSinkTargetModeStorage().store(BufferedSinkTargetMode::Async, std::memory_order_release);
                asyncSinkTargetModeStorage().store(hasCustomSink() ? AsyncSinkTargetMode::Custom
                                                                   : AsyncSinkTargetMode::Default,
                                                   std::memory_order_release);
            }
            ensureAsyncSinkRunning();
        }

        static inline void disableAsyncSink() noexcept {
            disableThreadBufferedSink();
        }

        static inline void setLogSink(LogSink& sink) {
            setCustomLogSink(&sink);
        }

        static inline void resetLogSink() {
            setCustomLogSink(nullptr);
        }

    protected:
        friend class ::xyzzy::scopetimer::ScopeTimer_TestFriend;
        static inline bool canEmit() noexcept {
            return !processCleanupStartedStorage().load(std::memory_order_acquire)
                && !customSinkCallbackActive_;
        }

        static inline void emitLine(const char* data, std::size_t len) noexcept {
            // These sink-state atomics are intentionally acquire/release or relaxed
            // instead of seq_cst. They publish configuration chosen under
            // sinkConfigMutex(), and stronger global ordering would add fences on the
            // steady-state timer path without improving correctness. Sonar's blanket
            // seq_cst rule is suppressed for this header in sonar-project.properties.
            const auto activeSink = activeSinkStorage().load(std::memory_order_acquire);
            if (activeSink != ActiveSink::ThreadBuffered) {
                std::lock_guard lock(outMutex());
                if (len) {
                    writeToActiveSink(activeSink, data, len);

                    // Serialize custom sink flush hooks with writes and sink
                    // reconfiguration. The default file sink is unbuffered and its
                    // flush hook is a no-op, so it needs no periodic bookkeeping.
                    if (activeSink == ActiveSink::Custom) {
                        recordDirectCustomSinkLine();
                    }
                }
            } else if (len) {
                // Thread-buffered sink flushes on size; avoid periodic counters and
                // interval checks to keep the buffered hot path cheap.
                writeToActiveSink(activeSink, data, len);
            }
        }

        /**
         * @brief Returns the periodic direct custom sink flush interval.
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
         * Controlled by SCOPE_TIMER_DIR, with a platform-appropriate fallback.
         */
        struct LogDirectoryState {
            std::string cache{"/tmp/"};
            bool initialized{false};
        };

        static inline LogDirectoryState& logDirectoryState() noexcept {
            return detail::singletonStorage<detail::LogDirectoryStateTag, LogDirectoryState>();
        }

        static inline const std::string& logDirectory() {
            auto& state = logDirectoryState();
            if (!state.initialized) {
                resetLogDirectoryForTests();
            }
            return state.cache;
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
#if defined(_WIN32)
                if (const char* tempDir = std::getenv("TEMP"); tempDir && *tempDir) {
                    normalized = tempDir;
                } else if (const char* tmpDir = std::getenv("TMP"); tmpDir && *tmpDir) {
                    normalized = tmpDir;
                } else {
                    normalized = ".";
                }
#else
                normalized = "/tmp";
#endif
            }
            if (!normalized.empty()
#if defined(_WIN32)
                && normalized.back() != '/' && normalized.back() != '\\'
#else
                && normalized.back() != '/'
#endif
            ) {
#if defined(_WIN32)
                normalized.push_back('\\');
#else
                normalized.push_back('/');
#endif
            }
            auto& state = logDirectoryState();
            state.cache = std::move(normalized);
            state.initialized = true;
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

        struct AsyncSinkBatch {
            std::vector<char> data;
            std::size_t size{0U};
        };

        struct ThreadBufferState {
            std::mutex flushMutex;
            std::vector<char> data;
            std::size_t size{0U};
            std::size_t capacity{0U};
        };

        struct ThreadBufferHandle {
            std::shared_ptr<ThreadBufferState> state;

            ThreadBufferHandle() noexcept {
                try {
                    state = std::make_shared<ThreadBufferState>();
                    registerThreadBuffer(state);
                } catch (...) {
                    // Profiling must not terminate the host when thread-local
                    // diagnostic storage cannot be allocated.
                    state.reset();
                }
            }

            ~ThreadBufferHandle() {
                threadBufferTeardown_ = true;
                if (state) {
                    if (!SinkRuntime::processCleanupStartedStorage().load(std::memory_order_acquire)) {
                        SinkRuntime::flushThreadBuffer(*state);
                    }
                    SinkRuntime::unregisterThreadBuffer(state);
                }
            }

            ThreadBufferHandle(const ThreadBufferHandle&) = delete;
            ThreadBufferHandle& operator=(const ThreadBufferHandle&) = delete;
            ThreadBufferHandle(ThreadBufferHandle&&) = delete;
            ThreadBufferHandle& operator=(ThreadBufferHandle&&) = delete;
        };

        // Trivially destructible state remains readable after the handle is gone.
        static inline thread_local bool threadBufferTeardown_{false};
        static inline ThreadBufferState* threadLocalBuffer() noexcept {
            if (threadBufferTeardown_) {
                return nullptr;
            }
            thread_local ThreadBufferHandle handle;
            return handle.state.get();
        }
        static inline std::size_t normalizeSinkFlushBytes(std::size_t flushBytes) noexcept {
            if (flushBytes == 0U) {
                return DefaultSinkFlushBytes;
            }
            return std::min(flushBytes, MaxSinkFlushBytes);
        }
        static inline std::atomic<std::size_t>& threadBufferFlushBytesStorage() noexcept {
            return detail::singletonStorage<detail::ThreadBufferFlushBytesTag, std::atomic<std::size_t>>(DefaultSinkFlushBytes);
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
            try {
                std::lock_guard lock(threadBufferRegistryMutex());
                threadBufferRegistry().emplace_back(state);
            } catch (...) {
                // The owning thread still flushes its local buffer at thread
                // exit; only cross-thread best-effort draining is unavailable.
            }
        }
        static inline void unregisterThreadBuffer(const std::shared_ptr<ThreadBufferState>& state) noexcept {
            try {
                std::lock_guard lock(threadBufferRegistryMutex());
                auto& registry = threadBufferRegistry();
                registry.erase(
                    std::remove_if(registry.begin(), registry.end(), [&state](const auto& weakState) {
                        return weakState.expired()
                            || (!weakState.owner_before(state) && !state.owner_before(weakState));
                    }),
                    registry.end()
                );
            } catch (...) {
                // Registry maintenance is best-effort; the shared state still
                // releases normally even if its weak entry cannot be removed.
            }
        }
        static inline std::vector<std::shared_ptr<ThreadBufferState>> snapshotThreadBuffers() noexcept {
            try {
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
            } catch (...) {
                return {};
            }
        }
        static inline bool ensureThreadBufferCapacity(ThreadBufferState& state, std::size_t flushBytes) noexcept {
            if (state.capacity >= flushBytes) {
                return true;
            }

            std::lock_guard lock(state.flushMutex);
            if (state.capacity >= flushBytes) {
                return true;
            }

            try {
                if (state.capacity == 0U && bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async) {
                    auto replacement = acquireAsyncSinkBatch(flushBytes);
                    if (!replacement) return false;
                    state.data = std::move(replacement->data);
                } else {
                    state.data.resize(flushBytes);
                }
                state.capacity = state.data.size();
                return true;
            } catch (...) {
                return false;
            }
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
            if (bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async) {
                AsyncSinkBatch batch;
                {
                    std::lock_guard lock(state.flushMutex);
                    if (state.size == 0U) return;
                    batch.data.swap(state.data);
                    batch.size = std::exchange(state.size, 0U);
                    state.capacity = 0U;
                }
                enqueueAsyncSinkBatch(std::move(batch));
                return;
            }
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
                if (bufferedTarget == BufferedSinkTargetMode::Async) {
                    flushThreadBuffer(*state);
                    continue;
                }
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
        static inline std::atomic<bool>& processCleanupStartedStorage() noexcept {
            return detail::singletonStorage<detail::ProcessCleanupStartedTag, std::atomic<bool>>(false);
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
        static inline LogSink*& customLogSinkStorage() noexcept {
            return detail::singletonStorage<detail::CustomLogSinkStorageTag, LogSink*>(nullptr);
        }
        static inline std::function<void(const char*, std::size_t)>& bufferedTestSinkWriteStorage() {
            return detail::singletonStorage<detail::BufferedTestSinkWriteStorageTag, std::function<void(const char*, std::size_t)>>();
        }
        static inline bool hasCustomSink() {
            return customLogSinkStorage() != nullptr || static_cast<bool>(customSinkWriteStorage());
        }
        static inline std::atomic<std::uint64_t> droppedRecords_{0U};
        static inline void recordDroppedPayload(const char* data, std::size_t len) noexcept {
            if (len == 0U) return;
            const auto records = static_cast<std::uint64_t>(std::count(data, data + len, '\n'))
                + (data[len - 1U] != '\n' ? 1U : 0U);
            droppedRecords_.fetch_add(records, std::memory_order_relaxed);
        }
        static inline thread_local bool customSinkCallbackActive_{false};
        static inline void writeToCustomSink(const char* data, std::size_t len) noexcept {
            if (customSinkCallbackActive_) {
                // A callback may instrument its own work. Dropping that nested
                // diagnostic record prevents recursive callback loops.
                return;
            }
            customSinkCallbackActive_ = true;
            try {
                if (auto* sink = customLogSinkStorage()) {
                    sink->write(data, len);
                } else if (const auto& writeFn = customSinkWriteStorage(); writeFn) {
                    writeFn(data, len);
                }
            } catch (...) {
                recordDroppedPayload(data, len);
                // A throwing std::function must not violate the timer's
                // noexcept destructor contract.
            }
            customSinkCallbackActive_ = false;
        }
        static inline void flushCustomSink() noexcept {
            if (customSinkCallbackActive_) {
                return;
            }
            customSinkCallbackActive_ = true;
            try {
                if (auto* sink = customLogSinkStorage()) {
                    sink->flush();
                } else if (const auto& flushFn = customSinkFlushStorage(); flushFn) {
                    flushFn();
                }
            } catch (...) {
                // See writeToCustomSink(): diagnostics cannot terminate the
                // application merely because a callback throws.
            }
            customSinkCallbackActive_ = false;
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
        static inline void recordDirectCustomSinkLine() noexcept {
            // Direct custom-sink writes call this with outMutex() held.
            auto& linesSinceFlush = customSinkLineCounter();
            ++linesSinceFlush;
            if (linesSinceFlush == flushInterval()) { // configurable via SCOPE_TIMER_FLUSH_N
                linesSinceFlush = 0U;
                flushCustomSink();
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

        static inline constexpr std::size_t MaxAsyncSinkQueuedBytes = 16U * 1024U * 1024U;
        static inline constexpr std::size_t MaxAsyncSinkRecycledBytes = 16U * 1024U * 1024U;

        struct AsyncSinkState {
            std::mutex mutex;
            std::condition_variable ready;
            std::condition_variable drained;
            std::deque<AsyncSinkBatch> queue;
            std::vector<AsyncSinkBatch> recycled;
            std::thread worker;
            std::size_t queuedBytes{0U};
            std::size_t recycledBytes{0U};
            bool running{false};
            bool stop{false};
            bool writing{false};
            bool restartAfterSelfShutdown{false};
        };

        static inline AsyncSinkState& asyncSinkState() noexcept {
            return detail::singletonStorage<detail::AsyncSinkStateTag, AsyncSinkState>();
        }
        static inline thread_local bool asyncSinkWorkerActive_{false};

        static inline bool isAsyncSinkWorkerThread() noexcept {
            return asyncSinkWorkerActive_;
        }

        static inline std::optional<AsyncSinkBatch> acquireAsyncSinkBatch(std::size_t len) noexcept {
            try {
                AsyncSinkBatch batch;
                auto& state = asyncSinkState();
                {
                    std::lock_guard lock(state.mutex);
                    if (!state.recycled.empty()) {
                        batch = std::move(state.recycled.back());
                        const auto capacity = batch.data.capacity();
                        state.recycled.pop_back();
                        state.recycledBytes = capacity > state.recycledBytes
                            ? 0U
                            : state.recycledBytes - capacity;
                    }
                }

                if (batch.data.size() < len) {
                    batch.data.resize(len);
                }
                batch.size = len;
                return batch;
            } catch (...) {
                return std::nullopt;
            }
        }
        static inline void enqueueAsyncSinkBatch(AsyncSinkBatch batch) noexcept {
            const auto len = batch.size;
            auto& state = asyncSinkState();
            bool notifyWorker = false;
            {
                std::lock_guard lock(state.mutex);
                if (!state.running || len > MaxAsyncSinkQueuedBytes || state.queuedBytes > MaxAsyncSinkQueuedBytes - len) {
                    recordDroppedPayload(batch.data.data(), len);
                    return;
                }
                notifyWorker = state.queue.empty();
                try {
                    state.queue.emplace_back(std::move(batch));
                    state.queuedBytes += len;
                } catch (...) {
                    recordDroppedPayload(batch.data.data(), len);
                    return;
                }
            }
            if (notifyWorker) {
                state.ready.notify_one();
            }
        }

#if !defined(_WIN32)
        static inline void writeIovecsBestEffort(int fd, ::iovec* vectors, std::size_t count) noexcept {
            while (count != 0U) {
                const auto written = ::writev(fd, vectors, static_cast<int>(count));
                if (written < 0 && errno == EINTR) continue;
                if (written <= 0) {
                    for (std::size_t i = 0; i < count; ++i) {
                        recordDroppedPayload(static_cast<const char*>(vectors[i].iov_base), vectors[i].iov_len);
                    }
                    return;
                }
                auto remaining = static_cast<std::size_t>(written);
                while (count != 0U && remaining >= vectors->iov_len) {
                    remaining -= vectors->iov_len;
                    ++vectors;
                    --count;
                }
                if (count != 0U) {
                    vectors->iov_base = static_cast<char*>(vectors->iov_base) + remaining;
                    vectors->iov_len -= remaining;
                }
            }
        }
#endif

        static inline void defaultSinkWriteBatches(const std::deque<AsyncSinkBatch>& batches) noexcept {
#if !defined(_WIN32)
            constexpr std::size_t MaxIovecs = 64U;
            std::array<::iovec, MaxIovecs> iovecs{};
            std::size_t count = 0U;

            int fd = logFd();
            if (fd < 0) {
                if (!ensureLogFdOpen()) {
                    for (const auto& batch : batches) recordDroppedPayload(batch.data.data(), batch.size);
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
                    writeIovecsBestEffort(fd, iovecs.data(), count);
                    count = 0U;
                }
            }

            if (count != 0U) {
                writeIovecsBestEffort(fd, iovecs.data(), count);
            }
#else
            for (const auto& batch : batches) {
                defaultSinkWrite(batch.data.data(), batch.size);
            }
#endif
        }
        static inline void runAsyncSinkWorker() noexcept {
            auto& workerState = asyncSinkState();
            asyncSinkWorkerActive_ = true;
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
                    workerState.queuedBytes = 0U;
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
                        const auto capacity = batch.data.capacity();
                        if (capacity <= MaxAsyncSinkRecycledBytes - std::min(workerState.recycledBytes, MaxAsyncSinkRecycledBytes)) {
                            try {
                                workerState.recycled.emplace_back(std::move(batch));
                                workerState.recycledBytes += capacity;
                            } catch (...) {
                                // Dropping a recycled batch is preferable to
                                // terminating the application for diagnostics.
                            }
                        }
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

            bool restart = false;
            {
                std::lock_guard lock(workerState.mutex);
                restart = workerState.restartAfterSelfShutdown
                    && activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::ThreadBuffered
                    && bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async
                    && !processCleanupStartedStorage().load(std::memory_order_acquire);
                workerState.restartAfterSelfShutdown = false;
                workerState.stop = false;
                workerState.running = false;
                workerState.writing = false;
                workerState.drained.notify_all();
            }
            asyncSinkWorkerActive_ = false;
            if (restart) {
                ensureAsyncSinkRunning();
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
            state.restartAfterSelfShutdown = false;
            try {
                state.worker = std::thread([] { runAsyncSinkWorker(); });
                state.running = true;
            } catch (...) {
                state.running = false;
                state.stop = false;
            }
        }

        static inline void shutdownAsyncSink() noexcept {
            auto& state = asyncSinkState();
            std::unique_lock lock(state.mutex);
            if (!state.running) {
                return;
            }
            state.stop = true;
            if (isAsyncSinkWorkerThread()) {
                state.restartAfterSelfShutdown = true;
                if (state.worker.joinable()) {
                    state.worker.detach();
                }
                lock.unlock();
                state.ready.notify_all();
                return;
            }
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
            if (!canEmit()) {
                // Sink registration is a quiescent setup/teardown operation;
                // changing a callback while it is executing would invalidate
                // the std::function being invoked.
                return;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();

            const bool asyncModeActive = activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::ThreadBuffered
                && bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async;

            std::lock_guard outputLock(outMutex());
            closeLogFd();
            customLogSinkStorage() = nullptr;
            customSinkWriteStorage() = std::move(writeFn);
            customSinkFlushStorage() = flushFn ? std::move(flushFn) : std::function<void()>{};
            updateCustomSinkRouting(asyncModeActive);
        }

        static inline void setCustomLogSink(LogSink* sink) {
            if (!canEmit()) {
                return;
            }
            std::lock_guard sinkStateLock(sinkConfigMutex());
            flushAllThreadBuffers();
            asyncSinkFlush();
            shutdownAsyncSink();

            const bool asyncModeActive = activeSinkStorage().load(std::memory_order_acquire) == ActiveSink::ThreadBuffered
                && bufferedSinkTargetModeStorage().load(std::memory_order_acquire) == BufferedSinkTargetMode::Async;

            std::lock_guard outputLock(outMutex());
            closeLogFd();
            customLogSinkStorage() = sink;
            customSinkWriteStorage() = {};
            customSinkFlushStorage() = {};
            updateCustomSinkRouting(asyncModeActive);
        }

        static inline void updateCustomSinkRouting(bool asyncModeActive) {
            // A newly configured direct sink starts a fresh flush cadence.
            // This function is called with outMutex() held, which also protects
            // the counter on the steady-state write path.
            customSinkLineCounter() = 0U;

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

        static inline int openLogFileForAppend(const std::string& path) noexcept {
#if defined(_WIN32)
            const HANDLE handle = ::CreateFileA(
                path.c_str(),
                FILE_APPEND_DATA,
                FILE_SHARE_READ,
                nullptr,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT,
                nullptr
            );
            if (handle == INVALID_HANDLE_VALUE) {
                return -1;
            }
            BY_HANDLE_FILE_INFORMATION status{};
            if (::GetFileInformationByHandle(handle, &status) == 0
                || (status.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) {
                (void)::CloseHandle(handle);
                return -1;
            }

            int openFlags = _O_WRONLY | _O_APPEND;
#ifdef _O_BINARY
            openFlags |= _O_BINARY;
#endif
#ifdef _O_NOINHERIT
            openFlags |= _O_NOINHERIT;
#endif
            const int fd = ::_open_osfhandle(
                reinterpret_cast<intptr_t>(handle),
                openFlags
            );
            if (fd < 0) {
                (void)::CloseHandle(handle);
            }
            return fd;
#else
            int openFlags = O_CREAT | O_WRONLY | O_APPEND | O_NONBLOCK;
#ifdef O_CLOEXEC
            openFlags |= O_CLOEXEC;
#endif
#ifdef O_NOFOLLOW
            openFlags |= O_NOFOLLOW;
#else
            // A predictable filename under /tmp must not fall back to an
            // implementation that follows attacker-controlled symlinks.
            return -1;
#endif
            const int fd = ::open(path.c_str(), openFlags, 0600);
            if (fd < 0) {
                return -1;
            }
            struct stat status {};
            if (::fstat(fd, &status) != 0
                || !S_ISREG(status.st_mode)
                || status.st_uid != ::geteuid()
                || status.st_nlink != 1
                || ::fchmod(fd, 0600) != 0) {
                (void)::close(fd);
                return -1;
            }
#ifndef O_CLOEXEC
            (void)::fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif
            return fd;
#endif
        }

        static inline void closeFd(int fd) noexcept {
#if defined(_WIN32)
            (void)::_close(fd);
#else
            (void)::close(fd);
#endif
        }

        static inline void writeFdBestEffort(int fd, const char* data, std::size_t len) noexcept {
            while (len != 0U) {
#if defined(_WIN32)
                const auto chunk = static_cast<unsigned int>(std::min(len,
                    static_cast<std::size_t>(std::numeric_limits<int>::max())));
                const auto written = ::_write(fd, data, chunk);
#else
                const auto written = ::write(fd, data, len);
#endif
                if (written < 0 && errno == EINTR) continue;
                if (written <= 0) {
                    recordDroppedPayload(data, len);
                    return;
                }
                data += written;
                len -= static_cast<std::size_t>(written);
            }
        }

        struct LogOpenState {
            std::string lastFailedPath;
            std::chrono::steady_clock::time_point lastFailure{};
            bool lastAttemptFailed{false};
        };

        static inline LogOpenState& logOpenState() noexcept {
            return detail::singletonStorage<detail::LogOpenStateTag, LogOpenState>();
        }

        /**
         * @brief Opens the default log file descriptor on first use (best-effort).
         */
        static inline bool ensureLogFdOpen() noexcept {
            int& fd = logFd();
            if (fd >= 0) {
                return true;
            }

            try {
                auto& state = logOpenState();
                const std::string path = logDirectory() + "ScopeTimer.log";
                const auto now = std::chrono::steady_clock::now();
                constexpr auto RetryDelay = std::chrono::milliseconds{100};

                if (state.lastAttemptFailed
                    && path == state.lastFailedPath
                    && now - state.lastFailure < RetryDelay) {
                    return false;
                }

                if (int newFd = openLogFileForAppend(path); newFd >= 0) {
                    fd = newFd;
                    state.lastAttemptFailed = false;
                    state.lastFailedPath.clear();
                    registerProcessCleanup();
                    return true;
                }

                state.lastFailedPath = path;
                state.lastFailure = now;
                state.lastAttemptFailed = true;
                return false;
            } catch (...) {
                return false;
            }
        }

        /**
         * @brief Registers the atexit handler that drains active sinks and closes the log descriptor.
         */
        static inline void registerProcessCleanup() noexcept {
            // Function-local static initialization is synchronized by C++11.
            // This can be reached concurrently through the default sink and a
            // buffered-sink setup call, so a hand-rolled boolean would race.
            static const bool registered = []() noexcept {
                return std::atexit([]() noexcept {
                    std::lock_guard sinkStateLock(sinkConfigMutex());
                    processCleanupStartedStorage().store(true, std::memory_order_release);
                    flushAllThreadBuffers();
                    asyncSinkFlush();
                    shutdownAsyncSink();
                    std::lock_guard outputLock(outMutex());
                    closeLogFd();
                    customLogSinkStorage() = nullptr;
                    customSinkWriteStorage() = {};
                    customSinkFlushStorage() = {};
                    bufferedTestSinkWriteStorage() = {};
                    activeSinkStorage().store(ActiveSink::Default, std::memory_order_release);
                    bufferedSinkTargetModeStorage().store(BufferedSinkTargetMode::Default, std::memory_order_release);
                    asyncSinkTargetModeStorage().store(AsyncSinkTargetMode::Default, std::memory_order_release);
                }) == 0;
            }();
            (void)registered;
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
                closeFd(fd);
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

    };

    // Formatting has no ownership of timers or sinks.
    class TimerFormatting {
    protected:
        friend class ::xyzzy::scopetimer::ScopeTimer_TestFriend;
        // One-time-selected elapsed-time formatter infrastructure
        // I call through a cached function pointer to avoid branching in the hot path.
        enum class TimeFormat { Auto, Seconds, Millis, Micros, Nanos };
        using FormatterFn = std::size_t(*)(long long ns, char* out, size_t outSz) noexcept;

        static inline bool fillLocalTime(std::tm& tm, const std::time_t& tt) noexcept {
#if defined(_WIN32) && defined(_MSC_VER)
            return ::localtime_s(&tm, &tt) == 0;
#elif defined(_WIN32)
            std::lock_guard lock(localtimeMutex());
            if (std::tm* local = std::localtime(&tt)) {
                tm = *local;
                return true;
            }
            return false;
#else
            return ::localtime_r(&tt, &tm) != nullptr;
#endif
        }

        static inline bool appendFixedDigits(char*& out, const char* end, unsigned value, unsigned width) noexcept {
            if (constexpr unsigned MaxFixedDigitWidth = 10U;
                width > MaxFixedDigitWidth || static_cast<std::size_t>(end - out) < width) {
                return false;
            }

            if (width == 2U) {
                out[0] = static_cast<char>('0' + ((value / 10U) % 10U));
                out[1] = static_cast<char>('0' + (value % 10U));
                out += 2U;
                return true;
            }

            if (width == 4U) {
                out[0] = static_cast<char>('0' + ((value / 1000U) % 10U));
                out[1] = static_cast<char>('0' + ((value / 100U) % 10U));
                out[2] = static_cast<char>('0' + ((value / 10U) % 10U));
                out[3] = static_cast<char>('0' + (value % 10U));
                out += 4U;
                return true;
            }

            for (unsigned i = width; i > 0U; --i) {
                out[i - 1U] = static_cast<char>('0' + (value % 10U));
                value /= 10U;
            }
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

        static inline void appendBytesTruncatingReserved(
            char*& out,
            const char* end,
            const char* s,
            std::size_t n,
            std::size_t reservedBytes
        ) noexcept {
            if (n == 0U || out >= end) {
                return;
            }
            const auto available = static_cast<std::size_t>(end - out);
            if (available <= reservedBytes) {
                return;
            }
            const auto toCopy = std::min(n, available - reservedBytes);
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
                if (!fillLocalTime(tm, tt)) {
                    out[0] = '\0';
                    return 0;
                }

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

        static inline void appendThreadIdTruncatingReserved(
            char*& out,
            const char* end,
            unsigned tid,
            std::size_t reservedBytes
        ) noexcept {
            std::array<char, 16> digits{};
            char* digitEnd = digits.data();
            if (tid < 1000U) {
                digits[0] = static_cast<char>('0' + ((tid / 100U) % 10U));
                digits[1] = static_cast<char>('0' + ((tid / 10U) % 10U));
                digits[2] = static_cast<char>('0' + (tid % 10U));
                digitEnd += 3U;
            } else {
                const auto result = std::to_chars(digits.data(), digits.data() + digits.size(), tid);
                if (result.ec != std::errc{}) {
                    return;
                }
                digitEnd = result.ptr;
            }
            appendBytesTruncatingReserved(
                out,
                end,
                digits.data(),
                static_cast<std::size_t>(digitEnd - digits.data()),
                reservedBytes
            );
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
            if (outSz < 2U) {
                if (outSz == 1U) {
                    out[0] = '\0';
                }
                return 0;
            }

            char* cur = out;
            const char* end = out + outSz - 2U; // reserve newline plus terminator
            constexpr std::size_t MaxThreadIdBytes = 10U;
            constexpr std::size_t ClosingLabelBytes = sizeof("] TID=") - 1U;
            constexpr std::size_t SeparatorBytes = sizeof(" | ") - 1U;
            const std::size_t elapsedBytes = (sizeof(" | elapsed=") - 1U) + fields.elapsed.size();
            const std::size_t afterLabelBytes = ClosingLabelBytes + MaxThreadIdBytes + SeparatorBytes + elapsedBytes;

            if (static_cast<std::size_t>(end - cur) > afterLabelBytes) {
                appendCharTruncating(cur, end, '[');
            }
            appendBytesTruncatingReserved(cur, end, fields.label.data(), fields.label.size(), afterLabelBytes);
            appendBytesTruncatingReserved(cur, end, "] TID=", ClosingLabelBytes,
                                          MaxThreadIdBytes + SeparatorBytes + elapsedBytes);
            appendThreadIdTruncatingReserved(cur, end, fields.threadNum, SeparatorBytes + elapsedBytes);
            appendBytesTruncatingReserved(cur, end, " | ", SeparatorBytes, elapsedBytes);
            appendBytesTruncatingReserved(cur, end, fields.where.data(), fields.where.size(), elapsedBytes);
            if (fields.wallTimeEnabled) {
                appendBytesTruncatingReserved(cur, end, " | start=", sizeof(" | start=") - 1U, elapsedBytes);
                appendBytesTruncatingReserved(cur, end, fields.startWall.data(), fields.startWall.size(), elapsedBytes);
                appendBytesTruncatingReserved(cur, end, " | end=", sizeof(" | end=") - 1U, elapsedBytes);
                appendBytesTruncatingReserved(cur, end, fields.endWall.data(), fields.endWall.size(), elapsedBytes);
            }
            appendBytesTruncating(cur, end, " | elapsed=", sizeof(" | elapsed=") - 1U);
            appendBytesTruncating(cur, end, fields.elapsed.data(), fields.elapsed.size());
            *cur++ = '\n';
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
            if (outSz < 2U) {
                if (outSz == 1U) {
                    out[0] = '\0';
                }
                return 0;
            }

            char* cur = out;
            const char* end = out + outSz - 2U;
            const std::size_t elapsedBytes = (sizeof("] elapsed=") - 1U) + elapsedLen;
            if (static_cast<std::size_t>(end - cur) > elapsedBytes) {
                appendCharTruncating(cur, end, '[');
            }
            appendBytesTruncatingReserved(cur, end, label.data(), label.size(), elapsedBytes);
            appendBytesTruncating(cur, end, "] elapsed=", sizeof("] elapsed=") - 1U);
            appendBytesTruncating(cur, end, elapsed, elapsedLen);
            *cur++ = '\n';
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

        static inline thread_local FormatBuffers tlsFormatBuffers_{};
        static inline thread_local LineBuffer tlsLineBuffer_{};

    };

    // Both timer variants own labels with the same lifetime and truncation rules.
    class LabelStorage {
    protected:
        friend class ::xyzzy::scopetimer::ScopeTimer_TestFriend;
        inline bool assignLabel(detail::LabelData data) noexcept {
            const std::string_view source = !data.storage.empty() ? std::string_view{data.storage} : data.view;
            if (source.empty()) {
                label_ = "ScopeTimer";
                return true;
            }

            if (data.canBorrowView()) {
                label_ = source;
                labelHeapStorage_.clear();
                return true;
            }

            if (source.size() < labelBuffer_.size()) {
                std::memcpy(labelBuffer_.data(), source.data(), source.size());
                label_ = std::string_view{labelBuffer_.data(), source.size()};
                labelHeapStorage_.clear();
            } else {
                try {
                    labelHeapStorage_ = !data.storage.empty()
                        ? std::move(data.storage)
                        : std::string(source);
                } catch (...) {
                    // Preserve the constructor's noexcept contract by dropping
                    // this timer when long-label storage cannot be allocated.
                    labelHeapStorage_.clear();
                    label_ = "ScopeTimer";
                    return false;
                }
                label_ = labelHeapStorage_;
            }
            return true;
        }

        std::string_view label_{ "ScopeTimer" }; ///< Label for the log output.
        std::array<char, 128> labelBuffer_{};
        std::string labelHeapStorage_;
    };
    } // namespace detail

    /**
     * @brief A high-resolution scope timer for measuring execution time of code blocks.
     *
     * This class uses C++20 features such as std::string_view for lightweight string parameters,
     * thread_local storage and std::atomic for lock-free thread ID assignment,
     * and fixed-size stack buffers for efficient formatting without heap allocation.
     * It logs timing information to a configurable sink with buffered options for
     * high throughput.
     */
    class ScopeTimer : public detail::SinkRuntime, private detail::TimerFormatting, private detail::LabelStorage {
    public:
        struct HotPathTag {};

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

            if (!assignWhere(where)) {
                droppedRecords_.fetch_add(1U, std::memory_order_relaxed);
                disabled_ = true;
                return;
            }
            startStandardTimer(std::move(labelData));
        }

        /**
         * @brief Internal fast path for compiler-provided function names with static storage.
         */
        inline explicit ScopeTimer(
            detail::BorrowedWhere where,
            detail::LabelData labelData = detail::LabelData{}
        ) noexcept {
            if (isDisabled()) {
                disabled_ = true;
                return;
            }

            where_ = where.view;
            startStandardTimer(std::move(labelData));
        }

        /**
         * @brief Convenience overload that accepts a plain string_view label.
         */
        inline explicit ScopeTimer(std::string_view where, std::string_view label) noexcept
            : ScopeTimer(where, detail::LabelData{label}) {}

        template <std::size_t N>
        inline explicit ScopeTimer(std::string_view where, const char (&label)[N]) noexcept
            : ScopeTimer(where, detail::LabelData{
                detail::labelArrayView(label),
                {},
                detail::LabelStorageKind::Copy
            }) {}

        inline explicit ScopeTimer(HotPathTag, detail::LabelData labelData = detail::LabelData{}) noexcept {
            if (isDisabled()) {
                disabled_ = true;
                return;
            }

            hotPathMode_ = true;
            if (!assignLabel(std::move(labelData))) {
                droppedRecords_.fetch_add(1U, std::memory_order_relaxed);
                disabled_ = true;
                return;
            }
            startSteady_ = std::chrono::steady_clock::now();
        }

        inline explicit ScopeTimer(HotPathTag, std::string_view label) noexcept
            : ScopeTimer(HotPathTag{}, detail::LabelData{label}) {}

        template <std::size_t N>
        inline explicit ScopeTimer(HotPathTag, const char (&label)[N]) noexcept
            : ScopeTimer(HotPathTag{}, detail::LabelData{
                detail::labelArrayView(label),
                {},
                detail::LabelStorageKind::Copy
            }) {}

        ScopeTimer(const ScopeTimer&) = delete; ///< Deleted copy constructor for safety.
        ScopeTimer& operator=(const ScopeTimer&) = delete; ///< Deleted copy assignment operator.
        ScopeTimer(ScopeTimer&&) = delete; ///< Timers must remain bound to their original scope.
        ScopeTimer& operator=(ScopeTimer&&) = delete; ///< Timers must remain bound to their original scope.

        /**
         * @brief Destructor that records the end time, computes elapsed duration,
         * formats timing information, and writes a log entry.
         *
         * Logs include thread ID, scope name, start and end timestamps, and elapsed time.
         */
        inline ~ScopeTimer() noexcept {
            if (disabled_ || !canEmit()) return;

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

            emitLine(lineBuf.data, len);
        }

        // Label expressions in the macros are evaluated only when enabled.
        static inline bool enabled() noexcept { return !isDisabled(); }

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

                const std::string val = normalizeBooleanSetting(env);

                return val == "OFF" || val == "FALSE" || val == "NO" || val == "0";
            }();

            return disabled;
        }

        static inline std::string normalizeBooleanSetting(const char* env) {
            std::string value(env ? env : "");
            const auto isSpace = [](char c) {
                return std::isspace(static_cast<unsigned char>(c)) != 0;
            };
            const auto first = std::find_if_not(value.begin(), value.end(), isSpace);
            const auto last = std::find_if_not(value.rbegin(), value.rend(), isSpace).base();
            if (first >= last) {
                return {};
            }

            std::string normalized(first, last);
            std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                           [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return normalized;
        }

        static inline bool isTruthySetting(const char* envName, bool defaultValue) noexcept {
            if (const char* env = std::getenv(envName); env && *env) {
                const std::string value = normalizeBooleanSetting(env);
                if (value.empty()) {
                    return defaultValue;
                }
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

        inline void startStandardTimer(detail::LabelData labelData) noexcept {
            if (!assignLabel(std::move(labelData))) {
                droppedRecords_.fetch_add(1U, std::memory_order_relaxed);
                disabled_ = true;
                return;
            }
            threadNum_ = getThreadIdNumber();
            startSteady_ = std::chrono::steady_clock::now();
            if (includeWallTime()) {
                startWall_ = std::chrono::system_clock::now();
                startWallFormattedLen_ = static_cast<std::uint8_t>(formatTime(
                    startWall_,
                    startWallFormatted_,
                    sizeof(startWallFormatted_)
                ));
            }
        }

        inline bool assignWhere(std::string_view source) noexcept {
            if (source.empty()) {
                whereHeapStorage_.clear();
                where_ = {};
                return true;
            }
            try {
                whereHeapStorage_.assign(source.data(), source.size());
            } catch (...) {
                // Instrumentation must not terminate the host process if an
                // owned diagnostic string cannot be allocated.
                whereHeapStorage_.clear();
                where_ = {};
                return false;
            }
            where_ = whereHeapStorage_;
            return true;
        }

        std::string_view where_; ///< Description of the scope being timed.
        std::string whereHeapStorage_;
        uint32_t threadNum_{0}; ///< Unique thread ID number.

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
        // Compact timer used by the hot-path macro. The public HotPathTag
        // constructor remains available for source compatibility.
        class HotPathTimer : private SinkRuntime, private TimerFormatting, private LabelStorage {
        public:
            explicit HotPathTimer(ScopeTimer::HotPathTag, LabelData label) noexcept {
                if (!ScopeTimer::enabled()) return;
                if (!assignLabel(std::move(label))) {
                    droppedRecords_.fetch_add(1U, std::memory_order_relaxed);
                    return;
                }
                active_ = true;
                started_ = std::chrono::steady_clock::now();
            }
            ~HotPathTimer() noexcept {
                if (!active_ || !canEmit()) return;
                const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now() - started_).count();
                auto& buffers = formatBuffers();
                const auto elapsedLen = formatElapsedNanos(elapsed, buffers.elapsedBuf, sizeof(buffers.elapsedBuf));
                auto& line = lineBuffer();
                emitLine(line.data, buildHotPathLogLine(line.data, sizeof(line.data), label_, buffers.elapsedBuf, elapsedLen));
            }
            HotPathTimer(const HotPathTimer&) = delete;
            HotPathTimer& operator=(const HotPathTimer&) = delete;
            HotPathTimer(HotPathTimer&&) = delete;
            HotPathTimer& operator=(HotPathTimer&&) = delete;
        private:
            std::chrono::steady_clock::time_point started_{};
            bool active_{false};
        };

        template <typename Timer = ScopeTimer>
        class ConditionalScopeTimer {
        public:
            template <typename Factory>
            ConditionalScopeTimer(bool condition, Factory&& construct) {
                if (condition && ScopeTimer::enabled()) {
                    // Construction happens within the factory's full expression,
                    // before temporary-backed label pointers/views expire.
                    construct(timer_);
                }
            }

            ~ConditionalScopeTimer() = default;
            ConditionalScopeTimer(const ConditionalScopeTimer&) = delete;
            ConditionalScopeTimer& operator=(const ConditionalScopeTimer&) = delete;
            ConditionalScopeTimer(ConditionalScopeTimer&&) = delete;
            ConditionalScopeTimer& operator=(ConditionalScopeTimer&&) = delete;

        private:
            std::optional<Timer> timer_;
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


#define ST_TIMER_IMPL(TimerType, condition, where, id, ...)                                 \
    ::xyzzy::scopetimer::detail::ConditionalScopeTimer<TimerType>                            \
        ST_CAT(scopeTimerInstance_, id)(                                                   \
            (condition), [&, ST_CAT(scopeTimerWhere_, id) = (where)](                       \
                auto& ST_CAT(scopeTimerStorage_, id)) {                                    \
                ST_CAT(scopeTimerStorage_, id).emplace(ST_CAT(scopeTimerWhere_, id),        \
                    ::xyzzy::scopetimer::detail::makeLabelData(__VA_ARGS__));               \
            })

#ifndef SCOPE_TIMER
#define SCOPE_TIMER(...)                                                                  \
    ST_TIMER_IMPL(::xyzzy::scopetimer::ScopeTimer, true,                                    \
        ::xyzzy::scopetimer::detail::makeBorrowedWhere(SCOPE_FUNCTION), ST_UNIQ, __VA_ARGS__)
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
    ST_TIMER_IMPL(::xyzzy::scopetimer::ScopeTimer, (cond),                                  \
        ::xyzzy::scopetimer::detail::makeBorrowedWhere(SCOPE_FUNCTION), ST_UNIQ, __VA_ARGS__)
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
#define SCOPE_TIMER_HOT_PATH(...)                                                           \
    ST_TIMER_IMPL(::xyzzy::scopetimer::detail::HotPathTimer, true,                           \
        ::xyzzy::scopetimer::ScopeTimer::HotPathTag{}, ST_UNIQ, __VA_ARGS__)
#endif

#else // Release build -> no-op

    /**
     * @brief No-op ScopeTimer class for release builds.
     *
     * Provides a matching interface to the debug ScopeTimer but performs no timing or logging.
     * This class uses C++20 std::string_view for parameter compatibility.
     */
    class ScopeTimer {
    public:
        struct HotPathTag {};

        static inline constexpr std::size_t DefaultSinkFlushBytes = 16U * 1024U;
        static inline constexpr std::size_t MaxSinkFlushBytes = 64U * 1024U * 1024U;

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
        inline explicit ScopeTimer(HotPathTag, std::string_view = "ScopeTimer") noexcept {}
        ScopeTimer(const ScopeTimer&) = delete;
        ScopeTimer& operator=(const ScopeTimer&) = delete;
        ScopeTimer(ScopeTimer&&) = delete;
        ScopeTimer& operator=(ScopeTimer&&) = delete;
        static inline void enableThreadBufferedSink(std::size_t = DefaultSinkFlushBytes) noexcept {}
        static inline void disableThreadBufferedSink() noexcept {}
        static inline void enableAsyncSink(std::size_t = DefaultSinkFlushBytes) noexcept {}
        static inline void disableAsyncSink() noexcept {}
        static inline void setLogSink(LogSink&) {}
        static inline void resetLogSink() {}
        static inline bool enabled() noexcept { return false; }
        static inline void flush() noexcept {}
        static inline std::uint64_t droppedRecords() noexcept { return 0U; }
    };

#ifndef SCOPE_TIMER
#define SCOPE_TIMER(...) \
    do { if constexpr (false) { __VA_OPT__(static_cast<void>(__VA_ARGS__);) } } while(0)
#endif

#ifndef SCOPE_TIMER_IF
// Do not evaluate cond or labels; the discarded branch preserves source
// compatibility and marks instrumentation-only variables as used.
#define SCOPE_TIMER_IF(cond, ...) \
    do { if constexpr (false) { static_cast<void>(cond); __VA_OPT__(static_cast<void>(__VA_ARGS__);) } } while(0)
#endif

#ifndef SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_ENABLE_THREAD_BUFFERED_SINK(...) \
    do { if constexpr (false) { __VA_OPT__(static_cast<void>(__VA_ARGS__);) } } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK
#define SCOPE_TIMER_DISABLE_THREAD_BUFFERED_SINK() \
    do { } while(0)
#endif

#ifndef SCOPE_TIMER_ENABLE_ASYNC_SINK
#define SCOPE_TIMER_ENABLE_ASYNC_SINK(...) \
    do { if constexpr (false) { __VA_OPT__(static_cast<void>(__VA_ARGS__);) } } while(0)
#endif

#ifndef SCOPE_TIMER_DISABLE_ASYNC_SINK
#define SCOPE_TIMER_DISABLE_ASYNC_SINK() \
    do { } while(0)
#endif

#ifndef SCOPE_TIMER_HOT_PATH
#define SCOPE_TIMER_HOT_PATH(...) \
    do { if constexpr (false) { __VA_OPT__(static_cast<void>(__VA_ARGS__);) } } while(0)
#endif

#endif // NDEBUG

} // namespace xyzzy::scopetimer

#ifndef NDEBUG
inline void xyzzy::scopetimer::detail::SinkRuntime::defaultSinkWrite(const char* data, std::size_t len) noexcept {
    // Fast path: nothing to write.
    if (len == 0) {
        return;
    }

    int fd = logFd();
    if (fd < 0) {
        // Attempt to open/create the log file lazily; if that fails we silently drop the line.
        if (!ensureLogFdOpen()) {
            recordDroppedPayload(data, len);
            return;
        }
        fd = logFd();
        if (fd < 0) {
            recordDroppedPayload(data, len);
            return;
        }
    }

    // Retry interrupted/partial writes and count any undeliverable tail.
    writeFdBestEffort(fd, data, len);
}

inline void xyzzy::scopetimer::detail::SinkRuntime::defaultSinkFlush() noexcept {
    // Default sink writes use unbuffered file descriptors, so periodic flush
    // has no userspace buffer to drain. Avoid forcing disk durability on the
    // timer hot path.
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
inline void xyzzy::scopetimer::detail::SinkRuntime::noopSinkFlush() noexcept {
    // Intentionally blank: used when tests inject a sink but do not need flush semantics.
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline void xyzzy::scopetimer::detail::SinkRuntime::threadBufferedSinkWrite(const char* data, std::size_t len) noexcept {
    if (len == 0) {
        return;
    }

    auto* buffer = threadLocalBuffer();
    if (buffer == nullptr) {
        recordDroppedPayload(data, len);
        return;
    }
    const std::size_t flushBytes = threadBufferFlushBytes();
    if (!ensureThreadBufferCapacity(*buffer, flushBytes)) {
        recordDroppedPayload(data, len);
        return;
    }

    if (len >= flushBytes) {
        flushThreadBuffer(*buffer);
        publishBufferedSinkPayload(data, len);
        return;
    }

    if (buffer->size + len > flushBytes) {
        flushThreadBuffer(*buffer);
        if (!ensureThreadBufferCapacity(*buffer, flushBytes)) {
            recordDroppedPayload(data, len);
            return;
        }
    }

    std::memcpy(buffer->data.data() + buffer->size, data, len);
    buffer->size += len;
    if (buffer->size >= flushBytes) {
        flushThreadBuffer(*buffer);
    }
}

inline void xyzzy::scopetimer::detail::SinkRuntime::threadBufferedSinkFlush() noexcept {
    if (auto* buffer = threadLocalBuffer()) {
        flushThreadBuffer(*buffer, BufferedSinkFlushMode::Forced);
    }
}

inline void xyzzy::scopetimer::detail::SinkRuntime::asyncSinkWrite(const char* data, std::size_t len) noexcept {
    if (len == 0) {
        return;
    }
    if (len > MaxAsyncSinkQueuedBytes) {
        return;
    }

    auto batch = acquireAsyncSinkBatch(len);
    if (!batch) {
        recordDroppedPayload(data, len);
        return;
    }
    std::memcpy(batch->data.data(), data, len);

    enqueueAsyncSinkBatch(std::move(*batch));
}

inline void xyzzy::scopetimer::detail::SinkRuntime::asyncSinkFlush() noexcept {
    if (isAsyncSinkWorkerThread()) {
        return;
    }
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
