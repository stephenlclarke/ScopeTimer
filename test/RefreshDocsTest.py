#!/usr/bin/env python3
"""Focused safety tests for the managed documentation build directory."""

from __future__ import annotations

import importlib.util
import tempfile
import unittest
from pathlib import Path
from unittest import mock


SCRIPT_PATH = Path(__file__).resolve().parent.parent / "scripts" / "refresh_docs.py"
SPEC = importlib.util.spec_from_file_location("refresh_docs", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
refresh_docs = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(refresh_docs)


class ManagedBuildDirectoryTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = tempfile.TemporaryDirectory(prefix="scopetimer-docs-test-")
        self.addCleanup(self.temp_dir.cleanup)
        self.base = Path(self.temp_dir.name)
        self.repo_root = self.base / "repo"
        self.managed_root = self.base / "outer-build"
        self.repo_root.mkdir()
        self.managed_root.mkdir()
        self.docs_build_dir = self.managed_root / refresh_docs.MANAGED_BUILD_DIR_NAME

    def prepare(self) -> None:
        refresh_docs.prepare_managed_build_dir(
            self.docs_build_dir,
            self.managed_root,
            self.repo_root,
        )

    def test_creates_and_reuses_only_marker_owned_directory(self) -> None:
        self.prepare()
        marker = self.docs_build_dir / refresh_docs.MANAGED_BUILD_MARKER
        self.assertEqual(
            marker.read_text(encoding="utf-8"),
            refresh_docs.MANAGED_BUILD_MARKER_CONTENT,
        )

        stale_file = self.docs_build_dir / "stale.txt"
        stale_file.write_text("stale", encoding="utf-8")
        self.prepare()

        self.assertFalse(stale_file.exists())
        self.assertTrue(marker.is_file())

    def test_refuses_unowned_existing_directory_without_deleting_it(self) -> None:
        self.docs_build_dir.mkdir()
        sentinel = self.docs_build_dir / "keep.txt"
        sentinel.write_text("keep", encoding="utf-8")

        with self.assertRaisesRegex(RuntimeError, "unowned"):
            self.prepare()

        self.assertEqual(sentinel.read_text(encoding="utf-8"), "keep")

    def test_refuses_paths_other_than_the_managed_child(self) -> None:
        wrong_path = self.managed_root / "other"

        with self.assertRaisesRegex(RuntimeError, "managed child"):
            refresh_docs.prepare_managed_build_dir(
                wrong_path,
                self.managed_root,
                self.repo_root,
            )

        self.assertFalse(wrong_path.exists())

    def test_refuses_repo_root_as_managed_root(self) -> None:
        unsafe_path = self.repo_root / refresh_docs.MANAGED_BUILD_DIR_NAME

        with self.assertRaisesRegex(RuntimeError, "unsafe managed build root"):
            refresh_docs.prepare_managed_build_dir(
                unsafe_path,
                self.repo_root,
                self.repo_root,
            )

        self.assertFalse(unsafe_path.exists())

    def test_generated_transcript_uses_managed_refresh_target(self) -> None:
        build_doc = self.base / "BUILD.md"
        completed = mock.Mock(stdout=f"nested build output in {self.repo_root}\n")

        with mock.patch.object(refresh_docs, "run_command", return_value=completed):
            refresh_docs.refresh_build_doc(
                self.repo_root,
                self.docs_build_dir,
                build_doc,
            )

        generated = build_doc.read_text(encoding="utf-8")
        transcript = generated.split("<!-- markdownlint-disable MD013 -->", 1)[1]
        self.assertIn("--target docs_refresh", generated)
        self.assertNotIn("cmake -S . -B", transcript)
        self.assertNotIn("build-docs.log", transcript)
        self.assertNotIn(str(self.repo_root), transcript)

    def test_generated_transcript_does_not_overlap_head_and_tail(self) -> None:
        outputs = [
            mock.Mock(stdout="\n".join(f"configure-{index}" for index in range(50)) + "\n"),
            mock.Mock(stdout="\n".join(f"build-{index}" for index in range(50)) + "\n"),
            mock.Mock(stdout="\n".join(f"test-{index}" for index in range(50)) + "\n"),
        ]
        build_doc = self.base / "BUILD.md"

        with mock.patch.object(refresh_docs, "run_command", side_effect=outputs):
            refresh_docs.refresh_build_doc(
                self.repo_root,
                self.docs_build_dir,
                build_doc,
            )

        transcript = build_doc.read_text(encoding="utf-8")
        self.assertEqual(transcript.count("configure-49"), 1)
        self.assertEqual(transcript.count("build-49"), 1)
        self.assertEqual(transcript.count("test-49"), 1)


if __name__ == "__main__":
    unittest.main()
