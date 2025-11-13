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
#include <cctype>     // for std::toupper
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

#if defined(__GNUC__) || defined(__clang__)
#define SCOPE_FUNCTION __PRETTY_FUNCTION__
#else
#define SCOPE_FUNCTION __func__
#endif

// Always open the namespaces unconditionally, so they are present in all builds.
namespace xyzzy::scopetimer {

    class ScopeTimer_TestFriend; // Forward declaration

#ifndef NDEBUG // Debug build only

    // avoid a global variable altogether and expose a function‑local static via an inline accessor
    inline std::mutex& outMutex() noexcept {
        static std::mutex m;
        return m;
    }

    inline std::atomic<unsigned>& lineCounter() noexcept {
        static std::atomic<unsigned> c{0};
        return c;
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
        struct LabelData {
            std::string storage;
            std::string_view view{ "ScopeTimer" };

            LabelData() = default;

            explicit LabelData(std::string_view v, std::string&& owned = {}) noexcept
                : storage(std::move(owned)) {
                if (!storage.empty()) {
                    view = storage;
                } else if (!v.empty()) {
                    view = v;
                } else {
                    view = "ScopeTimer";
                }
            }
        };

        class LabelArg {
        public:
            LabelArg() = default;

            template <std::size_t N>
            explicit LabelArg(const char (&literal)[N]) noexcept
                : storage_(literal, N ? N - 1 : 0) {}

            explicit LabelArg(const char* s) noexcept
                : storage_(s && *s ? std::string_view{s} : std::string_view{"ScopeTimer"}) {}

            explicit LabelArg(std::string_view sv)
                : storage_(sv) {}

            explicit LabelArg(const std::string& s)
                : owned_(s),
                  ownsStorage_(true) {}

            explicit LabelArg(std::string&& s) noexcept
                : owned_(std::move(s)),
                  ownsStorage_(true) {}

            LabelData toLabelData() && noexcept {
                if (ownsStorage_) {
                    return LabelData(std::string_view{}, std::move(owned_));
                }
                return LabelData(storage_);
            }

        private:
            std::string_view storage_{ "ScopeTimer" };
            std::string owned_;
            bool ownsStorage_{ false };
        };
    } // namespace detail

    /**
     * @brief A high-resolution scope timer for measuring execution time of code blocks.
     *
     * This class uses C++17 features such as std::string_view for lightweight string parameters,
     * thread_local storage and std::atomic for lock-free thread ID assignment,
     * and fixed-size stack buffers for efficient formatting without heap allocation.
     * It logs timing information to a file with buffered output for high throughput.
     */
    class ScopeTimer {
    public:
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
            startWall_ = std::chrono::system_clock::now();
            formatTime(startWall_, startWallFormatted_, sizeof(startWallFormatted_));
        }

        /**
         * @brief Convenience overload that accepts a plain string_view label.
         */
        inline explicit ScopeTimer(std::string_view where, std::string_view label) noexcept
            : ScopeTimer(where, detail::LabelData{label}) {}

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
            const auto endWall   = std::chrono::system_clock::now();
            const auto elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endSteady - startSteady_).count();

            auto& fmtBufs = formatBuffers();
            formatTime(endWall, fmtBufs.endBuf, sizeof(fmtBufs.endBuf));
            formatElapsed(elapsedNs, fmtBufs.elapsedBuf, sizeof(fmtBufs.elapsedBuf));

            // Final line buffer reused per thread to avoid repeated stack allocation.
            auto& lineBuf = lineBuffer();

            // Build the log line
            int n = std::snprintf(
                lineBuf.data, sizeof(lineBuf.data),
                "[%.*s] TID=%03u | %.*s | start=%s | end=%s | elapsed=%s\n",
                static_cast<int>(label_.size()), label_.data(),
                threadNum_,
                static_cast<int>(where_.size()), where_.data(),
                startWallFormatted_,
                fmtBufs.endBuf,
                fmtBufs.elapsedBuf
            );

            // Convert snprintf result to a safe byte count
            const std::size_t len = ScopeTimerDetail::finalize_snprintf_result(n, lineBuf.data, sizeof(lineBuf.data));

            // Keep log lines non-interleaved (mutex only around IO)
            std::lock_guard lock(outMutex());

            if (len) {
                sinkWriteFn_(lineBuf.data, len);
            }

            // Periodic flush to keep logs visible even with large buffers
            unsigned cnt = lineCounter().fetch_add(1, std::memory_order_relaxed) + 1;

            if (cnt % flushInterval() == 0) { // configurable via SCOPE_TIMER_FLUSH_N
                sinkFlushFn_();
            }
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
                tid = next.fetch_add(1, std::memory_order_relaxed);
            }

            return tid;
        }

        /**
         * @brief Returns the periodic flush interval for the ScopeTimer log.
         *
         * Controlled by the environment variable `SCOPE_TIMER_FLUSH_N`.
         * If unset, non-numeric, or <= 0, defaults to 256. Parsed once and cached.
         *
         * @return unsigned The number of lines between flushes.
         */
        static inline unsigned flushInterval() noexcept {
            static const unsigned interval = []() noexcept {

                if(const char* p = std::getenv("SCOPE_TIMER_FLUSH_N")) {
                    char* end = nullptr;
                    unsigned long v = std::strtoul(p, &end, 10);

                    if(end != p && *end == '\0' && v > 0UL && v <= 1000000UL) {
                        return static_cast<unsigned>(v);
                    }
                }
                return 256U; // default
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
        using FormatterFn = void(*)(long long ns, char* out, size_t outSz) noexcept;

        // Portable localtime shim
#if defined(_WIN32)
#  define LOCALTIME(tm_ptr, time_ptr) localtime_s((tm_ptr), (time_ptr))
#else
#  define LOCALTIME(tm_ptr, time_ptr) localtime_r((time_ptr), (tm_ptr))
#endif

        /**
         * @brief Formats a system_clock time_point into a human-readable timestamp string.
         *
         * The format is "YYYY-MM-DD HH:MM:SS.mmm" with millisecond precision.
         *
         * @param tp The time_point to format.
         * @param out Buffer to write the formatted string into.
         * @param outSz Size of the output buffer.
         */
        static inline void formatTime(std::chrono::system_clock::time_point tp, char* out, size_t outSz) noexcept {
            const std::time_t tt = std::chrono::system_clock::to_time_t(tp);
            auto tm = std::tm{};
            LOCALTIME(&tm, &tt); // Use thread-safe localtime_r/localtime_s

            const auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
            const auto ms3 = static_cast<int>(ms_since_epoch % 1000);  // 0..999 fits in int 

            std::snprintf(
                out, 
                outSz, 
                "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                1900 + tm.tm_year, 
                1 + tm.tm_mon, 
                tm.tm_mday,
                tm.tm_hour, 
                tm.tm_min, 
                tm.tm_sec, 
                ms3
            );
        }

        /**
         * @brief Elapsed time formatters selected once for speed.
         *
         * Each formatter accepts elapsed nanoseconds and writes a formatted
         * string into the provided buffer. The active formatter is chosen once
         * from the SCOPE_TIMER_FORMAT environment variable and cached via a
         * function pointer to avoid branching in the hot path.
         */
        static inline void fmtSeconds(long long ns, char* out, size_t outSz) noexcept {
            const long long sec   = ns / 1000000000LL;
            const long long remMs = (ns / 1000000LL) % 1000LL; // 3 decimals
            std::snprintf(out, outSz, "%lld.%03llds", sec, remMs);
        }

        static inline void fmtMillis(long long ns, char* out, size_t outSz) noexcept {
            const long long ms    = ns / 1000000LL;
            const long long remUs = (ns / 1000LL) % 1000LL;   // 3 decimals for ms
            std::snprintf(out, outSz, "%lld.%03lldms", ms, remUs);
        }

        static inline void fmtMicros(long long ns, char* out, size_t outSz) noexcept {
            const long long us    = ns / 1000LL;
            const long long remNs = ns % 1000LL; // nanoseconds remainder
            std::snprintf(out, outSz, "%lld.%03lldus", us, remNs);
        }

        static inline void fmtNanos(long long ns, char* out, size_t outSz) noexcept {
            std::snprintf(out, outSz, "%lldns", ns);
        }

        /**
         * @brief Automatic elapsed time formatter.
         *
         * Selects the most appropriate unit: seconds (>=1s), milliseconds (>=1ms), microseconds (>=1us), or nanoseconds (<1us).
         * For explicit microseconds, use SCOPE_TIMER_FORMAT=MICROS.
         */
        static inline void fmtAuto(long long ns, char* out, size_t outSz) noexcept {
            if (ns >= 1000000000LL) {
                fmtSeconds(ns, out, outSz);
            } else if (ns >= 1000000LL) {
                fmtMillis(ns, out, outSz);
            } else if (ns >= 1000LL) {
                fmtMicros(ns, out, outSz);
            } else {
                fmtNanos(ns, out, outSz);
            }
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
        static inline void formatElapsed(long long ns, char* out, size_t outSz) noexcept {
            getFormatter()(ns, out, outSz);
        }

        struct FormatBuffers {
            char endBuf[32];
            char elapsedBuf[32];
        };

        static inline FormatBuffers& formatBuffers() noexcept {
            return tlsFormatBuffers_;
        }

        static inline const char* endBufferAddressForTests() noexcept {
            return formatBuffers().endBuf;
        }

        struct LineBuffer {
            char data[512];
        };

        static inline LineBuffer& lineBuffer() noexcept {
            return tlsLineBuffer_;
        }

        static void defaultSinkWrite(const char* data, std::size_t len) noexcept;
        static void defaultSinkFlush() noexcept;
        static void noopSinkFlush() noexcept;

        using SinkWriteFn = std::function<void(const char*, std::size_t)>;
        using SinkFlushFn = std::function<void()>;

        static inline SinkWriteFn sinkWriteFn_{
            [](const char* data, std::size_t len) {
                defaultSinkWrite(data, len);
            }
        };
        static inline SinkFlushFn sinkFlushFn_{
            [] {
                defaultSinkFlush();
            }
        };

        static inline void setLogSinkForTests(SinkWriteFn writeFn, SinkFlushFn flushFn) noexcept {
            if (writeFn) {
                closeLogFd();
                sinkWriteFn_ = std::move(writeFn);
                sinkFlushFn_ = flushFn ? std::move(flushFn) : SinkFlushFn{noopSinkFlush};
            } else {
                closeLogFd();
                sinkWriteFn_ = defaultSinkWrite;
                sinkFlushFn_ = defaultSinkFlush;
            }
        }

        inline void assignLabel(detail::LabelData&& data) noexcept {
            if (!data.storage.empty()) {
                labelStorage_ = std::move(data.storage);
                label_ = labelStorage_;
            } else if (!data.view.empty()) {
                label_ = data.view;
            } else {
                label_ = "ScopeTimer";
            }
        }

        std::string_view where_; ///< Description of the scope being timed.
        std::string_view label_{ "ScopeTimer" }; ///< Label for the log output.
        std::string labelStorage_; ///< Owns label storage when provided via temporaries.
        uint32_t threadNum_; ///< Unique thread ID number.

        static inline thread_local FormatBuffers tlsFormatBuffers_{};
        static inline thread_local LineBuffer tlsLineBuffer_{};
        static inline std::string logDirCache_{"/tmp/"};
        static inline bool logDirInitialized_{false};

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

            if (int newFd = ::open(path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644); newFd >= 0) {
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

        static inline void registerLogFdCleanup() noexcept {
            static bool registered = false;
            if (!registered) {
                std::atexit([]() noexcept {
                    closeLogFd();
                });
                registered = true;
            }
        }

        static inline int& logFd() noexcept {
            static int fd = -1;
            return fd;
        }

        static inline void closeLogFd() noexcept {
            int& fd = logFd();
            if (fd >= 0) {
                ::close(fd);
                fd = -1;
            }
        }

        static inline int defaultLogFdForTests() noexcept {
            return logFd();
        }

        static inline void closeLogFdForTests() noexcept {
            closeLogFd();
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
        SCOPE_FUNCTION, ::xyzzy::scopetimer::detail::LabelArg{ __VA_ARGS__ }.toLabelData())
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
            return ::xyzzy::scopetimer::detail::LabelArg{ __VA_ARGS__ }.toLabelData();       \
        })
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
        /**
         * @brief Constructs a no-op ScopeTimer.
         *
         * @param where Unused parameter describing the scope.
         * @param label Unused parameter for compatibility.
         */
        inline explicit ScopeTimer(std::string_view, std::string_view = "ScopeTimer") noexcept {}
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

#endif // NDEBUG

} // namespace xyzzy::scopetimer

// Back-compatibility alias so existing code that referenced ::ewm::scopetimer keeps working.
namespace ewm {
    namespace scopetimer = ::xyzzy::scopetimer;
}

inline void xyzzy::scopetimer::ScopeTimer::defaultSinkWrite(const char* data, std::size_t len) noexcept {
    if (len == 0) {
        return;
    }
    if (!ensureLogFdOpen()) {
        return;
    }
    int fd = logFd();
    if (fd < 0) {
        return;
    }
    ssize_t unused = ::write(fd, data, len);
    (void)unused;
}

inline void xyzzy::scopetimer::ScopeTimer::defaultSinkFlush() noexcept {
    int fd = logFd();
    if (fd >= 0) {
        ::fsync(fd);
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
