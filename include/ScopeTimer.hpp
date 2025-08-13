/* SteveC - 10Aug2025
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
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>
#include <string_view>

#if defined(__GNUC__) || defined(__clang__)
#define SCOPE_FUNCTION __PRETTY_FUNCTION__
#else
#define SCOPE_FUNCTION __func__
#endif

// Always open the namespaces unconditionally, so they are present in all builds.
namespace ewm {
namespace ewm_scopetimer {

#ifndef NDEBUG // Debug build only

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
         * @param label An optional std::string_view label for the log output (defaults to "ScopeTimer").
         */
        explicit ScopeTimer(std::string_view where, std::string_view label = "ScopeTimer") noexcept {
            if(isDisabled()) {
                disabled_ = true;
                return;
            }

            where_ = where;
            label_ = label;
            threadNum_ = getThreadIdNumber();
            startSteady_ = std::chrono::steady_clock::now();
            startWall_ = std::chrono::system_clock::now();
        }

        ScopeTimer(const ScopeTimer&) = delete; ///< Deleted copy constructor for safety.
        ScopeTimer& operator=(const ScopeTimer&) = delete; ///< Deleted copy assignment operator.

        /**
         * @brief Destructor that records the end time, computes elapsed duration,
         * formats timing information, and writes a log entry.
         *
         * Logs include thread ID, scope name, start and end timestamps, and elapsed time.
         */
        ~ScopeTimer() noexcept {
            if(disabled_) {
                return;
            }

            const auto endSteady = std::chrono::steady_clock::now();
            const auto endWall = std::chrono::system_clock::now();
            const auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(endSteady - startSteady_).count();

            // Use fixed-size stack buffers to avoid heap allocation
            char startBuf[32];
            char endBuf[32];
            char elapsedBuf[32];

            formatTime(startWall_, startBuf, sizeof(startBuf));
            formatTime(endWall, endBuf, sizeof(endBuf));
            formatElapsed(elapsedUs, elapsedBuf, sizeof(elapsedBuf));

            char line[512];
            const int n = std::snprintf(
                line,
                sizeof(line),
                "[%.*s] TID=%03u | %.*s | start=%s | end=%s | elapsed=%s\n",
                static_cast<int>(label_.size()), label_.data(),
                threadNum_,
                static_cast<int>(where_.size()), where_.data(),
                startBuf,
                endBuf,
                elapsedBuf);

            // Mutex to keep log lines non-interleaved, but only for output
            static std::mutex outMutex;
            std::lock_guard<std::mutex> lock(outMutex);

            if(FILE* fp = logFile()) {
                std::fwrite(line, 1, (n > 0 ? static_cast<size_t>(n) : std::strlen(line)), fp);

                // Periodic flush to keep logs visible even with large buffers
                static std::atomic<unsigned> lineCounter{ 0 };
                unsigned cnt = lineCounter.fetch_add(1, std::memory_order_relaxed) + 1;

                if(cnt % flushInterval() == 0) { // flush every N lines (configurable via SCOPE_TIMER_FLUSH_N)
                    std::fflush(fp);
                }
            }
        }

        /**
         * @brief Checks if the ScopeTimer is disabled based on the SCOPE_TIMER environment variable.
         *
         * Reads the environment variable once and caches the result for subsequent calls.
         * Returns true if the value is "OFF", "FALSE", "NO", or "0" (case-insensitive).
         *
         * @return true if disabled, false otherwise.
         */
        static inline bool isDisabled() noexcept {
            static const bool disabled = []() -> bool {
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

    private:
        /**
         * @brief Retrieves a unique thread ID number in a lock-free manner.
         *
         * Uses thread_local storage to cache the thread ID, and an atomic counter
         * to assign unique IDs starting from 1.
         *
         * @return uint32_t The unique thread ID number.
         */
        static uint32_t getThreadIdNumber() noexcept {
            thread_local uint32_t tid = 0;

            if(tid == 0) {
                static std::atomic<uint32_t> next{ 1 };
                tid = next.fetch_add(1, std::memory_order_relaxed);
            }

            return tid;
        }

        // Accessor for the singleton FILE* so we can flush/close at process exit.
        static FILE*& fileHandle() noexcept {
            static FILE* fp{ nullptr };
            return fp;
        }

        /**
         * @brief Returns the periodic flush interval for the ScopeTimer log.
         *
         * Controlled by the environment variable `SCOPE_TIMER_FLUSH_N`.
         * If unset, non-numeric, or <= 0, defaults to 256. Parsed once and cached.
         *
         * @return unsigned The number of lines between flushes.
         */
        static unsigned flushInterval() noexcept {
            static const unsigned interval = []() noexcept -> unsigned {
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
         * @brief Provides a singleton FILE* for logging with a large user buffer.
         *
         * The log file path is controlled by the SCOPE_TIMER_DIR environment variable,
         * defaulting to /tmp/ScopeTimer.log. Uses a 1 MiB buffer for high-throughput buffered IO.
         *
         * @return FILE* Pointer to the opened log file, or nullptr on failure.
         */
        static FILE* logFile() noexcept {
            FILE*& fp = fileHandle();
            if(fp) {
                return fp;
            }
            // Singleton FILE* with large user buffer for high-throughput buffered IO
            const char* env = std::getenv("SCOPE_TIMER_DIR");
            std::string dir = (env && *env) ? std::string(env) : std::string("/tmp");
            if(!dir.empty() && dir.back() != '/') {
                dir.push_back('/');
            }
            std::string path = dir + "ScopeTimer.log";
            FILE* f = std::fopen(path.c_str(), "a");
            if(f) {
                // Use a static buffer for the FILE*, 1 MiB for throughput
                static char* buf = new char[1 << 20];
                std::setvbuf(f, buf, _IOFBF, 1 << 20);
                fp = f;
                // Ensure flush+close at process shutdown
                static bool registered = false;
                if(!registered) {
                    std::atexit([]() noexcept {
                        FILE* fh = fileHandle();
                        if(fh) {
                            std::fflush(fh);
                            std::fclose(fh);
                            // do not reset fh here; process is exiting
                        }
                    });
                    registered = true;
                }
            }
            return fp;
        }

        /**
         * @brief Formats a system_clock time_point into a human-readable timestamp string.
         *
         * The format is "YYYY-MM-DD HH:MM:SS.mmm" with millisecond precision.
         *
         * @param tp The time_point to format.
         * @param out Buffer to write the formatted string into.
         * @param outSz Size of the output buffer.
         */
        static void formatTime(std::chrono::system_clock::time_point tp, char* out, size_t outSz) noexcept {
            const std::time_t tt = std::chrono::system_clock::to_time_t(tp);
            std::tm tm{};
            localtime_r(&tt, &tm);

            const int ms3 = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000);

            std::snprintf(out, outSz, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                          1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                          tm.tm_hour, tm.tm_min, tm.tm_sec, ms3);
        }

        /**
         * @brief Formats elapsed time in microseconds into a human-readable string.
         *
         * Outputs seconds with millisecond precision if >= 1 second,
         * milliseconds with microsecond precision if >= 1 millisecond,
         * or microseconds otherwise.
         *
         * @param us Elapsed time in microseconds.
         * @param out Buffer to write the formatted string into.
         * @param outSz Size of the output buffer.
         */
        static void formatElapsed(long long us, char* out, size_t outSz) noexcept {
            const long long ms = us / 1000;
            const long long remUs = us % 1000;

            if(ms >= 1000) {
                const long long sec = ms / 1000;
                const long long remMs = ms % 1000;
                std::snprintf(out, outSz, "%lld.%03llds", sec, remMs);
            } else if(ms > 0) {
                std::snprintf(out, outSz, "%lld.%03lldms", ms, remUs);
            } else {
                std::snprintf(out, outSz, "%lldus", us);
            }
        }

        std::string_view where_; ///< Description of the scope being timed.
        std::string_view label_{ "ScopeTimer" }; ///< Label for the log output.
        uint32_t threadNum_; ///< Unique thread ID number.

        /**
         * I store both a steady_clock (startSteady_) and a system_clock (startWall_) timestamp:
         * - startSteady_: Used for computing precise elapsed durations, immune to system clock changes.
         * - startWall_: Used for logging human-readable absolute start times for contextual information.
         * This dual tracking ensures logs contain meaningful wall times while preserving accurate duration measurements.
         */
        std::chrono::steady_clock::time_point startSteady_; ///< Start time for high-resolution elapsed duration.

        /**
         * @brief Stores the real-world wall clock timestamp at timer start.
         *
         * This is useful for logging absolute times in human-readable form (e.g., for log file timestamps).
         * Unlike startSteady_ (which uses steady_clock for precise elapsed timing unaffected by system clock changes),
         * startWall_ captures the actual wall clock time for contextual/logging purposes.
         */
        std::chrono::system_clock::time_point startWall_;

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

    // Helper for optional label argument, to avoid use of GNU extension '##__VA_ARGS__'
    namespace detail {
        struct LabelArg {
            std::string_view v{ "ScopeTimer" };
            LabelArg() = default;
            LabelArg(std::string_view s)
                : v(s) {}
            operator std::string_view() const noexcept {
                return v;
            }
        };
    } // namespace detail

#ifndef SCOPE_TIMER
#define SCOPE_TIMER(...)                                    \
    ::ewm::ewm_scopetimer::ScopeTimer scopeTimerInstance__( \
        SCOPE_FUNCTION, ::ewm::ewm_scopetimer::detail::LabelArg{ __VA_ARGS__ })
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
        explicit ScopeTimer(std::string_view, std::string_view = "ScopeTimer") noexcept {}
    };

#ifndef SCOPE_TIMER
#define SCOPE_TIMER(...) \
    do {                 \
    } while(0)
#endif

#endif // NDEBUG

} // namespace ewm_scopetimer
} // namespace ewm
