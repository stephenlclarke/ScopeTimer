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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 19:19:10.293 | end=2026-07-11 19:19:10.293 | elapsed=2.333us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 19:19:10.294 | end=2026-07-11 19:19:10.297 | elapsed=3.132ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.298 | end=2026-07-11 19:19:10.300 | elapsed=1.892ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.300 | end=2026-07-11 19:19:10.302 | elapsed=2.515ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.297 | end=2026-07-11 19:19:10.303 | elapsed=6.333ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.306 | end=2026-07-11 19:19:10.307 | elapsed=1.132ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.305 | end=2026-07-11 19:19:10.307 | elapsed=2.152ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.304 | end=2026-07-11 19:19:10.307 | elapsed=3.049ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.303 | end=2026-07-11 19:19:10.307 | elapsed=3.818ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 19:19:10.308 | end=2026-07-11 19:19:10.310 | elapsed=1.517ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.310 | end=2026-07-11 19:19:10.310 | elapsed=384.792us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.310 | end=2026-07-11 19:19:10.311 | elapsed=380.125us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.311 | end=2026-07-11 19:19:10.311 | elapsed=381.208us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.311 | end=2026-07-11 19:19:10.311 | elapsed=379.625us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.311 | end=2026-07-11 19:19:10.312 | elapsed=379.625us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.310 | end=2026-07-11 19:19:10.312 | elapsed=1.933ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 19:19:10.312 | end=2026-07-11 19:19:10.312 | elapsed=636.167us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 19:19:10.312 | end=2026-07-11 19:19:10.313 | elapsed=684.375us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.313 | elapsed=192.625us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.313 | elapsed=193.250us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.313 | elapsed=194.500us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.313 | elapsed=595.208us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.313 | elapsed=154.750us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.313 | end=2026-07-11 19:19:10.314 | elapsed=155.166us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 19:19:10.333 | end=2026-07-11 19:19:10.333 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 19:19:10.333 | end=2026-07-11 19:19:10.336 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.337 | end=2026-07-11 19:19:10.339 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.339 | end=2026-07-11 19:19:10.342 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.336 | end=2026-07-11 19:19:10.343 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.346 | end=2026-07-11 19:19:10.347 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.345 | end=2026-07-11 19:19:10.347 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.344 | end=2026-07-11 19:19:10.347 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.343 | end=2026-07-11 19:19:10.347 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 19:19:10.348 | end=2026-07-11 19:19:10.350 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.350 | end=2026-07-11 19:19:10.350 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.350 | end=2026-07-11 19:19:10.351 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.351 | end=2026-07-11 19:19:10.351 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.351 | end=2026-07-11 19:19:10.351 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.351 | end=2026-07-11 19:19:10.352 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.350 | end=2026-07-11 19:19:10.352 | elapsed=0.001s
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 19:19:10.352 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 19:19:10.352 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.353 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.353 | end=2026-07-11 19:19:10.354 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 19:19:10.374 | end=2026-07-11 19:19:10.374 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 19:19:10.374 | end=2026-07-11 19:19:10.377 | elapsed=3.137ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.378 | end=2026-07-11 19:19:10.380 | elapsed=1.883ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.380 | end=2026-07-11 19:19:10.383 | elapsed=3.074ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.377 | end=2026-07-11 19:19:10.384 | elapsed=6.888ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.387 | end=2026-07-11 19:19:10.388 | elapsed=1.142ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.386 | end=2026-07-11 19:19:10.388 | elapsed=2.168ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.385 | end=2026-07-11 19:19:10.388 | elapsed=3.063ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.384 | end=2026-07-11 19:19:10.388 | elapsed=3.827ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 19:19:10.389 | end=2026-07-11 19:19:10.391 | elapsed=1.511ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.391 | end=2026-07-11 19:19:10.391 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.391 | end=2026-07-11 19:19:10.392 | elapsed=0.379ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.392 | end=2026-07-11 19:19:10.392 | elapsed=0.379ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.392 | end=2026-07-11 19:19:10.392 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.392 | end=2026-07-11 19:19:10.393 | elapsed=0.380ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.391 | end=2026-07-11 19:19:10.393 | elapsed=1.937ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 19:19:10.393 | end=2026-07-11 19:19:10.393 | elapsed=0.643ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 19:19:10.393 | end=2026-07-11 19:19:10.393 | elapsed=0.732ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.393 | end=2026-07-11 19:19:10.394 | elapsed=0.196ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.394 | end=2026-07-11 19:19:10.394 | elapsed=0.196ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.394 | end=2026-07-11 19:19:10.394 | elapsed=0.192ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.393 | end=2026-07-11 19:19:10.394 | elapsed=0.608ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.394 | end=2026-07-11 19:19:10.394 | elapsed=0.154ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.394 | end=2026-07-11 19:19:10.394 | elapsed=0.154ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 19:19:10.415 | end=2026-07-11 19:19:10.415 | elapsed=2.458us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 19:19:10.415 | end=2026-07-11 19:19:10.418 | elapsed=3139.958us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.420 | end=2026-07-11 19:19:10.421 | elapsed=1894.334us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.421 | end=2026-07-11 19:19:10.425 | elapsed=3139.709us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.418 | end=2026-07-11 19:19:10.425 | elapsed=6971.125us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.428 | end=2026-07-11 19:19:10.429 | elapsed=1136.291us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.427 | end=2026-07-11 19:19:10.429 | elapsed=2160.208us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.426 | end=2026-07-11 19:19:10.429 | elapsed=3060.667us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.425 | end=2026-07-11 19:19:10.429 | elapsed=3835.166us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 19:19:10.431 | end=2026-07-11 19:19:10.432 | elapsed=1514.875us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.432 | end=2026-07-11 19:19:10.433 | elapsed=381.500us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.433 | end=2026-07-11 19:19:10.433 | elapsed=380.750us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.433 | end=2026-07-11 19:19:10.433 | elapsed=380.166us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.433 | end=2026-07-11 19:19:10.434 | elapsed=380.958us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.434 | end=2026-07-11 19:19:10.434 | elapsed=379.334us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.432 | end=2026-07-11 19:19:10.434 | elapsed=1929.583us
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 19:19:10.434 | end=2026-07-11 19:19:10.435 | elapsed=637.917us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 19:19:10.434 | end=2026-07-11 19:19:10.435 | elapsed=706.875us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.435 | end=2026-07-11 19:19:10.435 | elapsed=197.334us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.435 | end=2026-07-11 19:19:10.435 | elapsed=192.250us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.435 | end=2026-07-11 19:19:10.435 | elapsed=194.083us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.435 | end=2026-07-11 19:19:10.435 | elapsed=607.791us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.436 | end=2026-07-11 19:19:10.436 | elapsed=155.083us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.436 | end=2026-07-11 19:19:10.436 | elapsed=154.917us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-11 19:19:10.456 | end=2026-07-11 19:19:10.456 | elapsed=2708ns
[simpleWork] TID=001 | void simpleWork() | start=2026-07-11 19:19:10.457 | end=2026-07-11 19:19:10.460 | elapsed=3139958ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.461 | end=2026-07-11 19:19:10.463 | elapsed=1883000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.463 | end=2026-07-11 19:19:10.466 | elapsed=3002958ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-11 19:19:10.460 | end=2026-07-11 19:19:10.467 | elapsed=6806791ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.469 | end=2026-07-11 19:19:10.470 | elapsed=1129917ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.468 | end=2026-07-11 19:19:10.470 | elapsed=2146166ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.467 | end=2026-07-11 19:19:10.470 | elapsed=3034125ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-11 19:19:10.467 | end=2026-07-11 19:19:10.470 | elapsed=3824625ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-11 19:19:10.472 | end=2026-07-11 19:19:10.473 | elapsed=1515166ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.473 | end=2026-07-11 19:19:10.474 | elapsed=380084ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.474 | end=2026-07-11 19:19:10.474 | elapsed=372584ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.474 | end=2026-07-11 19:19:10.475 | elapsed=381959ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.475 | end=2026-07-11 19:19:10.475 | elapsed=381042ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.475 | end=2026-07-11 19:19:10.475 | elapsed=380917ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-11 19:19:10.473 | end=2026-07-11 19:19:10.475 | elapsed=1929000ns
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-11 19:19:10.475 | end=2026-07-11 19:19:10.476 | elapsed=639417ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-11 19:19:10.475 | end=2026-07-11 19:19:10.476 | elapsed=691666ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.476 | end=2026-07-11 19:19:10.476 | elapsed=194125ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.476 | end=2026-07-11 19:19:10.476 | elapsed=192667ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.476 | end=2026-07-11 19:19:10.477 | elapsed=193125ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-11 19:19:10.476 | end=2026-07-11 19:19:10.477 | elapsed=603167ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.477 | end=2026-07-11 19:19:10.477 | elapsed=155917ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-11 19:19:10.477 | end=2026-07-11 19:19:10.477 | elapsed=158667ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char **)
  count=1  min=3us  avg=3us  max=3us  →

[simpleWork]void simpleWork()
  count=1  min=3.135ms  avg=3.135ms  max=3.135ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.894ms  avg=1.894ms  max=1.894ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.137ms  avg=3.137ms  max=3.137ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.962ms  avg=6.962ms  max=6.962ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.137ms  avg=1.137ms  max=1.137ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.162ms  avg=2.162ms  max=2.162ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.058ms  avg=3.058ms  max=3.058ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.828ms  avg=3.828ms  max=3.828ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.512ms  avg=1.512ms  max=1.512ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=381us  avg=382us  max=384us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.946ms  avg=1.946ms  max=1.946ms  →

[threadedWork:worker]auto threadedWork(int)::(anonymous class)::operator()() const
  count=1  min=640us  avg=640us  max=640us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=705us  avg=705us  max=705us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=194us  avg=194us  max=195us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=607us  avg=607us  max=607us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=159us  avg=160us  max=160us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=485us  avg=485us  max=485us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=104us  avg=104us  max=104us  →

[hotPath:processBatch]std::uint64_t processTelemetryBatchExample(const std::vector<TelemetryEvent> &, TelemetryTotals &)
  count=5  min=436us  avg=445us  max=457us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=2.239ms  avg=2.239ms  max=2.239ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=635us  avg=635us  max=635us  →

[Worker:singleTask]void Worker::doTask(const std::string_view) const
  count=1  min=1.264ms  avg=1.264ms  max=1.264ms  →

[Worker:task]void Worker::doTask(const std::string_view) const
  count=5  min=1.256ms  avg=1.258ms  max=1.263ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.795ms  avg=3.795ms  max=3.795ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.164ms  avg=3.164ms  max=3.164ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=36.402ms  avg=36.402ms  max=36.402ms  →

[Demo::main]int main(int, char **)
  count=1  min=36.905ms  avg=36.905ms  max=36.905ms  →
```

<!-- markdownlint-enable MD013 -->
