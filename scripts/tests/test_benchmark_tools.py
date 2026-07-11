from __future__ import annotations

import argparse
import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

SCRIPTS_DIR = Path(__file__).resolve().parents[1]
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

import benchmark_demo
import record_demo_benchmarks


class BenchmarkEnvironmentTests(unittest.TestCase):
    def test_parse_extra_env_normalizes_known_benchmark_values(self) -> None:
        parsed = benchmark_demo.parse_extra_env(
            [
                "SCOPE_TIMER_BENCH_SINK=buffered",
                "SCOPE_TIMER_BENCH_TIMER=fast",
                "SCOPE_TIMER_BENCH_THREADS=04",
                "scope_timer_bench_sink_bytes=4096",
            ]
        )

        self.assertEqual(parsed["SCOPE_TIMER_BENCH_SINK"], "BUFFERED")
        self.assertEqual(parsed["SCOPE_TIMER_BENCH_TIMER"], "FAST")
        self.assertEqual(parsed["SCOPE_TIMER_BENCH_THREADS"], "4")
        self.assertEqual(parsed["SCOPE_TIMER_BENCH_SINK_BYTES"], "4096")

    def test_run_once_removes_ambient_scope_timer_configuration(self) -> None:
        completed = mock.Mock(returncode=0)
        with tempfile.TemporaryDirectory() as tmp_dir:
            with (
                mock.patch.dict(
                    os.environ,
                    {
                        "SCOPE_TIMER": "ambient",
                        "SCOPE_TIMER_DIR": "/ambient",
                        "SCOPE_TIMER_FORMAT": "SECONDS",
                        "SCOPE_TIMER_BENCH_THREADS": "99",
                        "SCOPETIMER_TEST_PRESERVED": "yes",
                    },
                    clear=False,
                ),
                mock.patch.object(
                    benchmark_demo.subprocess,
                    "run",
                    return_value=completed,
                ) as run,
            ):
                benchmark_demo.run_once(
                    Path("Benchmark"),
                    iterations=1,
                    scenario="hotpath-bench",
                    enabled=False,
                    log_dir=Path(tmp_dir),
                    extra_env={},
                )

        child_env = run.call_args.kwargs["env"]
        self.assertEqual(child_env["SCOPE_TIMER"], "0")
        self.assertNotEqual(child_env["SCOPE_TIMER_DIR"], "/ambient")
        self.assertNotIn("SCOPE_TIMER_FORMAT", child_env)
        self.assertNotIn("SCOPE_TIMER_BENCH_THREADS", child_env)
        self.assertEqual(child_env["SCOPETIMER_TEST_PRESERVED"], "yes")

    def test_parse_extra_env_rejects_harness_owned_keys(self) -> None:
        for item in ("SCOPE_TIMER=0", "scope_timer_dir=/tmp/elsewhere"):
            with self.subTest(item=item):
                with self.assertRaises(benchmark_demo.BenchmarkConfigurationError):
                    benchmark_demo.parse_extra_env([item])

    def test_parse_extra_env_rejects_duplicate_known_keys_case_insensitively(self) -> None:
        with self.assertRaisesRegex(
            benchmark_demo.BenchmarkConfigurationError,
            "may only be specified once",
        ):
            benchmark_demo.parse_extra_env(
                [
                    "SCOPE_TIMER_BENCH_SINK=ASYNC",
                    "scope_timer_bench_sink=NULL",
                ]
            )

    def test_parse_extra_env_rejects_malformed_entries(self) -> None:
        for item in (
            "NO_EQUALS",
            "=value",
            "BAD-NAME=value",
            "NÁME=value",
            "SCOPE_TIMER_BENCH_THREADS=-1",
            "SCOPE_TIMER_BENCH_THREADS=1junk",
            "SCOPE_TIMER_BENCH_THREADS=0",
            "SCOPE_TIMER_BENCH_SINK_BYTES=999999999999999999999999",
            "SCOPE_TIMER_BENCH_SINK=other",
            "SCOPE_TIMER_BENCH_TIMER=other",
        ):
            with self.subTest(item=item):
                with self.assertRaises(benchmark_demo.BenchmarkConfigurationError):
                    benchmark_demo.parse_extra_env([item])

    def test_expected_record_count_is_independent_of_sink_output(self) -> None:
        self.assertEqual(benchmark_demo.expected_record_count(5, {}), 76_807)
        self.assertEqual(
            benchmark_demo.expected_record_count(
                5,
                {
                    "SCOPE_TIMER_BENCH_SINK": "NULL",
                    "SCOPE_TIMER_BENCH_THREADS": "4",
                },
            ),
            307_207,
        )

    def test_profile_output_detection_handles_null_aliases(self) -> None:
        self.assertTrue(benchmark_demo.profile_emits_output({}))
        self.assertTrue(
            benchmark_demo.profile_emits_output({"SCOPE_TIMER_BENCH_SINK": "ASYNC"})
        )
        self.assertFalse(
            benchmark_demo.profile_emits_output({"SCOPE_TIMER_BENCH_SINK": "NULL"})
        )
        self.assertFalse(
            benchmark_demo.profile_emits_output({"SCOPE_TIMER_BENCH_SINK": "NOOP"})
        )


class BenchmarkProbeTests(unittest.TestCase):
    @staticmethod
    def completed(stdout: str, *, returncode: int = 0, stderr: str = ""):
        return mock.Mock(returncode=returncode, stdout=stdout, stderr=stderr)

    def test_enabled_benchmark_probe_is_accepted(self) -> None:
        with mock.patch.object(
            benchmark_demo.subprocess,
            "run",
            return_value=self.completed(benchmark_demo.INSTRUMENTATION_PROBE_ENABLED + "\n"),
        ) as run:
            result = benchmark_demo.probe_benchmark_binary(Path("Benchmark"))

        self.assertEqual(result["identity"], "ScopeTimerBenchmark")
        self.assertEqual(result["instrumentation"], "enabled")
        self.assertEqual(
            run.call_args.args[0],
            ["Benchmark", "--instrumentation-status"],
        )

    def test_release_benchmark_probe_is_rejected(self) -> None:
        binary = Path("Benchmark")
        with mock.patch.object(
            benchmark_demo.subprocess,
            "run",
            return_value=self.completed(benchmark_demo.INSTRUMENTATION_PROBE_DISABLED + "\n"),
        ):
            with self.assertRaisesRegex(
                benchmark_demo.BenchmarkInvariantError,
                "disabled by NDEBUG",
            ):
                benchmark_demo.probe_benchmark_binary(binary)

    def test_empty_true_like_probe_is_rejected(self) -> None:
        binary = Path("true")
        with mock.patch.object(
            benchmark_demo.subprocess,
            "run",
            return_value=self.completed(""),
        ):
            with self.assertRaisesRegex(
                benchmark_demo.BenchmarkInvariantError,
                "did not return the ScopeTimer Benchmark",
            ):
                benchmark_demo.probe_benchmark_binary(binary)

    @unittest.skipUnless(shutil.which("true"), "requires the platform true executable")
    def test_real_true_executable_is_rejected(self) -> None:
        true_binary = Path(shutil.which("true") or "true")
        with self.assertRaisesRegex(
            benchmark_demo.BenchmarkInvariantError,
            "did not return the ScopeTimer Benchmark",
        ):
            benchmark_demo.probe_benchmark_binary(true_binary)

    def test_probe_rejects_nonzero_or_noisy_response(self) -> None:
        binary = Path("Benchmark")
        cases = (
            (self.completed("", returncode=2), "exited with 2"),
            (
                self.completed(
                    benchmark_demo.INSTRUMENTATION_PROBE_ENABLED,
                    stderr="unexpected",
                ),
                "unexpected stderr",
            ),
            (self.completed("different protocol"), "did not return"),
        )
        for completed, message in cases:
            with self.subTest(message=message):
                with mock.patch.object(
                    benchmark_demo.subprocess,
                    "run",
                    return_value=completed,
                ):
                    with self.assertRaisesRegex(
                        benchmark_demo.BenchmarkInvariantError,
                        message,
                    ):
                        benchmark_demo.probe_benchmark_binary(binary)

    def test_probe_timeout_is_rejected(self) -> None:
        binary = Path("Benchmark")
        with mock.patch.object(
            benchmark_demo.subprocess,
            "run",
            side_effect=benchmark_demo.subprocess.TimeoutExpired("Benchmark", 5),
        ):
            with self.assertRaisesRegex(
                benchmark_demo.BenchmarkInvariantError,
                "identity probe timed out",
            ):
                benchmark_demo.probe_benchmark_binary(binary)


class BenchmarkReportTests(unittest.TestCase):
    ENABLED_PROBE = {
        "protocol": 1,
        "identity": "ScopeTimerBenchmark",
        "instrumentation": "enabled",
    }

    @staticmethod
    def fake_runner(calls: list[bool]):
        def run_once(
            binary: Path,
            iterations: int,
            scenario: str,
            enabled: bool,
            log_dir: Path,
            extra_env: dict[str, str],
        ) -> dict[str, float | int | bool]:
            del binary, scenario, log_dir
            calls.append(enabled)
            expected = benchmark_demo.expected_record_count(iterations, extra_env)
            emits = benchmark_demo.profile_emits_output(extra_env)
            return {
                "seconds": 1.0 + (expected / 1_000_000.0 if enabled else 0.0),
                "log_exists": enabled and emits,
                "log_lines": expected if enabled and emits else 0,
                "log_bytes": expected * 80 if enabled and emits else 0,
            }

        return run_once

    def test_build_report_counterbalances_pairs_and_records_order(self) -> None:
        calls: list[bool] = []
        with (
            mock.patch.object(
                benchmark_demo,
                "probe_benchmark_binary",
                return_value=self.ENABLED_PROBE,
            ) as probe,
            mock.patch.object(
                benchmark_demo,
                "run_once",
                side_effect=self.fake_runner(calls),
            ),
        ):
            report = benchmark_demo.build_report(
                Path("Benchmark"),
                iterations=1,
                runs=4,
                scenario="hotpath-bench",
                extra_env={},
            )

        self.assertEqual(calls[:2], [False, True])
        self.assertEqual(
            calls[2:],
            [False, True, True, False, False, True, True, False],
        )
        self.assertEqual(
            report["pair_orders"],
            [
                ["disabled", "enabled"],
                ["enabled", "disabled"],
                ["disabled", "enabled"],
                ["enabled", "disabled"],
            ],
        )
        self.assertEqual(report["enabled_log_lines"], 3_075)
        self.assertEqual(report["expected_enabled_records"], 3_075)
        self.assertEqual(report["instrumentation_probe"], self.ENABLED_PROBE)
        probe.assert_called_once_with(Path("Benchmark"))
        self.assertAlmostEqual(float(report["approx_per_record_us"]), 1.0)

    def test_null_sink_uses_modeled_record_denominator(self) -> None:
        calls: list[bool] = []
        with (
            mock.patch.object(
                benchmark_demo,
                "probe_benchmark_binary",
                return_value=self.ENABLED_PROBE,
            ) as probe,
            mock.patch.object(
                benchmark_demo,
                "run_once",
                side_effect=self.fake_runner(calls),
            ),
        ):
            report = benchmark_demo.build_report(
                Path("Benchmark"),
                iterations=1,
                runs=2,
                scenario="hotpath-bench",
                extra_env={"SCOPE_TIMER_BENCH_SINK": "NULL"},
            )

        self.assertEqual(report["enabled_log_lines"], 0)
        self.assertEqual(report["per_record_denominator"], 3_075)
        self.assertFalse(report["profile_emits_output"])
        probe.assert_called_once_with(Path("Benchmark"))
        self.assertAlmostEqual(float(report["approx_per_record_us"]), 1.0)

    def test_build_report_rejects_missing_enabled_output(self) -> None:
        binary = Path("Benchmark")

        def missing_output(*args, **kwargs):
            del args, kwargs
            return {
                "seconds": 1.0,
                "log_exists": False,
                "log_lines": 0,
                "log_bytes": 0,
            }

        with (
            mock.patch.object(
                benchmark_demo,
                "probe_benchmark_binary",
                return_value=self.ENABLED_PROBE,
            ),
            mock.patch.object(benchmark_demo, "run_once", side_effect=missing_output),
        ):
            with self.assertRaisesRegex(
                benchmark_demo.BenchmarkInvariantError,
                "compiled with NDEBUG",
            ):
                benchmark_demo.build_report(
                    binary,
                    iterations=1,
                    runs=1,
                    scenario="hotpath-bench",
                    extra_env={},
                )

    def test_validate_run_result_rejects_wrong_record_count(self) -> None:
        with self.assertRaisesRegex(benchmark_demo.BenchmarkInvariantError, "expected 10"):
            benchmark_demo.validate_run_result(
                {
                    "seconds": 1.0,
                    "log_exists": True,
                    "log_lines": 9,
                    "log_bytes": 100,
                },
                enabled=True,
                emits_output=True,
                expected_records=10,
                description="test run",
            )

    def test_validate_run_result_rejects_disabled_output(self) -> None:
        with self.assertRaisesRegex(benchmark_demo.BenchmarkInvariantError, "disabled run"):
            benchmark_demo.validate_run_result(
                {
                    "seconds": 1.0,
                    "log_exists": True,
                    "log_lines": 1,
                    "log_bytes": 10,
                },
                enabled=False,
                emits_output=True,
                expected_records=10,
                description="test run",
            )


class MetadataAndFingerprintTests(unittest.TestCase):
    def test_optional_command_helpers_tolerate_missing_executables(self) -> None:
        with mock.patch.object(
            record_demo_benchmarks.subprocess,
            "run",
            side_effect=FileNotFoundError,
        ):
            self.assertEqual(record_demo_benchmarks.run_command(["missing-tool"]), "")
            self.assertEqual(
                record_demo_benchmarks.run_git(["status"], Path(".")),
                "",
            )

    def test_diskutil_metadata_tolerates_missing_tool(self) -> None:
        with (
            mock.patch.object(record_demo_benchmarks.platform, "system", return_value="Darwin"),
            mock.patch.object(
                record_demo_benchmarks.subprocess,
                "run",
                side_effect=FileNotFoundError,
            ),
        ):
            self.assertEqual(record_demo_benchmarks.diskutil_metadata("/"), {})

    def test_fingerprint_is_canonical_and_changes_with_context(self) -> None:
        context_a = {
            "benchmark": {"iterations": 5, "threads": 4},
            "machine": {"cpu": "example"},
            "toolchain": {"compiler": "clang"},
        }
        context_reordered = {
            "toolchain": {"compiler": "clang"},
            "machine": {"cpu": "example"},
            "benchmark": {"threads": 4, "iterations": 5},
        }
        context_b = {
            **context_a,
            "benchmark": {"iterations": 5, "threads": 8},
        }

        fingerprint_a = record_demo_benchmarks.comparison_fingerprint(context_a)
        fingerprint_reordered = record_demo_benchmarks.comparison_fingerprint(context_reordered)
        fingerprint_b = record_demo_benchmarks.comparison_fingerprint(context_b)

        self.assertEqual(fingerprint_a["sha256"], fingerprint_reordered["sha256"])
        self.assertNotEqual(fingerprint_a["sha256"], fingerprint_b["sha256"])

    def test_toolchain_metadata_records_cache_compiler_and_binary_hash(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            root = Path(tmp_dir)
            binary = root / "Benchmark"
            binary.write_bytes(b"benchmark-binary")
            (root / "CMakeCache.txt").write_text(
                "CMAKE_CXX_COMPILER:FILEPATH=/usr/bin/example-c++\n"
                "CMAKE_BUILD_TYPE:STRING=Debug\n"
                "CMAKE_GENERATOR:INTERNAL=Ninja\n",
                encoding="utf-8",
            )
            with mock.patch.object(
                record_demo_benchmarks,
                "run_command",
                return_value="Example C++ 1.0\nmore detail",
            ):
                metadata = record_demo_benchmarks.toolchain_metadata(root, binary, "-O3")

        self.assertEqual(metadata["compiler_name"], "example-c++")
        self.assertEqual(metadata["compiler_version_output"], "Example C++ 1.0")
        self.assertEqual(metadata["cmake"]["CMAKE_BUILD_TYPE"], "Debug")
        self.assertEqual(
            metadata["binary_sha256"],
            "feb5b9b30949349d3a59c9489c8483aa3e1c210789c3d169e9ab8540f2b4b4aa",
        )


class ComparisonTests(unittest.TestCase):
    @staticmethod
    def fingerprint(marker: str) -> dict[str, object]:
        return record_demo_benchmarks.comparison_fingerprint(
            {
                "benchmark": {"scenario": "hotpath-bench", "iterations": 5, "marker": marker},
                "machine": {"system": "TestOS", "machine": "arm64", "cpu_brand": "Test CPU"},
                "toolchain": {
                    "compiler_name": "test-c++",
                    "compiler_version_output": "test-c++ 1.0",
                    "cmake_generator": "Ninja",
                },
            }
        )

    @staticmethod
    def baseline(fingerprint: dict[str, object] | None) -> dict[str, object]:
        baseline: dict[str, object] = {
            "git": {"short_commit": "abc1234", "commit": "abc1234-full"},
            "recorded_at_utc": "2026-01-01T00:00:00+00:00",
            "results": [{"name": "profile", "approx_per_record_us": 1.0}],
        }
        if fingerprint is not None:
            baseline["comparison_fingerprint"] = fingerprint
        return baseline

    def test_missing_baseline_fingerprint_is_incomparable(self) -> None:
        current = self.fingerprint("current")
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(None),
            "profile",
            current,
        )

        self.assertEqual(comparison["status"], "incomparable")
        self.assertIn("no configuration fingerprint", comparison["summary"])

    def test_incomplete_current_fingerprint_is_incomparable(self) -> None:
        current = record_demo_benchmarks.comparison_fingerprint(
            {"benchmark": {"scenario": "hotpath-bench", "iterations": 5}}
        )
        baseline = self.fingerprint("baseline")
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(baseline),
            "profile",
            current,
        )

        self.assertFalse(current["complete"])
        self.assertEqual(comparison["status"], "incomparable")
        self.assertIn("lacks required metadata", comparison["summary"])

    def test_incomplete_baseline_fingerprint_is_incomparable(self) -> None:
        current = self.fingerprint("current")
        baseline = record_demo_benchmarks.comparison_fingerprint(
            {"benchmark": {"scenario": "hotpath-bench", "iterations": 5}}
        )
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(baseline),
            "profile",
            current,
        )

        self.assertFalse(baseline["complete"])
        self.assertEqual(comparison["status"], "incomparable")
        self.assertIn("baseline fingerprint lacks", comparison["summary"])

    def test_missing_current_fingerprint_preserves_legacy_comparison(self) -> None:
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(None),
            "profile",
        )

        self.assertEqual(comparison["status"], "faster")
        self.assertAlmostEqual(comparison["delta_pct"], -10.0)

    def test_mismatched_fingerprint_is_incomparable(self) -> None:
        current = self.fingerprint("current")
        baseline = self.fingerprint("baseline")
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(baseline),
            "profile",
            current,
        )

        self.assertEqual(comparison["status"], "incomparable")
        self.assertIn("host, or toolchain differs", comparison["summary"])

    def test_matching_fingerprint_allows_metric_comparison(self) -> None:
        fingerprint = self.fingerprint("same")
        comparison = record_demo_benchmarks.comparison_for_profile(
            {"approx_per_record_us": 0.9},
            self.baseline(fingerprint),
            "profile",
            fingerprint,
        )

        self.assertEqual(comparison["status"], "faster")
        self.assertAlmostEqual(comparison["delta_pct"], -10.0)


class MatrixArgumentTests(unittest.TestCase):
    @staticmethod
    def namespace(**overrides):
        values = {
            "binary": "build-bench/Benchmark",
            "scenario": "hotpath-bench",
            "iterations": 5,
            "runs": 8,
            "threads": 4,
            "sink_bytes": 4096,
            "history_file": "history.json",
            "report_file": "report.md",
            "build_dir": "build-bench",
            "cxx_flags": "-O3",
            "refresh_report_only": False,
        }
        values.update(overrides)
        return argparse.Namespace(**values)

    def test_matrix_arguments_are_normalized_once(self) -> None:
        root = Path("/repo")
        config = record_demo_benchmarks.normalize_args(self.namespace(), root)

        self.assertEqual(config.iterations, 5)
        self.assertEqual(config.threads, 4)
        self.assertEqual(config.build_dir, root / "build-bench")

    def test_matrix_arguments_reject_zero_instead_of_clamping(self) -> None:
        args = self.namespace(threads=0)
        root = Path("/repo")
        with self.assertRaises(benchmark_demo.BenchmarkConfigurationError):
            record_demo_benchmarks.normalize_args(args, root)

    def test_build_directory_defaults_to_binary_parent(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            binary = Path(temp) / "custom-build" / "Benchmark"
            config = record_demo_benchmarks.normalize_args(
                self.namespace(binary=str(binary), build_dir=None),
                Path(temp),
            )

        self.assertEqual(config.binary, binary.resolve())
        self.assertEqual(config.build_dir, binary.parent.resolve())

    def test_repo_paths_are_made_portable_for_history(self) -> None:
        binary = record_demo_benchmarks.REPO_ROOT / "build-review" / "benchmark-build" / "Benchmark"

        self.assertEqual(
            record_demo_benchmarks.display_path(str(binary)),
            "./build-review/benchmark-build/Benchmark",
        )


if __name__ == "__main__":
    unittest.main()
