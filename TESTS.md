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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:45:10.326 | end=2026-04-16 11:45:10.326 | elapsed=3.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:45:10.327 | end=2026-04-16 11:45:10.330 | elapsed=3.138ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.331 | end=2026-04-16 11:45:10.333 | elapsed=1.884ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.333 | end=2026-04-16 11:45:10.336 | elapsed=3.133ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.330 | end=2026-04-16 11:45:10.337 | elapsed=6.960ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.340 | end=2026-04-16 11:45:10.341 | elapsed=1.137ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.339 | end=2026-04-16 11:45:10.341 | elapsed=2.182ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.338 | end=2026-04-16 11:45:10.341 | elapsed=3.112ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.337 | end=2026-04-16 11:45:10.341 | elapsed=3.876ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:45:10.342 | end=2026-04-16 11:45:10.344 | elapsed=1.515ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.344 | end=2026-04-16 11:45:10.344 | elapsed=390.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.344 | end=2026-04-16 11:45:10.345 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.345 | end=2026-04-16 11:45:10.345 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.345 | end=2026-04-16 11:45:10.345 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.345 | end=2026-04-16 11:45:10.346 | elapsed=381.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.344 | end=2026-04-16 11:45:10.346 | elapsed=1.970ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:45:10.346 | end=2026-04-16 11:45:10.346 | elapsed=634.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:45:10.346 | end=2026-04-16 11:45:10.346 | elapsed=691.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.347 | end=2026-04-16 11:45:10.347 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.347 | end=2026-04-16 11:45:10.347 | elapsed=205.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.347 | end=2026-04-16 11:45:10.347 | elapsed=196.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.346 | end=2026-04-16 11:45:10.347 | elapsed=837.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.347 | end=2026-04-16 11:45:10.348 | elapsed=166.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.348 | end=2026-04-16 11:45:10.348 | elapsed=157.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:45:10.375 | end=2026-04-16 11:45:10.375 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:45:10.375 | end=2026-04-16 11:45:10.378 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.380 | end=2026-04-16 11:45:10.382 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.382 | end=2026-04-16 11:45:10.385 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.378 | end=2026-04-16 11:45:10.385 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.388 | end=2026-04-16 11:45:10.389 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.387 | end=2026-04-16 11:45:10.389 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.386 | end=2026-04-16 11:45:10.389 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.385 | end=2026-04-16 11:45:10.389 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:45:10.391 | end=2026-04-16 11:45:10.392 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.392 | end=2026-04-16 11:45:10.393 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.393 | end=2026-04-16 11:45:10.393 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.393 | end=2026-04-16 11:45:10.393 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.393 | end=2026-04-16 11:45:10.394 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.394 | end=2026-04-16 11:45:10.394 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.392 | end=2026-04-16 11:45:10.394 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:45:10.394 | end=2026-04-16 11:45:10.395 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:45:10.394 | end=2026-04-16 11:45:10.395 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.395 | end=2026-04-16 11:45:10.395 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.395 | end=2026-04-16 11:45:10.395 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.395 | end=2026-04-16 11:45:10.395 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.395 | end=2026-04-16 11:45:10.396 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.396 | end=2026-04-16 11:45:10.396 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.396 | end=2026-04-16 11:45:10.396 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:45:10.423 | end=2026-04-16 11:45:10.423 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:45:10.424 | end=2026-04-16 11:45:10.427 | elapsed=3.133ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.428 | end=2026-04-16 11:45:10.430 | elapsed=1.880ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.430 | end=2026-04-16 11:45:10.433 | elapsed=3.133ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.427 | end=2026-04-16 11:45:10.434 | elapsed=6.928ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.436 | end=2026-04-16 11:45:10.437 | elapsed=1.131ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.435 | end=2026-04-16 11:45:10.437 | elapsed=2.151ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.434 | end=2026-04-16 11:45:10.437 | elapsed=3.033ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.434 | end=2026-04-16 11:45:10.437 | elapsed=3.790ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:45:10.439 | end=2026-04-16 11:45:10.440 | elapsed=1.508ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.440 | end=2026-04-16 11:45:10.441 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.441 | end=2026-04-16 11:45:10.441 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.441 | end=2026-04-16 11:45:10.442 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.442 | end=2026-04-16 11:45:10.442 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.442 | end=2026-04-16 11:45:10.442 | elapsed=0.379ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.440 | end=2026-04-16 11:45:10.442 | elapsed=1.925ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:45:10.442 | end=2026-04-16 11:45:10.443 | elapsed=0.634ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:45:10.442 | end=2026-04-16 11:45:10.443 | elapsed=0.682ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.443 | end=2026-04-16 11:45:10.443 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.443 | end=2026-04-16 11:45:10.443 | elapsed=0.192ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.443 | end=2026-04-16 11:45:10.444 | elapsed=0.192ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.443 | end=2026-04-16 11:45:10.444 | elapsed=0.731ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.444 | end=2026-04-16 11:45:10.444 | elapsed=0.159ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.444 | end=2026-04-16 11:45:10.444 | elapsed=0.159ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:45:10.471 | end=2026-04-16 11:45:10.471 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:45:10.471 | end=2026-04-16 11:45:10.474 | elapsed=3125.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.475 | end=2026-04-16 11:45:10.477 | elapsed=1880.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.477 | end=2026-04-16 11:45:10.480 | elapsed=3132.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.474 | end=2026-04-16 11:45:10.481 | elapsed=6917.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.484 | end=2026-04-16 11:45:10.485 | elapsed=1131.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.483 | end=2026-04-16 11:45:10.485 | elapsed=2145.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.482 | end=2026-04-16 11:45:10.485 | elapsed=3029.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.481 | end=2026-04-16 11:45:10.485 | elapsed=3786.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:45:10.486 | end=2026-04-16 11:45:10.488 | elapsed=1506.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.488 | end=2026-04-16 11:45:10.488 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.488 | end=2026-04-16 11:45:10.489 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.489 | end=2026-04-16 11:45:10.489 | elapsed=378.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.489 | end=2026-04-16 11:45:10.489 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.489 | end=2026-04-16 11:45:10.490 | elapsed=379.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.488 | end=2026-04-16 11:45:10.490 | elapsed=1919.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:45:10.490 | end=2026-04-16 11:45:10.490 | elapsed=636.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:45:10.490 | end=2026-04-16 11:45:10.490 | elapsed=685.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.490 | end=2026-04-16 11:45:10.491 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.491 | end=2026-04-16 11:45:10.491 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.491 | end=2026-04-16 11:45:10.491 | elapsed=192.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.490 | end=2026-04-16 11:45:10.491 | elapsed=733.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.491 | end=2026-04-16 11:45:10.491 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.491 | end=2026-04-16 11:45:10.491 | elapsed=154.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:45:10.518 | end=2026-04-16 11:45:10.518 | elapsed=2000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:45:10.518 | end=2026-04-16 11:45:10.522 | elapsed=3136000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.523 | end=2026-04-16 11:45:10.525 | elapsed=1881000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.525 | end=2026-04-16 11:45:10.528 | elapsed=3131000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:45:10.522 | end=2026-04-16 11:45:10.528 | elapsed=6919000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.531 | end=2026-04-16 11:45:10.532 | elapsed=1130000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.530 | end=2026-04-16 11:45:10.532 | elapsed=2144000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.529 | end=2026-04-16 11:45:10.532 | elapsed=3027000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:45:10.528 | end=2026-04-16 11:45:10.532 | elapsed=3787000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:45:10.534 | end=2026-04-16 11:45:10.535 | elapsed=1505000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.535 | end=2026-04-16 11:45:10.536 | elapsed=379000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.536 | end=2026-04-16 11:45:10.536 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.536 | end=2026-04-16 11:45:10.536 | elapsed=380000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.536 | end=2026-04-16 11:45:10.537 | elapsed=379000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.537 | end=2026-04-16 11:45:10.537 | elapsed=378000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:45:10.535 | end=2026-04-16 11:45:10.537 | elapsed=1920000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:45:10.537 | end=2026-04-16 11:45:10.538 | elapsed=633000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:45:10.537 | end=2026-04-16 11:45:10.538 | elapsed=684000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.538 | end=2026-04-16 11:45:10.538 | elapsed=192000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.538 | end=2026-04-16 11:45:10.538 | elapsed=191000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.538 | end=2026-04-16 11:45:10.538 | elapsed=190000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:45:10.538 | end=2026-04-16 11:45:10.539 | elapsed=718000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.539 | end=2026-04-16 11:45:10.539 | elapsed=154000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:45:10.539 | end=2026-04-16 11:45:10.539 | elapsed=154000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=2us  avg=2us  max=2us  →

[simpleWork]void simpleWork()
  count=1  min=3.134ms  avg=3.134ms  max=3.134ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.879ms  avg=1.879ms  max=1.879ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.133ms  avg=3.133ms  max=3.133ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.916ms  avg=6.916ms  max=6.916ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.130ms  avg=1.130ms  max=1.130ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.140ms  avg=2.140ms  max=2.140ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.027ms  avg=3.027ms  max=3.027ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.783ms  avg=3.783ms  max=3.783ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.506ms  avg=1.506ms  max=1.506ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=378us  avg=379us  max=380us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.915ms  avg=1.915ms  max=1.915ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=636us  avg=636us  max=636us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=685us  avg=685us  max=685us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=192us  avg=192us  max=193us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=700us  avg=700us  max=700us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=154us  avg=156us  max=159us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=571us  avg=571us  max=571us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=55us  avg=55us  max=55us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=310us  avg=313us  max=316us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.574ms  avg=1.574ms  max=1.574ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=632us  avg=632us  max=632us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.256ms  avg=1.256ms  max=1.256ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.255ms  avg=1.257ms  max=1.258ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.786ms  avg=3.786ms  max=3.786ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.141ms  avg=3.141ms  max=3.141ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=34.084ms  avg=34.084ms  max=34.084ms  →

[Demo::main]int main(int, char**)
  count=1  min=34.519ms  avg=34.519ms  max=34.519ms  →
```

<!-- markdownlint-enable MD013 -->
