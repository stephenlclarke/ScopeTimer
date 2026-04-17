<!-- Generated automatically by scripts/refresh_docs.py -->

# Tests

Formatting and log-processing examples live here so the main README
stays focused on how to use the library.

See also:

- [README.md](README.md) for the library overview and API examples
- [BUILD.md](BUILD.md) for build, coverage, Sonar, and benchmark-target usage

<!-- markdownlint-disable MD013 -->

## Testing with default elapsed time formatting

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:01:37.468 | end=2026-04-17 21:01:37.468 | elapsed=1.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:01:37.469 | end=2026-04-17 21:01:37.472 | elapsed=3.057ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.473 | end=2026-04-17 21:01:37.475 | elapsed=1.896ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.475 | end=2026-04-17 21:01:37.478 | elapsed=3.149ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.472 | end=2026-04-17 21:01:37.479 | elapsed=7.000ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.481 | end=2026-04-17 21:01:37.483 | elapsed=1.146ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.480 | end=2026-04-17 21:01:37.483 | elapsed=2.180ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.480 | end=2026-04-17 21:01:37.483 | elapsed=3.070ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.479 | end=2026-04-17 21:01:37.483 | elapsed=3.848ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:01:37.484 | end=2026-04-17 21:01:37.486 | elapsed=1.511ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.486 | end=2026-04-17 21:01:37.486 | elapsed=387.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.486 | end=2026-04-17 21:01:37.486 | elapsed=394.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.487 | end=2026-04-17 21:01:37.487 | elapsed=386.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.487 | end=2026-04-17 21:01:37.487 | elapsed=393.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.487 | end=2026-04-17 21:01:37.488 | elapsed=394.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.486 | end=2026-04-17 21:01:37.488 | elapsed=2.001ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:01:37.488 | end=2026-04-17 21:01:37.488 | elapsed=644.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:01:37.488 | end=2026-04-17 21:01:37.488 | elapsed=746.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.489 | end=2026-04-17 21:01:37.489 | elapsed=196.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.489 | end=2026-04-17 21:01:37.489 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.489 | end=2026-04-17 21:01:37.489 | elapsed=199.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.488 | end=2026-04-17 21:01:37.489 | elapsed=722.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.489 | end=2026-04-17 21:01:37.489 | elapsed=156.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.489 | end=2026-04-17 21:01:37.490 | elapsed=155.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:01:37.519 | end=2026-04-17 21:01:37.519 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:01:37.519 | end=2026-04-17 21:01:37.522 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.524 | end=2026-04-17 21:01:37.525 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.526 | end=2026-04-17 21:01:37.529 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.522 | end=2026-04-17 21:01:37.529 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.532 | end=2026-04-17 21:01:37.533 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.531 | end=2026-04-17 21:01:37.533 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.530 | end=2026-04-17 21:01:37.533 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.529 | end=2026-04-17 21:01:37.533 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:01:37.535 | end=2026-04-17 21:01:37.536 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.536 | end=2026-04-17 21:01:37.537 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.537 | end=2026-04-17 21:01:37.537 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.537 | end=2026-04-17 21:01:37.537 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.537 | end=2026-04-17 21:01:37.538 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.538 | end=2026-04-17 21:01:37.538 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.536 | end=2026-04-17 21:01:37.538 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:01:37.538 | end=2026-04-17 21:01:37.539 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:01:37.538 | end=2026-04-17 21:01:37.539 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.539 | end=2026-04-17 21:01:37.539 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.539 | end=2026-04-17 21:01:37.539 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.539 | end=2026-04-17 21:01:37.540 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.539 | end=2026-04-17 21:01:37.540 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.540 | end=2026-04-17 21:01:37.540 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.540 | end=2026-04-17 21:01:37.540 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:01:37.569 | end=2026-04-17 21:01:37.569 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:01:37.569 | end=2026-04-17 21:01:37.572 | elapsed=3.131ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.574 | end=2026-04-17 21:01:37.576 | elapsed=1.899ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.576 | end=2026-04-17 21:01:37.579 | elapsed=3.135ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.572 | end=2026-04-17 21:01:37.579 | elapsed=6.985ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.582 | end=2026-04-17 21:01:37.583 | elapsed=1.150ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.581 | end=2026-04-17 21:01:37.583 | elapsed=2.181ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.580 | end=2026-04-17 21:01:37.583 | elapsed=3.078ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.579 | end=2026-04-17 21:01:37.583 | elapsed=3.858ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:01:37.585 | end=2026-04-17 21:01:37.586 | elapsed=1.521ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.586 | end=2026-04-17 21:01:37.587 | elapsed=0.386ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.587 | end=2026-04-17 21:01:37.587 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.587 | end=2026-04-17 21:01:37.588 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.588 | end=2026-04-17 21:01:37.588 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.588 | end=2026-04-17 21:01:37.588 | elapsed=0.383ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.586 | end=2026-04-17 21:01:37.588 | elapsed=1.937ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:01:37.588 | end=2026-04-17 21:01:37.589 | elapsed=0.631ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:01:37.588 | end=2026-04-17 21:01:37.589 | elapsed=0.679ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.589 | end=2026-04-17 21:01:37.589 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.589 | end=2026-04-17 21:01:37.589 | elapsed=0.200ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.589 | end=2026-04-17 21:01:37.590 | elapsed=0.198ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.589 | end=2026-04-17 21:01:37.590 | elapsed=0.729ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.590 | end=2026-04-17 21:01:37.590 | elapsed=0.158ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.590 | end=2026-04-17 21:01:37.590 | elapsed=0.156ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:01:37.620 | end=2026-04-17 21:01:37.620 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:01:37.620 | end=2026-04-17 21:01:37.624 | elapsed=3134.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.625 | end=2026-04-17 21:01:37.627 | elapsed=1894.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.627 | end=2026-04-17 21:01:37.630 | elapsed=3148.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.624 | end=2026-04-17 21:01:37.631 | elapsed=6987.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.633 | end=2026-04-17 21:01:37.634 | elapsed=1151.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.632 | end=2026-04-17 21:01:37.634 | elapsed=2186.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.631 | end=2026-04-17 21:01:37.635 | elapsed=3094.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.631 | end=2026-04-17 21:01:37.635 | elapsed=3870.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:01:37.636 | end=2026-04-17 21:01:37.638 | elapsed=1520.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.638 | end=2026-04-17 21:01:37.638 | elapsed=390.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.638 | end=2026-04-17 21:01:37.638 | elapsed=388.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.638 | end=2026-04-17 21:01:37.639 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.639 | end=2026-04-17 21:01:37.639 | elapsed=393.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.639 | end=2026-04-17 21:01:37.640 | elapsed=386.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.638 | end=2026-04-17 21:01:37.640 | elapsed=1973.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:01:37.640 | end=2026-04-17 21:01:37.640 | elapsed=651.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:01:37.640 | end=2026-04-17 21:01:37.640 | elapsed=730.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.640 | end=2026-04-17 21:01:37.641 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.641 | end=2026-04-17 21:01:37.641 | elapsed=191.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.641 | end=2026-04-17 21:01:37.641 | elapsed=199.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.640 | end=2026-04-17 21:01:37.641 | elapsed=726.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.641 | end=2026-04-17 21:01:37.641 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.641 | end=2026-04-17 21:01:37.641 | elapsed=156.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:01:37.671 | end=2026-04-17 21:01:37.671 | elapsed=1000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:01:37.671 | end=2026-04-17 21:01:37.674 | elapsed=3152000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.676 | end=2026-04-17 21:01:37.678 | elapsed=1886000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.678 | end=2026-04-17 21:01:37.681 | elapsed=3137000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:01:37.674 | end=2026-04-17 21:01:37.681 | elapsed=6940000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.684 | end=2026-04-17 21:01:37.685 | elapsed=1146000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.683 | end=2026-04-17 21:01:37.685 | elapsed=2179000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.682 | end=2026-04-17 21:01:37.685 | elapsed=3087000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:01:37.681 | end=2026-04-17 21:01:37.685 | elapsed=3871000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:01:37.687 | end=2026-04-17 21:01:37.688 | elapsed=1524000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.688 | end=2026-04-17 21:01:37.689 | elapsed=326000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.689 | end=2026-04-17 21:01:37.689 | elapsed=395000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.689 | end=2026-04-17 21:01:37.689 | elapsed=386000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.689 | end=2026-04-17 21:01:37.690 | elapsed=387000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.690 | end=2026-04-17 21:01:37.690 | elapsed=394000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:01:37.688 | end=2026-04-17 21:01:37.690 | elapsed=1938000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:01:37.690 | end=2026-04-17 21:01:37.691 | elapsed=646000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:01:37.690 | end=2026-04-17 21:01:37.691 | elapsed=755000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.691 | end=2026-04-17 21:01:37.691 | elapsed=193000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.691 | end=2026-04-17 21:01:37.692 | elapsed=193000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.692 | end=2026-04-17 21:01:37.692 | elapsed=199000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:01:37.691 | end=2026-04-17 21:01:37.692 | elapsed=727000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.692 | end=2026-04-17 21:01:37.692 | elapsed=161000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:01:37.692 | end=2026-04-17 21:01:37.692 | elapsed=156000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=1us  avg=1us  max=1us  →

[simpleWork]void simpleWork()
  count=1  min=3.148ms  avg=3.148ms  max=3.148ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.881ms  avg=1.881ms  max=1.881ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.135ms  avg=3.135ms  max=3.135ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.930ms  avg=6.930ms  max=6.930ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.155ms  avg=1.155ms  max=1.155ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.198ms  avg=2.198ms  max=2.198ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.108ms  avg=3.108ms  max=3.108ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.885ms  avg=3.885ms  max=3.885ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.525ms  avg=1.525ms  max=1.525ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=381us  avg=387us  max=396us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.977ms  avg=1.977ms  max=1.977ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=636us  avg=636us  max=636us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=686us  avg=686us  max=686us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=192us  avg=196us  max=199us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=724us  avg=724us  max=724us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=154us  avg=158us  max=163us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=610us  avg=610us  max=610us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=85us  avg=85us  max=85us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=361us  avg=370us  max=391us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.863ms  avg=1.863ms  max=1.863ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=643us  avg=643us  max=643us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.274ms  avg=1.274ms  max=1.274ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.264ms  avg=1.270ms  max=1.275ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.826ms  avg=3.826ms  max=3.826ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.185ms  avg=3.185ms  max=3.185ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=36.244ms  avg=36.244ms  max=36.244ms  →

[Demo::main]int main(int, char**)
  count=1  min=36.600ms  avg=36.600ms  max=36.600ms  →
```

<!-- markdownlint-enable MD013 -->
