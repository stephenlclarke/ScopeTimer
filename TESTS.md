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
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 20:03:43.394 | end=2026-07-11 20:03:43.394 | elapsed=2.292us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 20:03:43.394 | end=2026-07-11 20:03:43.397 | elapsed=3.772ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.399 | end=2026-07-11 20:03:43.401 | elapsed=2.263ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.401 | end=2026-07-11 20:03:43.405 | elapsed=3.765ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.397 | end=2026-07-11 20:03:43.406 | elapsed=8.344ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.409 | end=2026-07-11 20:03:43.410 | elapsed=1.355ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.408 | end=2026-07-11 20:03:43.410 | elapsed=2.580ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.407 | end=2026-07-11 20:03:43.410 | elapsed=3.642ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.406 | end=2026-07-11 20:03:43.410 | elapsed=4.551ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 20:03:43.412 | end=2026-07-11 20:03:43.414 | elapsed=1.808ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.414 | end=2026-07-11 20:03:43.414 | elapsed=457.292us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.414 | end=2026-07-11 20:03:43.415 | elapsed=454.708us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.415 | end=2026-07-11 20:03:43.415 | elapsed=455.500us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.415 | end=2026-07-11 20:03:43.416 | elapsed=455.416us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.416 | end=2026-07-11 20:03:43.416 | elapsed=457.792us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.414 | end=2026-07-11 20:03:43.416 | elapsed=2.327ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 20:03:43.416 | end=2026-07-11 20:03:43.417 | elapsed=761.167us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 20:03:43.416 | end=2026-07-11 20:03:43.417 | elapsed=809.708us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.417 | end=2026-07-11 20:03:43.417 | elapsed=233.083us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.417 | end=2026-07-11 20:03:43.418 | elapsed=228.708us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.418 | end=2026-07-11 20:03:43.418 | elapsed=233.125us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.417 | end=2026-07-11 20:03:43.418 | elapsed=713.042us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.418 | end=2026-07-11 20:03:43.418 | elapsed=188.208us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.418 | end=2026-07-11 20:03:43.418 | elapsed=183.708us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 20:03:43.441 | end=2026-07-11 20:03:43.441 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 20:03:43.441 | end=2026-07-11 20:03:43.445 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.446 | end=2026-07-11 20:03:43.448 | elapsed=0.002s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.448 | end=2026-07-11 20:03:43.452 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.445 | end=2026-07-11 20:03:43.453 | elapsed=0.008s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.456 | end=2026-07-11 20:03:43.458 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.455 | end=2026-07-11 20:03:43.458 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.454 | end=2026-07-11 20:03:43.458 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.453 | end=2026-07-11 20:03:43.458 | elapsed=0.004s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 20:03:43.459 | end=2026-07-11 20:03:43.461 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.461 | end=2026-07-11 20:03:43.462 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.462 | end=2026-07-11 20:03:43.462 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.462 | end=2026-07-11 20:03:43.463 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.463 | end=2026-07-11 20:03:43.463 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.463 | end=2026-07-11 20:03:43.463 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.461 | end=2026-07-11 20:03:43.463 | elapsed=0.002s
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 20:03:43.464 | end=2026-07-11 20:03:43.464 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 20:03:43.463 | end=2026-07-11 20:03:43.464 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.464 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.465 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.465 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.464 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.465 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.465 | end=2026-07-11 20:03:43.465 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 20:03:43.487 | end=2026-07-11 20:03:43.487 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 20:03:43.487 | end=2026-07-11 20:03:43.491 | elapsed=3.758ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.492 | end=2026-07-11 20:03:43.495 | elapsed=2.256ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.495 | end=2026-07-11 20:03:43.498 | elapsed=3.756ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.491 | end=2026-07-11 20:03:43.499 | elapsed=8.306ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.502 | end=2026-07-11 20:03:43.504 | elapsed=1.355ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.501 | end=2026-07-11 20:03:43.504 | elapsed=2.573ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.500 | end=2026-07-11 20:03:43.504 | elapsed=3.630ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.499 | end=2026-07-11 20:03:43.504 | elapsed=4.538ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 20:03:43.506 | end=2026-07-11 20:03:43.507 | elapsed=1.805ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.507 | end=2026-07-11 20:03:43.508 | elapsed=0.455ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.508 | end=2026-07-11 20:03:43.508 | elapsed=0.456ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.508 | end=2026-07-11 20:03:43.509 | elapsed=0.455ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.509 | end=2026-07-11 20:03:43.509 | elapsed=0.454ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.509 | end=2026-07-11 20:03:43.510 | elapsed=0.454ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.507 | end=2026-07-11 20:03:43.510 | elapsed=2.307ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 20:03:43.510 | end=2026-07-11 20:03:43.511 | elapsed=0.761ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 20:03:43.510 | end=2026-07-11 20:03:43.511 | elapsed=0.804ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.511 | end=2026-07-11 20:03:43.511 | elapsed=0.230ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.511 | end=2026-07-11 20:03:43.511 | elapsed=0.230ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.511 | end=2026-07-11 20:03:43.511 | elapsed=0.231ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.511 | end=2026-07-11 20:03:43.511 | elapsed=0.711ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.511 | end=2026-07-11 20:03:43.512 | elapsed=0.183ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.512 | end=2026-07-11 20:03:43.512 | elapsed=0.187ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 20:03:43.533 | end=2026-07-11 20:03:43.533 | elapsed=2.708us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 20:03:43.534 | end=2026-07-11 20:03:43.537 | elapsed=3757.875us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.539 | end=2026-07-11 20:03:43.541 | elapsed=2256.166us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.541 | end=2026-07-11 20:03:43.545 | elapsed=3758.500us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.537 | end=2026-07-11 20:03:43.546 | elapsed=8307.459us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.549 | end=2026-07-11 20:03:43.550 | elapsed=1359.667us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.548 | end=2026-07-11 20:03:43.550 | elapsed=2581.542us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.547 | end=2026-07-11 20:03:43.550 | elapsed=3639.125us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.546 | end=2026-07-11 20:03:43.550 | elapsed=4547.958us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 20:03:43.552 | end=2026-07-11 20:03:43.554 | elapsed=1807.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.554 | end=2026-07-11 20:03:43.554 | elapsed=457.250us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.554 | end=2026-07-11 20:03:43.555 | elapsed=459.250us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.555 | end=2026-07-11 20:03:43.555 | elapsed=455.333us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.555 | end=2026-07-11 20:03:43.556 | elapsed=458.333us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.556 | end=2026-07-11 20:03:43.556 | elapsed=458.834us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.554 | end=2026-07-11 20:03:43.556 | elapsed=2333.000us
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 20:03:43.556 | end=2026-07-11 20:03:43.557 | elapsed=765.042us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 20:03:43.556 | end=2026-07-11 20:03:43.557 | elapsed=810.125us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.557 | end=2026-07-11 20:03:43.557 | elapsed=231.667us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.557 | end=2026-07-11 20:03:43.557 | elapsed=229.916us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.557 | end=2026-07-11 20:03:43.558 | elapsed=230.709us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.557 | end=2026-07-11 20:03:43.558 | elapsed=709.500us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.558 | end=2026-07-11 20:03:43.558 | elapsed=183.958us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.558 | end=2026-07-11 20:03:43.558 | elapsed=186.625us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 20:03:43.580 | end=2026-07-11 20:03:43.580 | elapsed=2334ns
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 20:03:43.580 | end=2026-07-11 20:03:43.583 | elapsed=3756625ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.585 | end=2026-07-11 20:03:43.587 | elapsed=2255666ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.587 | end=2026-07-11 20:03:43.591 | elapsed=3757625ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 20:03:43.584 | end=2026-07-11 20:03:43.592 | elapsed=8314750ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.595 | end=2026-07-11 20:03:43.596 | elapsed=1354875ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.594 | end=2026-07-11 20:03:43.596 | elapsed=2584000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.593 | end=2026-07-11 20:03:43.596 | elapsed=3648875ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 20:03:43.592 | end=2026-07-11 20:03:43.596 | elapsed=4560708ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 20:03:43.598 | end=2026-07-11 20:03:43.600 | elapsed=1809292ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.600 | end=2026-07-11 20:03:43.600 | elapsed=458916ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.600 | end=2026-07-11 20:03:43.601 | elapsed=457792ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.601 | end=2026-07-11 20:03:43.601 | elapsed=455250ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.601 | end=2026-07-11 20:03:43.602 | elapsed=454083ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.602 | end=2026-07-11 20:03:43.602 | elapsed=455167ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 20:03:43.600 | end=2026-07-11 20:03:43.602 | elapsed=2321250ns
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 20:03:43.602 | end=2026-07-11 20:03:43.603 | elapsed=760459ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 20:03:43.602 | end=2026-07-11 20:03:43.603 | elapsed=812625ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.603 | end=2026-07-11 20:03:43.603 | elapsed=229667ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.603 | end=2026-07-11 20:03:43.604 | elapsed=232292ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.604 | end=2026-07-11 20:03:43.604 | elapsed=230041ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 20:03:43.603 | end=2026-07-11 20:03:43.604 | elapsed=709250ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.604 | end=2026-07-11 20:03:43.604 | elapsed=185250ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 20:03:43.604 | end=2026-07-11 20:03:43.604 | elapsed=187917ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char **)
  count=1  min=2us  avg=2us  max=2us  →

[simpleWork]void simpleWork()
  count=1  min=3.757ms  avg=3.757ms  max=3.757ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=2.256ms  avg=2.256ms  max=2.256ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.761ms  avg=3.761ms  max=3.761ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=8.321ms  avg=8.321ms  max=8.321ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.358ms  avg=1.358ms  max=1.358ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.581ms  avg=2.581ms  max=2.581ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.641ms  avg=3.641ms  max=3.641ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=4.401ms  avg=4.401ms  max=4.401ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.806ms  avg=1.806ms  max=1.806ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=454us  avg=456us  max=460us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=2.315ms  avg=2.315ms  max=2.315ms  →

[threadedWork:worker]auto threadedWork(int)::(anonymous class)::operator()() const
  count=1  min=759us  avg=759us  max=759us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=803us  avg=803us  max=803us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=229us  avg=232us  max=234us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=715us  avg=715us  max=715us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=183us  avg=184us  max=185us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=555us  avg=555us  max=555us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=85us  avg=85us  max=85us  →

[hotPath:processBatch]std::uint64_t processTelemetryBatchExample(const std::vector<TelemetryEvent> &, TelemetryTotals &)
  count=5  min=290us  avg=292us  max=294us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.471ms  avg=1.471ms  max=1.471ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=758us  avg=758us  max=758us  →

[Worker:singleTask]void Worker::doTask(const std::string_view) const
  count=1  min=1.506ms  avg=1.506ms  max=1.506ms  →

[Worker:task]void Worker::doTask(const std::string_view) const
  count=5  min=1.505ms  avg=1.505ms  max=1.506ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=4.536ms  avg=4.536ms  max=4.536ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.768ms  avg=3.768ms  max=3.768ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=41.604ms  avg=41.604ms  max=41.604ms  →

[Demo::main]int main(int, char **)
  count=1  min=42.008ms  avg=42.008ms  max=42.008ms  →
```

<!-- markdownlint-enable MD013 -->
