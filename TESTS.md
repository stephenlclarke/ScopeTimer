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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:09:09.122 | end=2026-04-17 21:09:09.122 | elapsed=1.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:09:09.122 | end=2026-04-17 21:09:09.126 | elapsed=3.148ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.127 | end=2026-04-17 21:09:09.129 | elapsed=1.907ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.129 | end=2026-04-17 21:09:09.132 | elapsed=3.151ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.126 | end=2026-04-17 21:09:09.133 | elapsed=7.014ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.135 | end=2026-04-17 21:09:09.136 | elapsed=1.145ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.134 | end=2026-04-17 21:09:09.136 | elapsed=2.175ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.133 | end=2026-04-17 21:09:09.136 | elapsed=3.067ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.133 | end=2026-04-17 21:09:09.136 | elapsed=3.841ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:09:09.138 | end=2026-04-17 21:09:09.139 | elapsed=1.527ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.139 | end=2026-04-17 21:09:09.140 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.140 | end=2026-04-17 21:09:09.140 | elapsed=390.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.140 | end=2026-04-17 21:09:09.141 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.141 | end=2026-04-17 21:09:09.141 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.141 | end=2026-04-17 21:09:09.141 | elapsed=384.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.139 | end=2026-04-17 21:09:09.141 | elapsed=1.966ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:09:09.141 | end=2026-04-17 21:09:09.142 | elapsed=632.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:09:09.141 | end=2026-04-17 21:09:09.142 | elapsed=741.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.142 | end=2026-04-17 21:09:09.142 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.142 | end=2026-04-17 21:09:09.143 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.143 | end=2026-04-17 21:09:09.143 | elapsed=192.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.142 | end=2026-04-17 21:09:09.143 | elapsed=721.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.143 | end=2026-04-17 21:09:09.143 | elapsed=159.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.143 | end=2026-04-17 21:09:09.143 | elapsed=163.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:09:09.174 | end=2026-04-17 21:09:09.174 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:09:09.174 | end=2026-04-17 21:09:09.177 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.179 | end=2026-04-17 21:09:09.181 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.181 | end=2026-04-17 21:09:09.184 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.177 | end=2026-04-17 21:09:09.184 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.187 | end=2026-04-17 21:09:09.188 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.186 | end=2026-04-17 21:09:09.188 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.185 | end=2026-04-17 21:09:09.188 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.184 | end=2026-04-17 21:09:09.188 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:09:09.190 | end=2026-04-17 21:09:09.191 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.191 | end=2026-04-17 21:09:09.192 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.192 | end=2026-04-17 21:09:09.192 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.192 | end=2026-04-17 21:09:09.193 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.193 | end=2026-04-17 21:09:09.193 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.193 | end=2026-04-17 21:09:09.193 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.191 | end=2026-04-17 21:09:09.193 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:09:09.193 | end=2026-04-17 21:09:09.194 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:09:09.193 | end=2026-04-17 21:09:09.194 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.194 | end=2026-04-17 21:09:09.194 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.194 | end=2026-04-17 21:09:09.195 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.195 | end=2026-04-17 21:09:09.195 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.194 | end=2026-04-17 21:09:09.195 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.195 | end=2026-04-17 21:09:09.195 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.195 | end=2026-04-17 21:09:09.195 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:09:09.226 | end=2026-04-17 21:09:09.226 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:09:09.226 | end=2026-04-17 21:09:09.229 | elapsed=3.146ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.231 | end=2026-04-17 21:09:09.232 | elapsed=1.897ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.232 | end=2026-04-17 21:09:09.235 | elapsed=2.722ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.229 | end=2026-04-17 21:09:09.236 | elapsed=6.557ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.239 | end=2026-04-17 21:09:09.240 | elapsed=1.148ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.237 | end=2026-04-17 21:09:09.240 | elapsed=2.183ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.237 | end=2026-04-17 21:09:09.240 | elapsed=3.092ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.236 | end=2026-04-17 21:09:09.240 | elapsed=3.868ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:09:09.241 | end=2026-04-17 21:09:09.243 | elapsed=1.508ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.243 | end=2026-04-17 21:09:09.243 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.243 | end=2026-04-17 21:09:09.244 | elapsed=0.389ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.244 | end=2026-04-17 21:09:09.244 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.244 | end=2026-04-17 21:09:09.244 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.244 | end=2026-04-17 21:09:09.245 | elapsed=0.380ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.243 | end=2026-04-17 21:09:09.245 | elapsed=1.949ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:09:09.245 | end=2026-04-17 21:09:09.245 | elapsed=0.640ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:09:09.245 | end=2026-04-17 21:09:09.245 | elapsed=0.702ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.246 | end=2026-04-17 21:09:09.246 | elapsed=0.196ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.246 | end=2026-04-17 21:09:09.246 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.246 | end=2026-04-17 21:09:09.246 | elapsed=0.194ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.245 | end=2026-04-17 21:09:09.246 | elapsed=0.765ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.246 | end=2026-04-17 21:09:09.246 | elapsed=0.158ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.246 | end=2026-04-17 21:09:09.247 | elapsed=0.156ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:09:09.277 | end=2026-04-17 21:09:09.277 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:09:09.277 | end=2026-04-17 21:09:09.280 | elapsed=3144.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.282 | end=2026-04-17 21:09:09.284 | elapsed=1890.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.284 | end=2026-04-17 21:09:09.287 | elapsed=3143.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.280 | end=2026-04-17 21:09:09.287 | elapsed=6962.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.290 | end=2026-04-17 21:09:09.291 | elapsed=1140.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.289 | end=2026-04-17 21:09:09.291 | elapsed=2175.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.288 | end=2026-04-17 21:09:09.291 | elapsed=3079.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.287 | end=2026-04-17 21:09:09.291 | elapsed=3853.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:09:09.293 | end=2026-04-17 21:09:09.294 | elapsed=1506.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.294 | end=2026-04-17 21:09:09.295 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.295 | end=2026-04-17 21:09:09.295 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.295 | end=2026-04-17 21:09:09.295 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.295 | end=2026-04-17 21:09:09.296 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.296 | end=2026-04-17 21:09:09.296 | elapsed=382.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.294 | end=2026-04-17 21:09:09.296 | elapsed=1947.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:09:09.296 | end=2026-04-17 21:09:09.297 | elapsed=648.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:09:09.296 | end=2026-04-17 21:09:09.297 | elapsed=742.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.297 | end=2026-04-17 21:09:09.297 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.297 | end=2026-04-17 21:09:09.297 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.297 | end=2026-04-17 21:09:09.298 | elapsed=196.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.297 | end=2026-04-17 21:09:09.298 | elapsed=737.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.298 | end=2026-04-17 21:09:09.298 | elapsed=158.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.298 | end=2026-04-17 21:09:09.298 | elapsed=156.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 21:09:09.328 | end=2026-04-17 21:09:09.328 | elapsed=1000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 21:09:09.328 | end=2026-04-17 21:09:09.331 | elapsed=3145000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.332 | end=2026-04-17 21:09:09.334 | elapsed=1886000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.334 | end=2026-04-17 21:09:09.337 | elapsed=3144000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 21:09:09.331 | end=2026-04-17 21:09:09.338 | elapsed=6966000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.341 | end=2026-04-17 21:09:09.342 | elapsed=1140000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.340 | end=2026-04-17 21:09:09.342 | elapsed=2164000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.339 | end=2026-04-17 21:09:09.342 | elapsed=3057000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 21:09:09.338 | end=2026-04-17 21:09:09.342 | elapsed=3818000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 21:09:09.343 | end=2026-04-17 21:09:09.345 | elapsed=1523000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.345 | end=2026-04-17 21:09:09.345 | elapsed=383000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.345 | end=2026-04-17 21:09:09.346 | elapsed=382000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.346 | end=2026-04-17 21:09:09.346 | elapsed=391000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.346 | end=2026-04-17 21:09:09.346 | elapsed=382000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.346 | end=2026-04-17 21:09:09.347 | elapsed=381000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 21:09:09.345 | end=2026-04-17 21:09:09.347 | elapsed=1958000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 21:09:09.347 | end=2026-04-17 21:09:09.348 | elapsed=650000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 21:09:09.347 | end=2026-04-17 21:09:09.348 | elapsed=736000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.348 | end=2026-04-17 21:09:09.348 | elapsed=194000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.348 | end=2026-04-17 21:09:09.348 | elapsed=195000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.348 | end=2026-04-17 21:09:09.348 | elapsed=193000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 21:09:09.348 | end=2026-04-17 21:09:09.348 | elapsed=760000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.348 | end=2026-04-17 21:09:09.349 | elapsed=156000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 21:09:09.349 | end=2026-04-17 21:09:09.349 | elapsed=158000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=1us  avg=1us  max=1us  →

[simpleWork]void simpleWork()
  count=1  min=3.143ms  avg=3.143ms  max=3.143ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.883ms  avg=1.883ms  max=1.883ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.144ms  avg=3.144ms  max=3.144ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.971ms  avg=6.971ms  max=6.971ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.133ms  avg=1.133ms  max=1.133ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.172ms  avg=2.172ms  max=2.172ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.070ms  avg=3.070ms  max=3.070ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.850ms  avg=3.850ms  max=3.850ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.522ms  avg=1.522ms  max=1.522ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=381us  avg=384us  max=389us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.958ms  avg=1.958ms  max=1.958ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=638us  avg=638us  max=638us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=692us  avg=692us  max=692us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=194us  avg=195us  max=196us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=773us  avg=773us  max=773us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=157us  avg=160us  max=161us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=611us  avg=611us  max=611us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=79us  avg=79us  max=79us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=327us  avg=336us  max=341us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.692ms  avg=1.692ms  max=1.692ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=643us  avg=643us  max=643us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.268ms  avg=1.268ms  max=1.268ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.256ms  avg=1.267ms  max=1.276ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.822ms  avg=3.822ms  max=3.822ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.180ms  avg=3.180ms  max=3.180ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=35.878ms  avg=35.878ms  max=35.878ms  →

[Demo::main]int main(int, char**)
  count=1  min=36.326ms  avg=36.326ms  max=36.326ms  →
```

<!-- markdownlint-enable MD013 -->
