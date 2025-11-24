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
#include <chrono>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <utility>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <cerrno>
#include <fcntl.h>

using namespace std::chrono_literals;

// All unit tests are now methods of a class that can be declared as a
// friend inside ScopeTimer.hpp (e.g., `friend class ScopeTimer_TestFriend;`).
// We DO NOT modify ScopeTimer.hpp here.
namespace xyzzy { namespace scopetimer {

class ScopeTimer_TestFriend {
public:
    // Entry point that runs the whole suite and returns the number of failures.
    static int run_all(int argc, char** argv) {
        init_exe_path(argc, argv);
        if (int rc = child_probe_main_if_requested(); rc == 0) return 0;

        test_is_disabled_env_non_disabled_branch();
        test_simple_scope();
        test_nested_scopes();
        test_conditional_timer();
        test_conditional_timer_spans_scope();
        test_parse_elapsed_millis_invalid_inputs();
        test_read_elapsed_millis_missing_file();
        test_child_probe_unknown_mode();
        test_init_exe_path_default_path();
        test_labelarg_temporary_string();
        test_labelarg_literal_and_pointer_variants();
        test_labeldata_manual_empty_view();
        test_labelarg_empty_literal_to_labeldata();
        test_labeldata_constructor_default_view();
        test_labelarg_owned_to_labeldata();
        test_log_directory_caching();
        test_memory_sink_captures_output();
        test_memory_sink_without_flush();
        test_default_sink_write_short_circuits();
        test_ensure_log_fd_reuses_existing_handle();
        test_default_sink_write_handles_closed_fd();
        test_label_storage_uses_local_buffer();
        test_labelarg_pointer_copies_input();
        test_threadlocal_format_buffers_reused();
        test_scope_timer_string_view_ctor();
        test_looped_work();
        test_threaded();
        test_env_format_variants();
        test_env_format_unset_and_empty();
        test_empty_label();
        test_seconds_format_branch();
        test_fmt_auto_seconds_branch();
        test_fmt_auto_nanos_branch();
        test_finalize_snprintf_result_branches();
        test_disabled_via_env_child_process();
        test_disabled_case_insensitivity_child_process();
        test_bad_env_values_child_process();
        test_flushN_variants_child_process();
        test_logdir_edge_cases_child_process();
        test_logfile_null_branch();
        test_logfile_failure_cache_branch();
        test_log_fd_has_cloexec();

        if (s_failures == 0) {
            std::fprintf(stdout, "All ScopeTimer tests passed.\n");
        }
        return s_failures;
    }

private:
    // ------------------ tiny test harness ------------------
    static inline void expect(bool cond, const char* msg) {
        if (!cond) {
            std::fprintf(stderr, "FAIL: %s\n", msg);
            ++s_failures;
        } else {
            std::fprintf(stdout, "OK  : %s\n", msg);
        }
    }

    static inline void busyFor(std::chrono::microseconds us) {
        std::this_thread::sleep_for(us);
    }

    static void verifyLabelResult(const char* scenario, std::string_view expected, bool expectOwned, ::xyzzy::scopetimer::detail::LabelArg arg) {
        ::xyzzy::scopetimer::ScopeTimer timer("tests:label:probe", std::move(arg).toLabelData());
        bool owned = !timer.labelStorage_.empty();
        std::string ownMsg = std::string(scenario) + " (ownership)";
        expect(owned == expectOwned, ownMsg.c_str());
        std::string txtMsg = std::string(scenario) + " (text)";
        expect(timer.label_ == expected, txtMsg.c_str());
        timer.disabled_ = true; // avoid destructor logging so tests stay quiet
    }

    static inline std::string& sinkCaptureBuffer() {
        static std::string buf;
        return buf;
    }

    static void testSinkWrite(const char* data, std::size_t len) noexcept {
        sinkCaptureBuffer().append(data, len);
    }

    static void testSinkFlush() noexcept {}

    static double parseElapsedMillis(const std::string& line) {
        const std::string needle = "elapsed=";
        const auto pos = line.find(needle);
        if (pos == std::string::npos) {
            return -1.0;
        }
        const auto valuePos = pos + needle.size();
        const auto endPos = line.find_first_of(" \t\r\n", valuePos);
        const std::string token = line.substr(valuePos, endPos == std::string::npos ? std::string::npos : endPos - valuePos);

        if (token.size() < 2 || token.substr(token.size() - 2) != "ms") {
            return -1.0;
        }

        std::string numeric = token.substr(0, token.size() - 2);
        char* endPtr = nullptr;
        const double value = std::strtod(numeric.c_str(), &endPtr);
        if (!endPtr || *endPtr != '\0') {
            return -1.0;
        }
        return value;
    }

    static double readElapsedMillisFromLog(const std::string& path, const std::string& label) {
        std::ifstream in(path);
        if (!in) {
            return -1.0;
        }
        std::string line;
        double latest = -1.0;
        while (std::getline(in, line)) {
            if (line.find(label) != std::string::npos) {
                const double candidate = parseElapsedMillis(line);
                if (candidate >= 0.0) {
                    latest = candidate;
                }
            }
        }
        return latest;
    }

    static std::string shellEscape(const std::string& s) {
        std::string out = "'";
        for (char c : s) {
            if (c == '\'') { out += "'\\''"; } else { out += c; }
        }
        out += "'";
        return out;
    }

    // ------------------ individual tests ------------------
    static void test_is_disabled_env_non_disabled_branch() {
        ::setenv("SCOPE_TIMER", "maybe", 1);
        bool disabled = ScopeTimer::isDisabled();
        expect(!disabled, "isDisabled returns false for unrecognized env value");
        ::unsetenv("SCOPE_TIMER");
    }

    static void test_simple_scope() {
        // These log assertions hit the real filesystem because ScopeTimer's contract is to
        // append plain-text entries; parsing the actual log exercises the same path end users get.
        const std::string logDir = "/tmp";
        const std::string logPath = logDir + "/ScopeTimer.log";
        std::remove(logPath.c_str());
        ::setenv("SCOPE_TIMER_DIR", logDir.c_str(), 1);
        ::setenv("SCOPE_TIMER_FLUSH_N", "1", 1);
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests(logDir);
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        {
            SCOPE_TIMER("tests:simple_scope");
            busyFor(2000us);
        }

        double elapsedMs = readElapsedMillisFromLog(logPath, "tests:simple_scope");
        expect(elapsedMs >= 2.0, "simple scope emitted log entry with elapsed >= 2ms");
        std::remove(logPath.c_str());
    }

    static void test_nested_scopes() {
        SCOPE_TIMER("tests:nested:outer");
        busyFor(10us);
        {
            SCOPE_TIMER("tests:nested:inner1");
            busyFor(15us);
        }
        {
            SCOPE_TIMER("tests:nested:inner2");
            busyFor(20us);
        }
        expect(true, "nested scopes executed");
    }

    static void test_conditional_timer() {
        SCOPE_TIMER_IF(true, "tests:conditional:on");
        busyFor(5us);
        SCOPE_TIMER_IF(false, "tests:conditional:off");
        busyFor(5us);
        expect(true, "conditional timer executed");
    }

    static void test_conditional_timer_spans_scope() {
        auto runScenario = [](bool forceFallbackOnly) {
            char templ[] = "/tmp/scopetimer_ifXXXXXX";
            char* dir = ::mkdtemp(templ);
            std::string tmpdir;
            bool cleanupDir = false;

            if (forceFallbackOnly && dir) {
                ::rmdir(dir);
                dir = nullptr;
            }

            if (dir) {
                tmpdir = dir;
                cleanupDir = true;
            } else {
                tmpdir = "/tmp/scopetimer_if_fallback_" + std::to_string(::getpid());
                if (::mkdir(tmpdir.c_str(), 0700) == 0) {
                    cleanupDir = true;
                } else if (errno == EEXIST) {
                    cleanupDir = false;
                } else {
                    tmpdir = "/tmp";
                    cleanupDir = false;
                }
            }

            if (forceFallbackOnly) {
                if (cleanupDir && tmpdir != "/tmp") {
                    ::rmdir(tmpdir.c_str());
                }
                return;
            }

            const std::string logfile = tmpdir + "/ScopeTimer.log";
            std::remove(logfile.c_str());

            int rc = run_child_with_env({
                {"SCOPETIMER_PROBE", "if_scope"},
                {"SCOPE_TIMER_DIR", tmpdir},
                {"SCOPE_TIMER_FORMAT", "MILLIS"},
                {"SCOPE_TIMER_FLUSH_N", "1"}
            });
            expect(rc == 0, "child process for conditional timer probe exited cleanly");

            const double elapsedMs = readElapsedMillisFromLog(logfile, "tests:conditional:lifetime");
            expect(elapsedMs >= 5.0, "SCOPE_TIMER_IF spans enclosing scope");

            std::remove(logfile.c_str());
            if (cleanupDir && tmpdir != "/tmp") {
                ::rmdir(tmpdir.c_str());
            }
        };

        runScenario(true);
        runScenario(false);
    }

    static void test_labeldata_manual_empty_view() {
        ::xyzzy::scopetimer::detail::LabelData data;
        data.view = std::string_view{};
        ::xyzzy::scopetimer::ScopeTimer timer("tests:labeldata:empty", std::move(data));
        expect(timer.label_ == std::string_view("ScopeTimer"), "LabelData empty view defaults to ScopeTimer");
        expect(timer.labelStorage_.empty(), "LabelData empty view does not allocate storage");
        timer.disabled_ = true;
    }

    static void test_labelarg_empty_literal_to_labeldata() {
        ::xyzzy::scopetimer::detail::LabelArg arg{""};
        auto data = std::move(arg).toLabelData();
        expect(data.view == std::string_view("ScopeTimer"), "LabelArg empty literal defaults to ScopeTimer");
        expect(data.storage.empty(), "LabelArg empty literal does not allocate storage");
    }

    static void test_labeldata_constructor_default_view() {
        ::xyzzy::scopetimer::detail::LabelData data(std::string_view{});
        expect(data.view == std::string_view("ScopeTimer"), "LabelData ctor defaults empty inputs to ScopeTimer");
        expect(data.storage.empty(), "LabelData ctor leaves storage empty when no ownership provided");
    }

    static void test_labelarg_owned_to_labeldata() {
        std::string ownedSource = "tests:label:owned";
        ::xyzzy::scopetimer::detail::LabelArg arg{std::move(ownedSource)};
        auto data = std::move(arg).toLabelData();
        expect(data.storage == "tests:label:owned", "LabelArg owned string moves storage");
        expect(data.view == data.storage, "LabelArg owned string view references storage");
    }

    static void test_log_directory_caching() {
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp/cached_dir");
        const std::string& first = ::xyzzy::scopetimer::ScopeTimer::logDirectory();
        expect(first == "/tmp/cached_dir/", "logDirectory uses test override with trailing slash");

        ::setenv("SCOPE_TIMER_DIR", "/tmp/ignored_change", 1);
        const std::string& second = ::xyzzy::scopetimer::ScopeTimer::logDirectory();
        expect(&first == &second, "logDirectory returns same cached reference after override");
        expect(second == "/tmp/cached_dir/", "logDirectory ignores env changes after override");

        ::unsetenv("SCOPE_TIMER_DIR");
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests();
    }

    static void test_threadlocal_format_buffers_reused() {
        const char* first = ::xyzzy::scopetimer::ScopeTimer::endBufferAddressForTests();
        const char* second = ::xyzzy::scopetimer::ScopeTimer::endBufferAddressForTests();
        expect(first == second, "thread-local format buffers reuse the same memory per thread");
    }

    static void test_memory_sink_captures_output() {
        sinkCaptureBuffer().clear();
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(&testSinkWrite, &testSinkFlush);
        {
            SCOPE_TIMER("tests:memory_sink");
            busyFor(150us);
        }
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        expect(sinkCaptureBuffer().find("tests:memory_sink") != std::string::npos,
               "custom log sink captured scope output");
    }

    static void test_memory_sink_without_flush() {
        sinkCaptureBuffer().clear();
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(&testSinkWrite, nullptr);
        {
            SCOPE_TIMER("tests:memory_sink_no_flush");
            busyFor(50us);
        }
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        expect(sinkCaptureBuffer().find("tests:memory_sink_no_flush") != std::string::npos,
               "custom log sink without flush still captures output");
    }

    static void test_default_sink_write_short_circuits() {
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr); // ensure default sink active
        ::xyzzy::scopetimer::ScopeTimer::defaultSinkWrite("ignored", 0);

        std::string bogus = "/tmp/scopetimer_default_sink_" + std::to_string(::getpid());
        ::setenv("SCOPE_TIMER_DIR", bogus.c_str(), 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests(bogus);
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        ::xyzzy::scopetimer::ScopeTimer::defaultSinkWrite("abc", 3);
        expect(::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests() == -1,
               "default sink write leaves fd closed when directory invalid");

        ::setenv("SCOPE_TIMER_DIR", "/tmp", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp");
    }

    static void test_ensure_log_fd_reuses_existing_handle() {
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        ::setenv("SCOPE_TIMER_DIR", "/tmp", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp");
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        bool opened = ::xyzzy::scopetimer::ScopeTimer::ensureLogFdOpen();
        expect(opened, "ensureLogFdOpen opens fd for valid directory");
        int fd = ::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests();
        expect(fd >= 0, "default sink exposes opened fd");

        bool reused = ::xyzzy::scopetimer::ScopeTimer::ensureLogFdOpen();
        expect(reused, "ensureLogFdOpen returns true when fd already open");
        expect(::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests() == fd,
               "ensureLogFdOpen leaves existing fd untouched");

        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();
    }

    static void test_default_sink_write_handles_closed_fd() {
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        std::string bogus = "/tmp/scopetimer_default_sink_fail_" + std::to_string(::getpid());
        ::setenv("SCOPE_TIMER_DIR", bogus.c_str(), 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests(bogus);
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        ::xyzzy::scopetimer::ScopeTimer::defaultSinkWrite("xyz", 3);
        expect(::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests() == -1,
               "defaultSinkWrite returns quickly when fd cannot open");

        ::setenv("SCOPE_TIMER_DIR", "/tmp", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp");
    }

    static void test_label_storage_uses_local_buffer() {
        sinkCaptureBuffer().clear();
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(&testSinkWrite, &testSinkFlush);
        std::string dynamicLabel = "tests:pmr_label:" + std::to_string(::getpid());
        {
            ::xyzzy::scopetimer::ScopeTimer timer("tests:pmr_scope", dynamicLabel);
            auto* resource = ::xyzzy::scopetimer::ScopeTimer::labelAllocatorResourceForTests(timer);
            auto* local = ::xyzzy::scopetimer::ScopeTimer::localLabelResourceForTests(timer);
            expect(resource == local, "pmr allocator matches the timer-local buffer resource");
        }
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
    }

    static void test_scope_timer_string_view_ctor() {
        std::string_view svLabel = "tests:label:ctor_sv";
        ::xyzzy::scopetimer::ScopeTimer timer("tests:label:ctor_scope", svLabel);
        expect(timer.label_ == svLabel, "ScopeTimer string_view ctor copies label text");
        expect(timer.labelStorage_.empty(), "ScopeTimer string_view ctor reuses provided storage");
        timer.disabled_ = true;
    }

    static void test_labelarg_temporary_string() {
        verifyLabelResult("temporary std::string rvalue", "tests:label:temporary",
                          true, ::xyzzy::scopetimer::detail::LabelArg{std::string("tests:label:temporary")});

        std::string lvalue = "tests:label:lvalue";
        verifyLabelResult("std::string lvalue copy", "tests:label:lvalue",
                          true, ::xyzzy::scopetimer::detail::LabelArg{lvalue});

        std::string moveSrc = "tests:label:moved";
        verifyLabelResult("std::string rvalue move", "tests:label:moved",
                          true, ::xyzzy::scopetimer::detail::LabelArg{std::move(moveSrc)});

        std::string_view sv = "tests:label:sv";
        verifyLabelResult("std::string_view copy", "tests:label:sv",
                          false, ::xyzzy::scopetimer::detail::LabelArg{sv});
    }

    static void test_labelarg_literal_and_pointer_variants() {
        verifyLabelResult("string literal", "tests:label:literal",
                          true, ::xyzzy::scopetimer::detail::LabelArg{"tests:label:literal"});

        verifyLabelResult("string literal empty", "ScopeTimer",
                          false, ::xyzzy::scopetimer::detail::LabelArg{""});

        const char* ptr = "tests:label:ptr";
        verifyLabelResult("const char* pointer", "tests:label:ptr",
                          true, ::xyzzy::scopetimer::detail::LabelArg{ptr});

        const char* emptyPtr = "";
        verifyLabelResult("const char* empty string", "ScopeTimer",
                          false, ::xyzzy::scopetimer::detail::LabelArg{emptyPtr});

        const char* nullPtr = nullptr;
        verifyLabelResult("const char* null pointer", "ScopeTimer",
                          false, ::xyzzy::scopetimer::detail::LabelArg{nullPtr});

        verifyLabelResult("default LabelArg", "ScopeTimer",
                          false, ::xyzzy::scopetimer::detail::LabelArg{});
    }

    static void test_labelarg_pointer_copies_input() {
        std::string src = "tests:label:ephemeral";
        const char* ptr = src.c_str();
        ::xyzzy::scopetimer::detail::LabelArg arg{ptr};
        src.clear(); // the original storage now differs; LabelArg should have copied
        auto data = std::move(arg).toLabelData();
        expect(data.storage == "tests:label:ephemeral", "LabelArg copies const char* to owned storage");
        expect(data.view == data.storage, "LabelArg const char* view references owned storage");
    }

    static void test_parse_elapsed_millis_invalid_inputs() {
        double val = parseElapsedMillis("no elapsed field");
        expect(val < 0.0, "parseElapsedMillis returns -1 when marker missing");

        val = parseElapsedMillis("elapsed=12us");
        expect(val < 0.0, "parseElapsedMillis returns -1 when units not ms");

        val = parseElapsedMillis("[ScopeTimer] elapsed=abcdms");
        expect(val < 0.0, "parseElapsedMillis returns -1 when numeric parse fails");
    }

    static void test_read_elapsed_millis_missing_file() {
        std::string path = "/tmp/scopetimer_missing_" + std::to_string(::getpid()) + ".log";
        std::remove(path.c_str());
        double val = readElapsedMillisFromLog(path, "tests:missing");
        expect(val < 0.0, "readElapsedMillisFromLog returns -1 when file missing");
    }

    static void test_looped_work() {
        SCOPE_TIMER("tests:looped:total");
        for (int i = 0; i < 1000; ++i) {
            SCOPE_TIMER("tests:looped:iter");
            busyFor(5us);
        }
        expect(true, "looped work executed");
    }

    static void test_threaded() {
        SCOPE_TIMER("tests:threaded:total");
        std::vector<std::thread> tg;
        for (int i = 0; i < 1000; ++i) {
            tg.emplace_back([i]() {
                SCOPE_TIMER("tests:threaded:worker");
                std::this_thread::sleep_for(std::chrono::microseconds{10 + i * 5});
            });
        }
        for (auto& t : tg) t.join();
        expect(true, "threaded work executed");
    }

    static void test_env_format_variants() {
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);
        { SCOPE_TIMER("tests:format:micros"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "MILLIS", 1);
        { SCOPE_TIMER("tests:format:millis"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "SECONDS", 1);
        { SCOPE_TIMER("tests:format:seconds"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "NANOS", 1);
        { SCOPE_TIMER("tests:format:nanos"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "BOGUS", 1);
        { SCOPE_TIMER("tests:format:bogus"); busyFor(1us); }
        expect(true, "env format variants executed");
    }

    static void test_env_format_unset_and_empty() {
        ::unsetenv("SCOPE_TIMER_FORMAT");
        { SCOPE_TIMER("tests:format:unset"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "", 1);
        { SCOPE_TIMER("tests:format:empty"); busyFor(1us); }
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);
        expect(true, "env format unset/empty executed");
    }

    static void test_empty_label() {
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);
        { SCOPE_TIMER(""); busyFor(1us); }
        expect(true, "empty label handled");
    }

    static void test_seconds_format_branch() {
        ::setenv("SCOPE_TIMER_FORMAT", "SECONDS", 1);
        { SCOPE_TIMER("tests:format:seconds:zero"); busyFor(0us); }
        { SCOPE_TIMER("tests:format:seconds:short"); busyFor(1100us); }
        { SCOPE_TIMER("tests:format:seconds:long"); busyFor(10ms); }
        expect(true, "seconds format branch executed");
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);
    }

    // --- tests that touch internal helpers via namespace ---
    static void test_fmt_auto_seconds_branch() {
        char buf[64];
        long long ns = 2500000000LL; // 2.5s -> seconds branch
        for (auto &c : buf) c = '\0';
        xyzzy::scopetimer::ScopeTimer::fmtAuto(ns, buf, sizeof(buf));
        const std::string out(buf);
        expect(out.find("ms") == std::string::npos, "fmtAuto(seconds): not milliseconds");
        expect(out.find("us") == std::string::npos, "fmtAuto(seconds): not microseconds");
        expect(out.find("ns") == std::string::npos, "fmtAuto(seconds): not nanoseconds");
        expect(out.find('s') != std::string::npos,   "fmtAuto(seconds): seconds unit present");
    }

    static void test_fmt_auto_nanos_branch() {
        char buf[64];
        long long ns = 500; // < 1000 ns triggers fmtNanos
        for (auto &c : buf) c = '\0';
        xyzzy::scopetimer::ScopeTimer::fmtAuto(ns, buf, sizeof(buf));
        const std::string out(buf);
        expect(out.find("ns") != std::string::npos, "fmtAuto(nanos): nanoseconds unit present");
        expect(out.find("us") == std::string::npos, "fmtAuto(nanos): not microseconds");
        expect(out.find("ms") == std::string::npos, "fmtAuto(nanos): not milliseconds");
        expect(out.find('s') == std::string::npos || out.find("ns") != std::string::npos,
               "fmtAuto(nanos): no plain seconds suffix");
    }

    static void test_finalize_snprintf_result_branches() {
        using xyzzy::scopetimer::ScopeTimerDetail::finalize_snprintf_result;
        {
            char buf[8]; for (auto &c : buf) c = 'X';
            size_t len = finalize_snprintf_result(-1, buf, sizeof(buf));
            expect(len == 0, "finalize_snprintf_result: n<0 returns 0");
            expect(buf[0] == '\0', "finalize_snprintf_result: n<0 zero-terminates at [0]");
        }
        {
            char buf[5]; for (auto &c : buf) c = 'Y';
            size_t len = finalize_snprintf_result(100, buf, sizeof(buf));
            expect(len == sizeof(buf) - 1, "finalize_snprintf_result: truncation returns size-1");
            expect(buf[len] == '\0', "finalize_snprintf_result: truncation enforces terminator at end");
        }
        {
            char buf[16]; for (auto &c : buf) c = 'Z';
            const int n = 5;
            size_t len = finalize_snprintf_result(n, buf, sizeof(buf));
            expect(len == static_cast<size_t>(n), "finalize_snprintf_result: exact length returned");
        }
    }

    // --- child process helpers (probe mode) ---
    static int child_probe_main_if_requested() {
        const char* probe = ::getenv("SCOPETIMER_PROBE");
        if (!probe) return -1;
        const std::string mode = probe;
        if (mode == "1") {
            SCOPE_TIMER("tests:child:probe");
            busyFor(100us);
            return 0;
        }
        if (mode == "if_scope") {
            SCOPE_TIMER_IF(true, "tests:conditional:lifetime");
            busyFor(20000us);
            return 0;
        }
        return -1;
    }

    static void test_child_probe_unknown_mode() {
        ::setenv("SCOPETIMER_PROBE", "unknown-mode", 1);
        int rc = child_probe_main_if_requested();
        expect(rc == -1, "child_probe_main_if_requested returns -1 for unknown probe mode");
        ::unsetenv("SCOPETIMER_PROBE");
    }

    static int run_child_with_env(const std::vector<std::pair<std::string,std::string>>& envs) {
        std::string envBlock;
        bool probeSet = false;
        for (const auto& kv : envs) {
            if (kv.first == "SCOPETIMER_PROBE") {
                probeSet = true;
            }
            envBlock += kv.first + "=" + kv.second + " ";
        }
        if (!probeSet) {
            envBlock = std::string("SCOPETIMER_PROBE=1 ") + envBlock;
        }
        std::string cmd = envBlock + shellEscape(s_exe_path);
        cmd += " >/dev/null 2>&1";
        return std::system(cmd.c_str());
    }

    static void test_disabled_via_env_child_process() {
        char templ[] = "/tmp/scopetimerXXXXXX";
        char* tdir = ::mkdtemp(templ);
        std::string tmpdir = tdir ? std::string(tdir) : std::string("/tmp");
        int rc = run_child_with_env({{"SCOPE_TIMER","0"},{"SCOPE_TIMER_FORMAT","MICROS"},{"SCOPE_TIMER_DIR",tmpdir}});
        expect(rc == 0, "disabled via env executed in child process");
    }

    static void test_disabled_case_insensitivity_child_process() {
        const char* variants[] = {"off", "Off", "FALSE", "False", "nO"};
        for (const char* variant : variants) {
            std::vector<std::pair<std::string,std::string>> env = {
                {"SCOPE_TIMER", variant},
                {"SCOPE_TIMER_FORMAT", "MICROS"}
            };
            int rc = run_child_with_env(env);
            const std::string msg = std::string("disabled env variant '") + variant + "' handled in child process";
            expect(rc == 0, msg.c_str());
        }
    }

    static void test_bad_env_values_child_process() {
        int rc = run_child_with_env({{"SCOPE_TIMER_DIR","/definitely/does/not/exist"},{"SCOPE_TIMER_FLUSH_N","bogus"},{"SCOPE_TIMER_FORMAT","WONKY"}});
        expect(rc == 0, "bad env values handled in child process");
    }

    static void test_flushN_variants_child_process() {
        const std::pair<const char*, const char*> cases[] = {
            {"SCOPE_TIMER_FLUSH_N",""}, {"SCOPE_TIMER_FLUSH_N","0"}, {"SCOPE_TIMER_FLUSH_N","1"},
            {"SCOPE_TIMER_FLUSH_N","5"}, {"SCOPE_TIMER_FLUSH_N","-1"}, {"SCOPE_TIMER_FLUSH_N","bogus"},
        };
        for (const auto& kv : cases) {
            int rc = run_child_with_env({{kv.first, kv.second},{"SCOPE_TIMER_FORMAT","MICROS"}});
            expect(rc == 0, "flush N variant executed in child process");
        }
    }

    static void test_logdir_edge_cases_child_process() {
        int rc1 = run_child_with_env({{"SCOPE_TIMER_DIR","/definitely-not-a-real-dir-xyz"},{"SCOPE_TIMER_FORMAT","MICROS"}});
        expect(rc1 == 0, "non-existent log dir handled in child process");
        char templ[] = "/tmp/scopetimer_ldirXXXXXX";
        char* tdir = ::mkdtemp(templ);
        std::string tmpdir = tdir ? std::string(tdir) : std::string("/tmp");
        int rc2 = run_child_with_env({{"SCOPE_TIMER_DIR", tmpdir},{"SCOPE_TIMER_FORMAT","MICROS"}});
        expect(rc2 == 0, "valid log dir handled in child process");
    }

    static void test_logfile_null_branch() {
        // Point log dir to a path that (very likely) does not exist so the default sink fails to open.
        std::string bogus = "/tmp/scopetimer_no_such_dir_" + std::to_string(::getpid());
        ::setenv("SCOPE_TIMER_DIR", bogus.c_str(), 1);
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests(bogus);
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        {
            SCOPE_TIMER("tests:logfile_null_branch");
            busyFor(50us);
        }

        struct stat st{};
        int dir_rc = ::stat(bogus.c_str(), &st);
        expect(dir_rc != 0, "default sink: invalid dir not created by library");

        std::string logfile = bogus + "/scopetimer.log";
        int file_rc = ::stat(logfile.c_str(), &st);
        expect(file_rc != 0, "default sink: no logfile created when dir invalid");
        expect(::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests() == -1,
               "default sink keeps fd closed after failure");
    }

    static void test_logfile_failure_cache_branch() {
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();
        std::string bogus = "/tmp/scopetimer_cached_fail_" + std::to_string(::getpid()) + "_dir";
        ::rmdir(bogus.c_str());
        ::setenv("SCOPE_TIMER_DIR", bogus.c_str(), 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests(bogus);

        bool firstAttempt = ::xyzzy::scopetimer::ScopeTimer::ensureLogFdOpen();
        expect(!firstAttempt, "ensureLogFdOpen fails for invalid directory");
        bool secondAttempt = ::xyzzy::scopetimer::ScopeTimer::ensureLogFdOpen();
        expect(!secondAttempt, "ensureLogFdOpen skips repeated attempts for same bad path");

        ::setenv("SCOPE_TIMER_DIR", "/tmp", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp");
    }

    static void test_log_fd_has_cloexec() {
        ::xyzzy::scopetimer::ScopeTimer::setLogSinkForTests(nullptr, nullptr);
        ::setenv("SCOPE_TIMER_DIR", "/tmp", 1);
        ::xyzzy::scopetimer::ScopeTimer::resetLogDirectoryForTests("/tmp");
        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();

        bool opened = ::xyzzy::scopetimer::ScopeTimer::ensureLogFdOpen();
        expect(opened, "ensureLogFdOpen opens fd for cloexec test");

        int fd = ::xyzzy::scopetimer::ScopeTimer::defaultLogFdForTests();
        int flags = ::fcntl(fd, F_GETFD);
        expect(flags != -1 && (flags & FD_CLOEXEC), "log fd has FD_CLOEXEC set");

        ::xyzzy::scopetimer::ScopeTimer::closeLogFdForTests();
    }

    // --------- bootstrapping helpers ---------
    static void init_exe_path(int argc, char** argv) {
        if (argv && argv[0]) {
            char buf[4096];
            if (::realpath(argv[0], buf)) s_exe_path = buf; else s_exe_path = argv[0];
        } else {
            s_exe_path = "./scopetimer_tests";
        }
    }

    static void test_init_exe_path_default_path() {
        std::string original = s_exe_path;
        init_exe_path(0, nullptr);
        expect(s_exe_path == "./scopetimer_tests", "init_exe_path falls back to default binary path");

        if (!original.empty()) {
            std::vector<char> buf(original.begin(), original.end());
            buf.push_back('\0');
            char* fakeArgv[] = { buf.data() };
            init_exe_path(1, fakeArgv);
        }
    }

    // state
    static inline int s_failures;
    static inline std::string s_exe_path;
};

}} // namespace xyzzy::scopetimer

// C-style main that forwards to our friend-test class driver.
int main(int argc, char** argv) {
    return xyzzy::scopetimer::ScopeTimer_TestFriend::run_all(argc, argv) == 0 ? 0 : 1;
}
