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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 17:36:19.173 | end=2026-04-17 17:36:19.173 | elapsed=1.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 17:36:19.174 | end=2026-04-17 17:36:19.177 | elapsed=3.134ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.178 | end=2026-04-17 17:36:19.180 | elapsed=1.892ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.180 | end=2026-04-17 17:36:19.183 | elapsed=3.146ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.177 | end=2026-04-17 17:36:19.184 | elapsed=6.978ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.187 | end=2026-04-17 17:36:19.188 | elapsed=1.106ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.186 | end=2026-04-17 17:36:19.188 | elapsed=2.144ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.185 | end=2026-04-17 17:36:19.188 | elapsed=3.054ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.184 | end=2026-04-17 17:36:19.188 | elapsed=3.834ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 17:36:19.189 | end=2026-04-17 17:36:19.191 | elapsed=1.520ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.191 | end=2026-04-17 17:36:19.191 | elapsed=383.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.191 | end=2026-04-17 17:36:19.192 | elapsed=393.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.192 | end=2026-04-17 17:36:19.192 | elapsed=374.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.192 | end=2026-04-17 17:36:19.192 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.192 | end=2026-04-17 17:36:19.193 | elapsed=380.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.191 | end=2026-04-17 17:36:19.193 | elapsed=1.943ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 17:36:19.193 | end=2026-04-17 17:36:19.193 | elapsed=638.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 17:36:19.193 | end=2026-04-17 17:36:19.194 | elapsed=686.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.194 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.194 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.194 | elapsed=193.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.194 | elapsed=706.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.194 | elapsed=158.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.194 | end=2026-04-17 17:36:19.195 | elapsed=155.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 17:36:19.223 | end=2026-04-17 17:36:19.223 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 17:36:19.223 | end=2026-04-17 17:36:19.226 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.228 | end=2026-04-17 17:36:19.230 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.230 | end=2026-04-17 17:36:19.233 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.226 | end=2026-04-17 17:36:19.233 | elapsed=0.007s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.236 | end=2026-04-17 17:36:19.237 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.235 | end=2026-04-17 17:36:19.237 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.234 | end=2026-04-17 17:36:19.237 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.233 | end=2026-04-17 17:36:19.237 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 17:36:19.239 | end=2026-04-17 17:36:19.240 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.240 | end=2026-04-17 17:36:19.241 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.241 | end=2026-04-17 17:36:19.241 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.241 | end=2026-04-17 17:36:19.242 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.242 | end=2026-04-17 17:36:19.242 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.242 | end=2026-04-17 17:36:19.242 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.240 | end=2026-04-17 17:36:19.242 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 17:36:19.242 | end=2026-04-17 17:36:19.243 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 17:36:19.242 | end=2026-04-17 17:36:19.243 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.243 | end=2026-04-17 17:36:19.243 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.243 | end=2026-04-17 17:36:19.244 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.244 | end=2026-04-17 17:36:19.244 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.243 | end=2026-04-17 17:36:19.244 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.244 | end=2026-04-17 17:36:19.244 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.244 | end=2026-04-17 17:36:19.244 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 17:36:19.273 | end=2026-04-17 17:36:19.273 | elapsed=0.001ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 17:36:19.273 | end=2026-04-17 17:36:19.276 | elapsed=3.138ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.277 | end=2026-04-17 17:36:19.279 | elapsed=1.898ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.279 | end=2026-04-17 17:36:19.282 | elapsed=3.148ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.276 | end=2026-04-17 17:36:19.283 | elapsed=6.988ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.286 | end=2026-04-17 17:36:19.287 | elapsed=1.145ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.285 | end=2026-04-17 17:36:19.287 | elapsed=2.179ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.284 | end=2026-04-17 17:36:19.287 | elapsed=3.082ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.283 | end=2026-04-17 17:36:19.287 | elapsed=3.722ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 17:36:19.288 | end=2026-04-17 17:36:19.290 | elapsed=1.522ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.290 | end=2026-04-17 17:36:19.290 | elapsed=0.382ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.290 | end=2026-04-17 17:36:19.291 | elapsed=0.394ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.291 | end=2026-04-17 17:36:19.291 | elapsed=0.384ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.291 | end=2026-04-17 17:36:19.291 | elapsed=0.389ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.291 | end=2026-04-17 17:36:19.292 | elapsed=0.386ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.290 | end=2026-04-17 17:36:19.292 | elapsed=1.971ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 17:36:19.292 | end=2026-04-17 17:36:19.292 | elapsed=0.638ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 17:36:19.292 | end=2026-04-17 17:36:19.292 | elapsed=0.684ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.292 | end=2026-04-17 17:36:19.293 | elapsed=0.196ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.293 | end=2026-04-17 17:36:19.293 | elapsed=0.196ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.293 | end=2026-04-17 17:36:19.293 | elapsed=0.199ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.292 | end=2026-04-17 17:36:19.293 | elapsed=0.714ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.293 | end=2026-04-17 17:36:19.293 | elapsed=0.157ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.293 | end=2026-04-17 17:36:19.294 | elapsed=0.160ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 17:36:19.322 | end=2026-04-17 17:36:19.322 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 17:36:19.322 | end=2026-04-17 17:36:19.325 | elapsed=3148.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.327 | end=2026-04-17 17:36:19.328 | elapsed=1903.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.328 | end=2026-04-17 17:36:19.332 | elapsed=3147.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.325 | end=2026-04-17 17:36:19.332 | elapsed=7002.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.335 | end=2026-04-17 17:36:19.336 | elapsed=1147.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.334 | end=2026-04-17 17:36:19.336 | elapsed=2187.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.333 | end=2026-04-17 17:36:19.336 | elapsed=3091.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.332 | end=2026-04-17 17:36:19.336 | elapsed=3861.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 17:36:19.338 | end=2026-04-17 17:36:19.339 | elapsed=1508.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.339 | end=2026-04-17 17:36:19.340 | elapsed=383.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.340 | end=2026-04-17 17:36:19.340 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.340 | end=2026-04-17 17:36:19.340 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.340 | end=2026-04-17 17:36:19.341 | elapsed=389.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.341 | end=2026-04-17 17:36:19.341 | elapsed=387.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.339 | end=2026-04-17 17:36:19.341 | elapsed=1975.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 17:36:19.341 | end=2026-04-17 17:36:19.342 | elapsed=644.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 17:36:19.341 | end=2026-04-17 17:36:19.342 | elapsed=735.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.342 | end=2026-04-17 17:36:19.342 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.342 | end=2026-04-17 17:36:19.342 | elapsed=197.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.342 | end=2026-04-17 17:36:19.343 | elapsed=195.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.342 | end=2026-04-17 17:36:19.343 | elapsed=697.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.343 | end=2026-04-17 17:36:19.343 | elapsed=157.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.343 | end=2026-04-17 17:36:19.343 | elapsed=156.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 17:36:19.372 | end=2026-04-17 17:36:19.372 | elapsed=1000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 17:36:19.372 | end=2026-04-17 17:36:19.375 | elapsed=3151000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.376 | end=2026-04-17 17:36:19.378 | elapsed=1881000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.378 | end=2026-04-17 17:36:19.381 | elapsed=3146000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 17:36:19.375 | end=2026-04-17 17:36:19.382 | elapsed=6963000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.385 | end=2026-04-17 17:36:19.386 | elapsed=1145000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.384 | end=2026-04-17 17:36:19.386 | elapsed=2145000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.383 | end=2026-04-17 17:36:19.386 | elapsed=3048000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 17:36:19.382 | end=2026-04-17 17:36:19.386 | elapsed=3831000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 17:36:19.387 | end=2026-04-17 17:36:19.389 | elapsed=1523000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.389 | end=2026-04-17 17:36:19.389 | elapsed=386000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.389 | end=2026-04-17 17:36:19.390 | elapsed=393000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.390 | end=2026-04-17 17:36:19.390 | elapsed=387000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.390 | end=2026-04-17 17:36:19.390 | elapsed=385000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.390 | end=2026-04-17 17:36:19.391 | elapsed=383000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 17:36:19.389 | end=2026-04-17 17:36:19.391 | elapsed=1964000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 17:36:19.391 | end=2026-04-17 17:36:19.391 | elapsed=644000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 17:36:19.391 | end=2026-04-17 17:36:19.392 | elapsed=729000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.392 | elapsed=197000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.392 | elapsed=198000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.392 | elapsed=195000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.392 | elapsed=729000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.392 | elapsed=156000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 17:36:19.392 | end=2026-04-17 17:36:19.393 | elapsed=154000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=2us  avg=2us  max=2us  →

[simpleWork]void simpleWork()
  count=1  min=3.148ms  avg=3.148ms  max=3.148ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.885ms  avg=1.885ms  max=1.885ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.148ms  avg=3.148ms  max=3.148ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.964ms  avg=6.964ms  max=6.964ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.146ms  avg=1.146ms  max=1.146ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.178ms  avg=2.178ms  max=2.178ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.079ms  avg=3.079ms  max=3.079ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.849ms  avg=3.849ms  max=3.849ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.531ms  avg=1.531ms  max=1.531ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=385us  avg=389us  max=394us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.997ms  avg=1.997ms  max=1.997ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=649us  avg=649us  max=649us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=738us  avg=738us  max=738us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=192us  avg=193us  max=193us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=710us  avg=710us  max=710us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=155us  avg=158us  max=161us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=568us  avg=568us  max=568us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=59us  avg=59us  max=59us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=358us  avg=361us  max=367us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.817ms  avg=1.817ms  max=1.817ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=644us  avg=644us  max=644us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.279ms  avg=1.279ms  max=1.279ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.258ms  avg=1.268ms  max=1.274ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.847ms  avg=3.847ms  max=3.847ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.155ms  avg=3.155ms  max=3.155ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=34.892ms  avg=34.892ms  max=34.892ms  →

[Demo::main]int main(int, char**)
  count=1  min=35.357ms  avg=35.357ms  max=35.357ms  →
```

<!-- markdownlint-enable MD013 -->
