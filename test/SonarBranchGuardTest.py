#!/usr/bin/env python3
"""Regression tests for the build-time Sonar main-branch guard."""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent
GUARD_SCRIPT = REPO_ROOT / "cmake" / "RequireSonarMainBranch.cmake"


class SonarBranchGuardTests(unittest.TestCase):
    def setUp(self) -> None:
        self.cmake = shutil.which("cmake")
        self.git = shutil.which("git")
        self.assertIsNotNone(self.cmake, "cmake is required for this test")
        self.assertIsNotNone(self.git, "git is required for this test")

    def run_guard(
        self,
        source_dir: Path,
        github_ref_name: str | None,
    ) -> subprocess.CompletedProcess[str]:
        env = os.environ.copy()
        if github_ref_name is None:
            env.pop("GITHUB_REF_NAME", None)
        else:
            env["GITHUB_REF_NAME"] = github_ref_name
        return subprocess.run(
            [
                str(self.cmake),
                f"-DSCOPETIMER_SOURCE_DIR:PATH={source_dir}",
                "-P",
                str(GUARD_SCRIPT),
            ],
            cwd=REPO_ROOT,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )

    def initialize_repo(self, path: Path, branch: str) -> None:
        subprocess.run(
            [str(self.git), "init", "--quiet", str(path)],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        (path / "tracked.txt").write_text("tracked\n", encoding="utf-8")
        subprocess.run(
            [str(self.git), "-C", str(path), "add", "tracked.txt"],
            check=True,
        )
        subprocess.run(
            [
                str(self.git),
                "-C",
                str(path),
                "-c",
                "user.name=ScopeTimer Test",
                "-c",
                "user.email=scopetimer-test@example.invalid",
                "commit",
                "--quiet",
                "-m",
                "initial",
            ],
            check=True,
        )
        subprocess.run(
            [str(self.git), "-C", str(path), "branch", "-M", branch],
            check=True,
        )

    def test_post_config_ref_change_stops_before_sonar_work(self) -> None:
        with tempfile.TemporaryDirectory(prefix="scopetimer-sonar-guard-") as temp:
            build_dir = Path(temp) / "build"
            configure_env = os.environ.copy()
            configure_env["GITHUB_REF_NAME"] = "main"
            configure = subprocess.run(
                [
                    str(self.cmake),
                    "-S",
                    str(REPO_ROOT),
                    "-B",
                    str(build_dir),
                    "-DBUILD_TESTING=OFF",
                    "-DENABLE_COVERAGE=OFF",
                    "-DENABLE_SONAR=ON",
                    f"-DSONAR_BUILD_WRAPPER:FILEPATH={sys.executable}",
                    f"-DSONAR_SCANNER_EXECUTABLE:FILEPATH={sys.executable}",
                ],
                cwd=REPO_ROOT,
                env=configure_env,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )
            self.assertEqual(configure.returncode, 0, configure.stdout)

            build_env = os.environ.copy()
            build_env["GITHUB_REF_NAME"] = "post-config-feature"
            guarded_build = subprocess.run(
                [
                    str(self.cmake),
                    "--build",
                    str(build_dir),
                    "--target",
                    "sonar_scan",
                ],
                cwd=REPO_ROOT,
                env=build_env,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

            self.assertNotEqual(guarded_build.returncode, 0, guarded_build.stdout)
            self.assertIn("post-config-feature", guarded_build.stdout)
            self.assertIn("main-only", guarded_build.stdout)
            self.assertFalse(
                (build_dir / "bw-output").exists(),
                "Sonar build-wrapper work began before the runtime branch guard",
            )

    def test_env_main_cannot_mask_an_attached_feature_branch(self) -> None:
        with tempfile.TemporaryDirectory(prefix="scopetimer-sonar-mismatch-") as temp:
            repo = Path(temp) / "repo"
            self.initialize_repo(repo, "feature")

            guarded = self.run_guard(repo, "main")

            self.assertNotEqual(guarded.returncode, 0, guarded.stdout)
            self.assertIn("attached git branch", guarded.stdout)
            self.assertIn("is 'feature'", guarded.stdout)
            self.assertIn("mismatched checkout", guarded.stdout)

    def test_env_main_allows_detached_ci_checkout(self) -> None:
        with tempfile.TemporaryDirectory(prefix="scopetimer-sonar-detached-") as temp:
            repo = Path(temp) / "repo"
            self.initialize_repo(repo, "main")
            subprocess.run(
                [str(self.git), "-C", str(repo), "checkout", "--quiet", "--detach"],
                check=True,
            )

            guarded = self.run_guard(repo, "main")

            self.assertEqual(guarded.returncode, 0, guarded.stdout)
            self.assertIn("detached HEAD", guarded.stdout)

    def test_git_branch_is_used_when_environment_ref_is_absent(self) -> None:
        with tempfile.TemporaryDirectory(prefix="scopetimer-sonar-git-") as temp:
            repo = Path(temp) / "repo"
            self.initialize_repo(repo, "main")

            guarded = self.run_guard(repo, None)

            self.assertEqual(guarded.returncode, 0, guarded.stdout)
            self.assertIn("guard passed (git)", guarded.stdout)


if __name__ == "__main__":
    unittest.main()
