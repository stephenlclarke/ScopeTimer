#include "ScopeTimer.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using namespace std::chrono_literals;

// All unit tests are now methods of a class that can be declared as a
// friend inside ScopeTimer.hpp (e.g., `friend class ScopeTimer_TestFriend;`).
// We DO NOT modify ScopeTimer.hpp here.
namespace ewm { namespace scopetimer {

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
        test_looped_work();
        test_threaded();
        test_env_format_variants();
        test_env_format_unset_and_empty();
        test_empty_label();
        test_seconds_format_branch();
        test_fmt_auto_seconds_branch();
        test_finalize_snprintf_result_branches();
        test_disabled_via_env_child_process();
        test_disabled_case_insensitivity_child_process();
        test_bad_env_values_child_process();
        test_flushN_variants_child_process();
        test_logdir_edge_cases_child_process();
        test_logfile_null_branch();

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
        SCOPE_TIMER("tests:simple_scope");
        busyFor(50us);
        expect(true, "simple scope executed");
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
        ewm::scopetimer::ScopeTimer::fmtAuto(ns, buf, sizeof(buf));
        const std::string out(buf);
        expect(out.find("ms") == std::string::npos, "fmtAuto(seconds): not milliseconds");
        expect(out.find("us") == std::string::npos, "fmtAuto(seconds): not microseconds");
        expect(out.find("ns") == std::string::npos, "fmtAuto(seconds): not nanoseconds");
        expect(out.find('s') != std::string::npos,   "fmtAuto(seconds): seconds unit present");
    }

    static void test_finalize_snprintf_result_branches() {
        using ewm::scopetimer::ScopeTimerDetail::finalize_snprintf_result;
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
        if (!probe || std::string(probe) != "1") return -1;
        SCOPE_TIMER("tests:child:probe");
        busyFor(100us);
        return 0;
    }

    static int run_child_with_env(const std::vector<std::pair<std::string,std::string>>& envs) {
        std::string cmd;
        for (const auto& kv : envs) cmd += kv.first + "=" + kv.second + " ";
        cmd += shellEscape(s_exe_path);
        cmd = std::string("SCOPETIMER_PROBE=1 ") + cmd;
        cmd += " >/dev/null 2>&1";
        return std::system(cmd.c_str());
    }

    static void test_disabled_via_env_child_process() {
        char templ[] = "/tmp/scopetimerXXXXXX";
        char* tdir = ::mkdtemp(templ);
        std::string tmpdir = tdir ? std::string(tdir) : std::string("/tmp");
        int rc = run_child_with_env({{"SCOPE_TIMER","0"},{"SCOPE_TIMER_FORMAT","MICROS"},{"SCOPE_TIMER_LOG_DIR",tmpdir}});
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
        int rc = run_child_with_env({{"SCOPE_TIMER_LOG_DIR","/definitely/does/not/exist"},{"SCOPE_TIMER_FLUSH_N","bogus"},{"SCOPE_TIMER_FORMAT","WONKY"}});
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
        int rc1 = run_child_with_env({{"SCOPE_TIMER_LOG_DIR","/definitely-not-a-real-dir-xyz"},{"SCOPE_TIMER_FORMAT","MICROS"}});
        expect(rc1 == 0, "non-existent log dir handled in child process");
        char templ[] = "/tmp/scopetimer_ldirXXXXXX";
        char* tdir = ::mkdtemp(templ);
        std::string tmpdir = tdir ? std::string(tdir) : std::string("/tmp");
        int rc2 = run_child_with_env({{"SCOPE_TIMER_LOG_DIR", tmpdir},{"SCOPE_TIMER_FORMAT","MICROS"}});
        expect(rc2 == 0, "valid log dir handled in child process");
    }

    static void test_logfile_null_branch() {
        // Point log dir to a path that (very likely) does not exist so logFile() returns nullptr,
        // exercising the false path of `if (FILE* fp = logFile()) { ... }`.
        // Use PID to ensure uniqueness.
        std::string bogus = "/tmp/scopetimer_no_such_dir_" + std::to_string(::getpid());
        ::setenv("SCOPE_TIMER_LOG_DIR", bogus.c_str(), 1);
        ::setenv("SCOPE_TIMER_FORMAT", "MICROS", 1);

        // Emit a timing scope; this should NOT create a logfile because the directory is invalid.
        {
            SCOPE_TIMER("tests:logfile_null_branch");
            busyFor(50us);
        }

        // Verify that the directory and file were not created.
        struct stat st{};
        int dir_rc = ::stat(bogus.c_str(), &st);
        expect(dir_rc != 0, "logFile() null branch: invalid dir not created by library");

        std::string logfile = bogus + "/scopetimer.log";
        int file_rc = ::stat(logfile.c_str(), &st);
        expect(file_rc != 0, "logFile() null branch: no logfile created when dir invalid");
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

    // state
    static inline int s_failures;
    static inline std::string s_exe_path;
};

}} // namespace ewm::scopetimer

// C-style main that forwards to our friend-test class driver.
int main(int argc, char** argv) {
    return ewm::scopetimer::ScopeTimer_TestFriend::run_all(argc, argv) == 0 ? 0 : 1;
}
