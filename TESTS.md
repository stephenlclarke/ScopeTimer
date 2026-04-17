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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:55:02.192 | end=2026-04-17 20:55:02.192 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:55:02.193 | end=2026-04-17 20:55:02.196 | elapsed=3.146ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.197 | end=2026-04-17 20:55:02.199 | elapsed=1.887ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.199 | end=2026-04-17 20:55:02.202 | elapsed=3.146ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.196 | end=2026-04-17 20:55:02.203 | elapsed=6.980ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.206 | end=2026-04-17 20:55:02.207 | elapsed=1.143ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.205 | end=2026-04-17 20:55:02.207 | elapsed=2.171ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.204 | end=2026-04-17 20:55:02.207 | elapsed=3.059ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.203 | end=2026-04-17 20:55:02.207 | elapsed=3.836ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:55:02.208 | end=2026-04-17 20:55:02.210 | elapsed=1.525ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.210 | end=2026-04-17 20:55:02.210 | elapsed=387.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.210 | end=2026-04-17 20:55:02.211 | elapsed=386.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.211 | end=2026-04-17 20:55:02.211 | elapsed=385.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.211 | end=2026-04-17 20:55:02.211 | elapsed=385.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.211 | end=2026-04-17 20:55:02.212 | elapsed=384.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.210 | end=2026-04-17 20:55:02.212 | elapsed=1.963ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:55:02.212 | end=2026-04-17 20:55:02.212 | elapsed=653.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:55:02.212 | end=2026-04-17 20:55:02.212 | elapsed=731.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.213 | end=2026-04-17 20:55:02.213 | elapsed=196.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.213 | end=2026-04-17 20:55:02.213 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.213 | end=2026-04-17 20:55:02.213 | elapsed=198.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.212 | end=2026-04-17 20:55:02.213 | elapsed=694.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.213 | end=2026-04-17 20:55:02.213 | elapsed=157.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.213 | end=2026-04-17 20:55:02.214 | elapsed=160.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:55:02.243 | end=2026-04-17 20:55:02.243 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:55:02.243 | end=2026-04-17 20:55:02.246 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.248 | end=2026-04-17 20:55:02.250 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.250 | end=2026-04-17 20:55:02.253 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.246 | end=2026-04-17 20:55:02.253 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.256 | end=2026-04-17 20:55:02.257 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.255 | end=2026-04-17 20:55:02.257 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.254 | end=2026-04-17 20:55:02.257 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.253 | end=2026-04-17 20:55:02.257 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:55:02.259 | end=2026-04-17 20:55:02.260 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.260 | end=2026-04-17 20:55:02.261 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.261 | end=2026-04-17 20:55:02.261 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.261 | end=2026-04-17 20:55:02.261 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.261 | end=2026-04-17 20:55:02.262 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.262 | end=2026-04-17 20:55:02.262 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.260 | end=2026-04-17 20:55:02.262 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:55:02.262 | end=2026-04-17 20:55:02.263 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:55:02.262 | end=2026-04-17 20:55:02.263 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.263 | end=2026-04-17 20:55:02.263 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.263 | end=2026-04-17 20:55:02.263 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.263 | end=2026-04-17 20:55:02.264 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.263 | end=2026-04-17 20:55:02.264 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.264 | end=2026-04-17 20:55:02.264 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.264 | end=2026-04-17 20:55:02.264 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:55:02.293 | end=2026-04-17 20:55:02.293 | elapsed=0.001ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:55:02.293 | end=2026-04-17 20:55:02.296 | elapsed=3.146ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.298 | end=2026-04-17 20:55:02.300 | elapsed=1.899ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.300 | end=2026-04-17 20:55:02.303 | elapsed=3.149ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.296 | end=2026-04-17 20:55:02.303 | elapsed=7.021ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.306 | end=2026-04-17 20:55:02.307 | elapsed=1.141ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.305 | end=2026-04-17 20:55:02.307 | elapsed=2.173ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.304 | end=2026-04-17 20:55:02.307 | elapsed=3.073ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.303 | end=2026-04-17 20:55:02.307 | elapsed=3.843ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:55:02.309 | end=2026-04-17 20:55:02.310 | elapsed=1.516ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.310 | end=2026-04-17 20:55:02.311 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.311 | end=2026-04-17 20:55:02.311 | elapsed=0.385ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.311 | end=2026-04-17 20:55:02.312 | elapsed=0.387ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.312 | end=2026-04-17 20:55:02.312 | elapsed=0.385ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.312 | end=2026-04-17 20:55:02.312 | elapsed=0.390ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.310 | end=2026-04-17 20:55:02.312 | elapsed=1.967ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:55:02.312 | end=2026-04-17 20:55:02.313 | elapsed=0.644ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:55:02.312 | end=2026-04-17 20:55:02.313 | elapsed=0.732ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.313 | end=2026-04-17 20:55:02.313 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.313 | end=2026-04-17 20:55:02.314 | elapsed=0.197ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.314 | end=2026-04-17 20:55:02.314 | elapsed=0.197ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.313 | end=2026-04-17 20:55:02.314 | elapsed=0.717ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.314 | end=2026-04-17 20:55:02.314 | elapsed=0.160ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.314 | end=2026-04-17 20:55:02.314 | elapsed=0.156ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:55:02.343 | end=2026-04-17 20:55:02.343 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:55:02.343 | end=2026-04-17 20:55:02.346 | elapsed=3143.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.348 | end=2026-04-17 20:55:02.350 | elapsed=1885.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.350 | end=2026-04-17 20:55:02.353 | elapsed=3135.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.347 | end=2026-04-17 20:55:02.353 | elapsed=6951.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.356 | end=2026-04-17 20:55:02.357 | elapsed=1141.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.355 | end=2026-04-17 20:55:02.357 | elapsed=2169.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.354 | end=2026-04-17 20:55:02.357 | elapsed=3061.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.353 | end=2026-04-17 20:55:02.357 | elapsed=3709.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:55:02.359 | end=2026-04-17 20:55:02.360 | elapsed=1507.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.360 | end=2026-04-17 20:55:02.361 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.361 | end=2026-04-17 20:55:02.361 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.361 | end=2026-04-17 20:55:02.361 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.361 | end=2026-04-17 20:55:02.362 | elapsed=394.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.362 | end=2026-04-17 20:55:02.362 | elapsed=385.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.360 | end=2026-04-17 20:55:02.362 | elapsed=1953.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:55:02.362 | end=2026-04-17 20:55:02.363 | elapsed=642.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:55:02.362 | end=2026-04-17 20:55:02.363 | elapsed=702.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.363 | end=2026-04-17 20:55:02.363 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.363 | end=2026-04-17 20:55:02.363 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.363 | end=2026-04-17 20:55:02.364 | elapsed=198.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.363 | end=2026-04-17 20:55:02.364 | elapsed=714.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.364 | end=2026-04-17 20:55:02.364 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.364 | end=2026-04-17 20:55:02.364 | elapsed=155.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:55:02.395 | end=2026-04-17 20:55:02.395 | elapsed=2000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:55:02.395 | end=2026-04-17 20:55:02.399 | elapsed=3153000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.400 | end=2026-04-17 20:55:02.402 | elapsed=1892000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.402 | end=2026-04-17 20:55:02.405 | elapsed=3147000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:55:02.399 | end=2026-04-17 20:55:02.406 | elapsed=6972000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.408 | end=2026-04-17 20:55:02.409 | elapsed=1135000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.407 | end=2026-04-17 20:55:02.409 | elapsed=2158000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.406 | end=2026-04-17 20:55:02.409 | elapsed=3052000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:55:02.406 | end=2026-04-17 20:55:02.409 | elapsed=3812000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:55:02.411 | end=2026-04-17 20:55:02.412 | elapsed=1519000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.412 | end=2026-04-17 20:55:02.413 | elapsed=383000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.413 | end=2026-04-17 20:55:02.413 | elapsed=382000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.413 | end=2026-04-17 20:55:02.414 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.414 | end=2026-04-17 20:55:02.414 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.414 | end=2026-04-17 20:55:02.414 | elapsed=389000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:55:02.412 | end=2026-04-17 20:55:02.414 | elapsed=1950000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:55:02.414 | end=2026-04-17 20:55:02.415 | elapsed=641000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:55:02.414 | end=2026-04-17 20:55:02.415 | elapsed=732000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.415 | end=2026-04-17 20:55:02.415 | elapsed=192000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.415 | end=2026-04-17 20:55:02.416 | elapsed=195000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.416 | end=2026-04-17 20:55:02.416 | elapsed=195000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:55:02.415 | end=2026-04-17 20:55:02.416 | elapsed=683000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.416 | end=2026-04-17 20:55:02.416 | elapsed=157000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:55:02.416 | end=2026-04-17 20:55:02.416 | elapsed=156000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=2us  avg=2us  max=2us  →

[simpleWork]void simpleWork()
  count=1  min=3.149ms  avg=3.149ms  max=3.149ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.886ms  avg=1.886ms  max=1.886ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.148ms  avg=3.148ms  max=3.148ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.960ms  avg=6.960ms  max=6.960ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.131ms  avg=1.131ms  max=1.131ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.154ms  avg=2.154ms  max=2.154ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.043ms  avg=3.043ms  max=3.043ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.805ms  avg=3.805ms  max=3.805ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.522ms  avg=1.522ms  max=1.522ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=385us  avg=391us  max=397us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.997ms  avg=1.997ms  max=1.997ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=647us  avg=647us  max=647us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=735us  avg=735us  max=735us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=193us  avg=197us  max=199us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=730us  avg=730us  max=730us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=158us  avg=160us  max=161us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=605us  avg=605us  max=605us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=81us  avg=81us  max=81us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=359us  avg=363us  max=372us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.826ms  avg=1.826ms  max=1.826ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=641us  avg=641us  max=641us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.268ms  avg=1.268ms  max=1.268ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.256ms  avg=1.258ms  max=1.260ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.789ms  avg=3.789ms  max=3.789ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.158ms  avg=3.158ms  max=3.158ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=36.074ms  avg=36.074ms  max=36.074ms  →

[Demo::main]int main(int, char**)
  count=1  min=36.463ms  avg=36.463ms  max=36.463ms  →
```

<!-- markdownlint-enable MD013 -->
