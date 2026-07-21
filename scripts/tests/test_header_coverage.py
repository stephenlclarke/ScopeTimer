from __future__ import annotations

import argparse
import sys
import unittest
from pathlib import Path


SCRIPTS_DIR = Path(__file__).resolve().parents[1]
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

import check_scope_timer_header_coverage as coverage


class HeaderCoverageArgumentTests(unittest.TestCase):
    def test_threshold_accepts_finite_percentage_range(self) -> None:
        self.assertEqual(coverage.coverage_threshold("0"), 0.0)
        self.assertEqual(coverage.coverage_threshold("100"), 100.0)
        self.assertEqual(coverage.coverage_threshold("80.5"), 80.5)

    def test_threshold_rejects_nan_infinity_and_out_of_range_values(self) -> None:
        for value in ("nan", "inf", "-inf", "-0.1", "100.1", "not-a-number"):
            with self.subTest(value=value):
                with self.assertRaises(argparse.ArgumentTypeError):
                    coverage.coverage_threshold(value)


if __name__ == "__main__":
    unittest.main()
