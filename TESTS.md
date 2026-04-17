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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:11:42.112 | end=2026-04-17 20:11:42.112 | elapsed=1.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:11:42.113 | end=2026-04-17 20:11:42.116 | elapsed=3.144ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.117 | end=2026-04-17 20:11:42.119 | elapsed=1.900ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.119 | end=2026-04-17 20:11:42.123 | elapsed=3.152ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.116 | end=2026-04-17 20:11:42.123 | elapsed=7.014ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.126 | end=2026-04-17 20:11:42.127 | elapsed=986.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.125 | end=2026-04-17 20:11:42.127 | elapsed=2.026ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.124 | end=2026-04-17 20:11:42.127 | elapsed=2.927ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.123 | end=2026-04-17 20:11:42.127 | elapsed=3.691ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:11:42.128 | end=2026-04-17 20:11:42.130 | elapsed=1.521ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.130 | end=2026-04-17 20:11:42.130 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.130 | end=2026-04-17 20:11:42.131 | elapsed=383.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.131 | end=2026-04-17 20:11:42.131 | elapsed=391.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.131 | end=2026-04-17 20:11:42.132 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.132 | end=2026-04-17 20:11:42.132 | elapsed=382.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.130 | end=2026-04-17 20:11:42.132 | elapsed=1.957ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:11:42.132 | end=2026-04-17 20:11:42.133 | elapsed=642.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:11:42.132 | end=2026-04-17 20:11:42.133 | elapsed=704.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.133 | end=2026-04-17 20:11:42.133 | elapsed=195.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.133 | end=2026-04-17 20:11:42.133 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.133 | end=2026-04-17 20:11:42.133 | elapsed=198.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.133 | end=2026-04-17 20:11:42.133 | elapsed=731.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.133 | end=2026-04-17 20:11:42.134 | elapsed=159.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.134 | end=2026-04-17 20:11:42.134 | elapsed=157.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:11:42.163 | end=2026-04-17 20:11:42.163 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:11:42.163 | end=2026-04-17 20:11:42.166 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.168 | end=2026-04-17 20:11:42.170 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.170 | end=2026-04-17 20:11:42.173 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.166 | end=2026-04-17 20:11:42.173 | elapsed=0.007s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.176 | end=2026-04-17 20:11:42.177 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.175 | end=2026-04-17 20:11:42.177 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.174 | end=2026-04-17 20:11:42.177 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.173 | end=2026-04-17 20:11:42.177 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:11:42.179 | end=2026-04-17 20:11:42.180 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.180 | end=2026-04-17 20:11:42.181 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.181 | end=2026-04-17 20:11:42.181 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.181 | end=2026-04-17 20:11:42.182 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.182 | end=2026-04-17 20:11:42.182 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.182 | end=2026-04-17 20:11:42.182 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.180 | end=2026-04-17 20:11:42.182 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:11:42.182 | end=2026-04-17 20:11:42.183 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:11:42.182 | end=2026-04-17 20:11:42.183 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.183 | end=2026-04-17 20:11:42.183 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.183 | end=2026-04-17 20:11:42.184 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.184 | end=2026-04-17 20:11:42.184 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.183 | end=2026-04-17 20:11:42.184 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.184 | end=2026-04-17 20:11:42.184 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.184 | end=2026-04-17 20:11:42.184 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:11:42.214 | end=2026-04-17 20:11:42.214 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:11:42.214 | end=2026-04-17 20:11:42.217 | elapsed=3.141ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.219 | end=2026-04-17 20:11:42.221 | elapsed=1.894ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.221 | end=2026-04-17 20:11:42.224 | elapsed=3.134ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.218 | end=2026-04-17 20:11:42.224 | elapsed=6.945ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.227 | end=2026-04-17 20:11:42.228 | elapsed=1.145ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.226 | end=2026-04-17 20:11:42.228 | elapsed=2.175ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.225 | end=2026-04-17 20:11:42.228 | elapsed=3.075ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.224 | end=2026-04-17 20:11:42.228 | elapsed=3.849ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:11:42.230 | end=2026-04-17 20:11:42.231 | elapsed=1.515ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.231 | end=2026-04-17 20:11:42.232 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.232 | end=2026-04-17 20:11:42.232 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.232 | end=2026-04-17 20:11:42.233 | elapsed=0.384ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.233 | end=2026-04-17 20:11:42.233 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.233 | end=2026-04-17 20:11:42.233 | elapsed=0.383ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.231 | end=2026-04-17 20:11:42.233 | elapsed=1.948ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:11:42.233 | end=2026-04-17 20:11:42.234 | elapsed=0.635ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:11:42.233 | end=2026-04-17 20:11:42.234 | elapsed=0.680ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.234 | end=2026-04-17 20:11:42.234 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.234 | end=2026-04-17 20:11:42.234 | elapsed=0.193ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.234 | end=2026-04-17 20:11:42.235 | elapsed=0.194ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.234 | end=2026-04-17 20:11:42.235 | elapsed=0.696ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.235 | end=2026-04-17 20:11:42.235 | elapsed=0.155ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.235 | end=2026-04-17 20:11:42.235 | elapsed=0.155ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:11:42.265 | end=2026-04-17 20:11:42.265 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:11:42.265 | end=2026-04-17 20:11:42.268 | elapsed=3142.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.270 | end=2026-04-17 20:11:42.272 | elapsed=1886.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.272 | end=2026-04-17 20:11:42.275 | elapsed=3136.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.268 | end=2026-04-17 20:11:42.275 | elapsed=6960.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.278 | end=2026-04-17 20:11:42.279 | elapsed=1140.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.277 | end=2026-04-17 20:11:42.279 | elapsed=2166.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.276 | end=2026-04-17 20:11:42.279 | elapsed=3062.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.275 | end=2026-04-17 20:11:42.279 | elapsed=3829.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:11:42.281 | end=2026-04-17 20:11:42.282 | elapsed=1518.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.282 | end=2026-04-17 20:11:42.283 | elapsed=384.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.283 | end=2026-04-17 20:11:42.283 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.283 | end=2026-04-17 20:11:42.283 | elapsed=383.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.283 | end=2026-04-17 20:11:42.284 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.284 | end=2026-04-17 20:11:42.284 | elapsed=382.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.282 | end=2026-04-17 20:11:42.284 | elapsed=1938.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:11:42.284 | end=2026-04-17 20:11:42.285 | elapsed=649.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:11:42.284 | end=2026-04-17 20:11:42.285 | elapsed=738.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.285 | end=2026-04-17 20:11:42.285 | elapsed=196.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.285 | end=2026-04-17 20:11:42.285 | elapsed=198.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.285 | end=2026-04-17 20:11:42.286 | elapsed=198.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.285 | end=2026-04-17 20:11:42.286 | elapsed=808.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.286 | end=2026-04-17 20:11:42.286 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.286 | end=2026-04-17 20:11:42.286 | elapsed=155.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-17 20:11:42.317 | end=2026-04-17 20:11:42.317 | elapsed=2000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-17 20:11:42.317 | end=2026-04-17 20:11:42.320 | elapsed=3103000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.321 | end=2026-04-17 20:11:42.323 | elapsed=1896000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.323 | end=2026-04-17 20:11:42.327 | elapsed=3142000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-17 20:11:42.320 | end=2026-04-17 20:11:42.327 | elapsed=6923000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.330 | end=2026-04-17 20:11:42.331 | elapsed=1142000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.329 | end=2026-04-17 20:11:42.331 | elapsed=2166000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.328 | end=2026-04-17 20:11:42.331 | elapsed=3061000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-17 20:11:42.327 | end=2026-04-17 20:11:42.331 | elapsed=3832000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-17 20:11:42.333 | end=2026-04-17 20:11:42.334 | elapsed=1522000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.334 | end=2026-04-17 20:11:42.334 | elapsed=383000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.334 | end=2026-04-17 20:11:42.335 | elapsed=388000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.335 | end=2026-04-17 20:11:42.335 | elapsed=384000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.335 | end=2026-04-17 20:11:42.336 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.336 | end=2026-04-17 20:11:42.336 | elapsed=379000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-17 20:11:42.334 | end=2026-04-17 20:11:42.336 | elapsed=1952000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-17 20:11:42.336 | end=2026-04-17 20:11:42.337 | elapsed=636000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-17 20:11:42.336 | end=2026-04-17 20:11:42.337 | elapsed=692000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.337 | end=2026-04-17 20:11:42.337 | elapsed=193000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.337 | end=2026-04-17 20:11:42.337 | elapsed=192000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.337 | end=2026-04-17 20:11:42.337 | elapsed=195000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-17 20:11:42.337 | end=2026-04-17 20:11:42.338 | elapsed=787000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.338 | end=2026-04-17 20:11:42.338 | elapsed=155000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-17 20:11:42.338 | end=2026-04-17 20:11:42.338 | elapsed=155000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=1us  avg=1us  max=1us  →

[simpleWork]void simpleWork()
  count=1  min=3.144ms  avg=3.144ms  max=3.144ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.896ms  avg=1.896ms  max=1.896ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.142ms  avg=3.142ms  max=3.142ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.984ms  avg=6.984ms  max=6.984ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.142ms  avg=1.142ms  max=1.142ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.179ms  avg=2.179ms  max=2.179ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.081ms  avg=3.081ms  max=3.081ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.856ms  avg=3.856ms  max=3.856ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.511ms  avg=1.511ms  max=1.511ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=380us  avg=384us  max=388us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.976ms  avg=1.976ms  max=1.976ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=646us  avg=646us  max=646us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=753us  avg=753us  max=753us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=193us  avg=193us  max=194us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=725us  avg=725us  max=725us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=145us  avg=152us  max=157us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=574us  avg=574us  max=574us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=76us  avg=76us  max=76us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=363us  avg=368us  max=373us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.851ms  avg=1.851ms  max=1.851ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=635us  avg=635us  max=635us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.259ms  avg=1.259ms  max=1.259ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.266ms  avg=1.269ms  max=1.272ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.847ms  avg=3.847ms  max=3.847ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.175ms  avg=3.175ms  max=3.175ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=36.211ms  avg=36.211ms  max=36.211ms  →

[Demo::main]int main(int, char**)
  count=1  min=36.703ms  avg=36.703ms  max=36.703ms  →
```

<!-- markdownlint-enable MD013 -->
