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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:45:44.337 | end=2026-04-17 21:45:44.337 | elapsed=1.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:45:44.338 | end=2026-04-17 21:45:44.342 | elapsed=3.756ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.343 | end=2026-04-17 21:45:44.346 | elapsed=2.260ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.346 | end=2026-04-17 21:45:44.349 | elapsed=3.773ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.342 | end=2026-04-17 21:45:44.350 | elapsed=8.348ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.353 | end=2026-04-17 21:45:44.355 | elapsed=1.371ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.352 | end=2026-04-17 21:45:44.355 | elapsed=2.611ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.351 | end=2026-04-17 21:45:44.355 | elapsed=3.693ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.350 | end=2026-04-17 21:45:44.355 | elapsed=4.620ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:45:44.357 | end=2026-04-17 21:45:44.358 | elapsed=1.809ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.358 | end=2026-04-17 21:45:44.359 | elapsed=455.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.359 | end=2026-04-17 21:45:44.359 | elapsed=456.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.359 | end=2026-04-17 21:45:44.360 | elapsed=467.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.360 | end=2026-04-17 21:45:44.360 | elapsed=457.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.360 | end=2026-04-17 21:45:44.361 | elapsed=460.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.358 | end=2026-04-17 21:45:44.361 | elapsed=2.332ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:45:44.361 | end=2026-04-17 21:45:44.362 | elapsed=770.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:45:44.361 | end=2026-04-17 21:45:44.362 | elapsed=865.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.362 | end=2026-04-17 21:45:44.362 | elapsed=232.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.362 | end=2026-04-17 21:45:44.362 | elapsed=232.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.362 | end=2026-04-17 21:45:44.362 | elapsed=232.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.362 | end=2026-04-17 21:45:44.363 | elapsed=846.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.363 | end=2026-04-17 21:45:44.363 | elapsed=187.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.363 | end=2026-04-17 21:45:44.363 | elapsed=186.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:45:44.396 | end=2026-04-17 21:45:44.396 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:45:44.396 | end=2026-04-17 21:45:44.400 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.401 | end=2026-04-17 21:45:44.403 | elapsed=0.002s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.403 | end=2026-04-17 21:45:44.407 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.400 | end=2026-04-17 21:45:44.408 | elapsed=0.008s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.411 | end=2026-04-17 21:45:44.413 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.410 | end=2026-04-17 21:45:44.413 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.409 | end=2026-04-17 21:45:44.413 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.408 | end=2026-04-17 21:45:44.413 | elapsed=0.004s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:45:44.414 | end=2026-04-17 21:45:44.416 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.416 | end=2026-04-17 21:45:44.417 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.417 | end=2026-04-17 21:45:44.417 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.417 | end=2026-04-17 21:45:44.418 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.418 | end=2026-04-17 21:45:44.418 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.418 | end=2026-04-17 21:45:44.419 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.416 | end=2026-04-17 21:45:44.419 | elapsed=0.002s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:45:44.419 | end=2026-04-17 21:45:44.419 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:45:44.419 | end=2026-04-17 21:45:44.419 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.420 | end=2026-04-17 21:45:44.420 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.420 | end=2026-04-17 21:45:44.420 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.420 | end=2026-04-17 21:45:44.420 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.419 | end=2026-04-17 21:45:44.420 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.420 | end=2026-04-17 21:45:44.421 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.421 | end=2026-04-17 21:45:44.421 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:45:44.454 | end=2026-04-17 21:45:44.454 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:45:44.454 | end=2026-04-17 21:45:44.458 | elapsed=3.774ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.460 | end=2026-04-17 21:45:44.462 | elapsed=2.271ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.462 | end=2026-04-17 21:45:44.466 | elapsed=3.773ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.458 | end=2026-04-17 21:45:44.467 | elapsed=8.360ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.470 | end=2026-04-17 21:45:44.471 | elapsed=1.371ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.469 | end=2026-04-17 21:45:44.471 | elapsed=2.613ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.467 | end=2026-04-17 21:45:44.471 | elapsed=3.679ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.467 | end=2026-04-17 21:45:44.471 | elapsed=4.596ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:45:44.473 | end=2026-04-17 21:45:44.475 | elapsed=1.818ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.475 | end=2026-04-17 21:45:44.475 | elapsed=0.457ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.475 | end=2026-04-17 21:45:44.476 | elapsed=0.459ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.476 | end=2026-04-17 21:45:44.476 | elapsed=0.464ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.476 | end=2026-04-17 21:45:44.477 | elapsed=0.458ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.477 | end=2026-04-17 21:45:44.477 | elapsed=0.464ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.475 | end=2026-04-17 21:45:44.477 | elapsed=2.354ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:45:44.477 | end=2026-04-17 21:45:44.478 | elapsed=0.773ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:45:44.477 | end=2026-04-17 21:45:44.478 | elapsed=0.884ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.478 | end=2026-04-17 21:45:44.478 | elapsed=0.232ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.478 | end=2026-04-17 21:45:44.479 | elapsed=0.230ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.479 | end=2026-04-17 21:45:44.479 | elapsed=0.232ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.478 | end=2026-04-17 21:45:44.479 | elapsed=0.879ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.479 | end=2026-04-17 21:45:44.479 | elapsed=0.186ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.479 | end=2026-04-17 21:45:44.479 | elapsed=0.184ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:45:44.513 | end=2026-04-17 21:45:44.513 | elapsed=3.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:45:44.514 | end=2026-04-17 21:45:44.517 | elapsed=3759.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.519 | end=2026-04-17 21:45:44.521 | elapsed=2271.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.521 | end=2026-04-17 21:45:44.525 | elapsed=3725.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.517 | end=2026-04-17 21:45:44.526 | elapsed=8331.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.529 | end=2026-04-17 21:45:44.530 | elapsed=1358.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.528 | end=2026-04-17 21:45:44.530 | elapsed=2586.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.527 | end=2026-04-17 21:45:44.530 | elapsed=3658.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.526 | end=2026-04-17 21:45:44.530 | elapsed=4571.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:45:44.532 | end=2026-04-17 21:45:44.534 | elapsed=1813.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.534 | end=2026-04-17 21:45:44.534 | elapsed=456.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.534 | end=2026-04-17 21:45:44.535 | elapsed=455.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.535 | end=2026-04-17 21:45:44.535 | elapsed=455.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.535 | end=2026-04-17 21:45:44.536 | elapsed=458.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.536 | end=2026-04-17 21:45:44.536 | elapsed=456.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.534 | end=2026-04-17 21:45:44.536 | elapsed=2316.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:45:44.536 | end=2026-04-17 21:45:44.537 | elapsed=538.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:45:44.536 | end=2026-04-17 21:45:44.537 | elapsed=600.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.537 | end=2026-04-17 21:45:44.537 | elapsed=230.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.537 | end=2026-04-17 21:45:44.537 | elapsed=229.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.537 | end=2026-04-17 21:45:44.538 | elapsed=231.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.537 | end=2026-04-17 21:45:44.538 | elapsed=820.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.538 | end=2026-04-17 21:45:44.538 | elapsed=186.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.538 | end=2026-04-17 21:45:44.538 | elapsed=185.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:45:44.572 | end=2026-04-17 21:45:44.572 | elapsed=2000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:45:44.572 | end=2026-04-17 21:45:44.575 | elapsed=3756000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.577 | end=2026-04-17 21:45:44.579 | elapsed=2271000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.579 | end=2026-04-17 21:45:44.583 | elapsed=3769000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:45:44.575 | end=2026-04-17 21:45:44.584 | elapsed=8349000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.587 | end=2026-04-17 21:45:44.588 | elapsed=1370000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.586 | end=2026-04-17 21:45:44.588 | elapsed=2616000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.585 | end=2026-04-17 21:45:44.588 | elapsed=3705000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:45:44.584 | end=2026-04-17 21:45:44.588 | elapsed=4626000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:45:44.590 | end=2026-04-17 21:45:44.592 | elapsed=1275000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.592 | end=2026-04-17 21:45:44.592 | elapsed=456000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.592 | end=2026-04-17 21:45:44.592 | elapsed=458000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.592 | end=2026-04-17 21:45:44.593 | elapsed=456000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.593 | end=2026-04-17 21:45:44.593 | elapsed=464000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.593 | end=2026-04-17 21:45:44.594 | elapsed=458000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:45:44.592 | end=2026-04-17 21:45:44.594 | elapsed=2337000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:45:44.594 | end=2026-04-17 21:45:44.595 | elapsed=773000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:45:44.594 | end=2026-04-17 21:45:44.595 | elapsed=883000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.595 | end=2026-04-17 21:45:44.595 | elapsed=232000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.595 | end=2026-04-17 21:45:44.595 | elapsed=231000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.595 | end=2026-04-17 21:45:44.596 | elapsed=231000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:45:44.595 | end=2026-04-17 21:45:44.596 | elapsed=891000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.596 | end=2026-04-17 21:45:44.596 | elapsed=186000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:45:44.596 | end=2026-04-17 21:45:44.596 | elapsed=186000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=3us  avg=3us  max=3us  →

[simpleWork]void simpleWork()
  count=1  min=3.408ms  avg=3.408ms  max=3.408ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=2.268ms  avg=2.268ms  max=2.268ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.768ms  avg=3.768ms  max=3.768ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=8.334ms  avg=8.334ms  max=8.334ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.370ms  avg=1.370ms  max=1.370ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.617ms  avg=2.617ms  max=2.617ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.684ms  avg=3.684ms  max=3.684ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=4.599ms  avg=4.599ms  max=4.599ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.812ms  avg=1.812ms  max=1.812ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=455us  avg=458us  max=461us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=2.326ms  avg=2.326ms  max=2.326ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=774us  avg=774us  max=774us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=868us  avg=868us  max=868us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=230us  avg=231us  max=232us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=858us  avg=858us  max=858us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=185us  avg=186us  max=187us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=683us  avg=683us  max=683us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=82us  avg=82us  max=82us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=360us  avg=369us  max=380us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.856ms  avg=1.856ms  max=1.856ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=757us  avg=757us  max=757us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.516ms  avg=1.516ms  max=1.516ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.507ms  avg=1.514ms  max=1.519ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=4.584ms  avg=4.584ms  max=4.584ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.801ms  avg=3.801ms  max=3.801ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=42.372ms  avg=42.372ms  max=42.372ms  →

[Demo::main]int main(int, char**)
  count=1  min=42.874ms  avg=42.874ms  max=42.874ms  →
```

<!-- markdownlint-enable MD013 -->
