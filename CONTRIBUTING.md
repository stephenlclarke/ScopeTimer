<!-- markdownlint-disable MD013 -->

# Contributing to ScopeTimer

Develop changes on `develop` and open pull requests against `main`. Keep each change focused, include regression coverage for bug fixes, and update user-facing documentation when behavior or configuration changes.

## Commit messages

Use [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/):

```text
<type>[(optional-scope)]: <short summary>
```

Common types are `feat`, `fix`, `perf`, `refactor`, `test`, `docs`, `build`, `ci`, and `chore`.

Examples:

```text
fix(scopetimer): preserve async records at process exit
perf(scopetimer): reduce buffered sink handoff overhead
docs(readme): clarify custom sink lifetime
```

For a breaking change, add a `BREAKING CHANGE:` footer explaining the compatibility impact.

## Local validation

Run the normal build and test suite:

```bash
cmake -S . -B build-review -DAUTO_REFRESH_DOCS=OFF
cmake --build build-review --parallel
ctest --test-dir build-review --output-on-failure
```

Run the header coverage and platform leak-check targets when their prerequisites are available:

```bash
cmake --build build-review --target scopetimer_header_coverage
cmake --build build-review --target leak_check
```

Changes that can affect timer overhead, sink behavior, formatting, buffering, threading, or benchmark configuration also require the local MacBook benchmark matrix:

```bash
cmake --build build-review --target demo_benchmark_matrix
```

Keep `BENCHMARK.md` and `benchmarks/demo_benchmark_history.json` together when a benchmark snapshot changes. Benchmarks are intentionally local-only and must not be added to GitHub Actions.
