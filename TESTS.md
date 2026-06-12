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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-06-12 16:03:51.842 | end=2026-06-12 16:03:51.842 | elapsed=4.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-06-12 16:03:51.842 | end=2026-06-12 16:03:51.846 | elapsed=3.133ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.847 | end=2026-06-12 16:03:51.849 | elapsed=1.882ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.849 | end=2026-06-12 16:03:51.852 | elapsed=3.132ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.846 | end=2026-06-12 16:03:51.853 | elapsed=6.913ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.855 | end=2026-06-12 16:03:51.856 | elapsed=1.134ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.854 | end=2026-06-12 16:03:51.856 | elapsed=2.148ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.853 | end=2026-06-12 16:03:51.856 | elapsed=3.033ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.853 | end=2026-06-12 16:03:51.856 | elapsed=3.791ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-06-12 16:03:51.858 | end=2026-06-12 16:03:51.859 | elapsed=1.508ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.859 | end=2026-06-12 16:03:51.860 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.860 | end=2026-06-12 16:03:51.860 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.860 | end=2026-06-12 16:03:51.860 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.860 | end=2026-06-12 16:03:51.861 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.861 | end=2026-06-12 16:03:51.861 | elapsed=379.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.859 | end=2026-06-12 16:03:51.861 | elapsed=1.923ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-06-12 16:03:51.861 | end=2026-06-12 16:03:51.862 | elapsed=632.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-06-12 16:03:51.861 | end=2026-06-12 16:03:51.862 | elapsed=685.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.862 | end=2026-06-12 16:03:51.862 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.862 | end=2026-06-12 16:03:51.862 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.862 | end=2026-06-12 16:03:51.863 | elapsed=196.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.862 | end=2026-06-12 16:03:51.863 | elapsed=856.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.863 | end=2026-06-12 16:03:51.863 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.863 | end=2026-06-12 16:03:51.863 | elapsed=161.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-06-12 16:03:51.909 | end=2026-06-12 16:03:51.909 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-06-12 16:03:51.909 | end=2026-06-12 16:03:51.912 | elapsed=0.003s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.913 | end=2026-06-12 16:03:51.915 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.915 | end=2026-06-12 16:03:51.918 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.912 | end=2026-06-12 16:03:51.919 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.922 | end=2026-06-12 16:03:51.923 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.921 | end=2026-06-12 16:03:51.923 | elapsed=0.001s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.920 | end=2026-06-12 16:03:51.923 | elapsed=0.002s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.919 | end=2026-06-12 16:03:51.923 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-06-12 16:03:51.924 | end=2026-06-12 16:03:51.926 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.926 | end=2026-06-12 16:03:51.926 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.926 | end=2026-06-12 16:03:51.926 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.926 | end=2026-06-12 16:03:51.927 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.927 | end=2026-06-12 16:03:51.927 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.927 | end=2026-06-12 16:03:51.928 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.926 | end=2026-06-12 16:03:51.928 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-06-12 16:03:51.928 | end=2026-06-12 16:03:51.928 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-06-12 16:03:51.928 | end=2026-06-12 16:03:51.928 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.928 | end=2026-06-12 16:03:51.929 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.929 | end=2026-06-12 16:03:51.929 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.929 | end=2026-06-12 16:03:51.929 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.928 | end=2026-06-12 16:03:51.929 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.929 | end=2026-06-12 16:03:51.930 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.930 | end=2026-06-12 16:03:51.930 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-06-12 16:03:51.978 | end=2026-06-12 16:03:51.978 | elapsed=0.003ms
[simpleWork] TID=001 | void simpleWork() | start=2026-06-12 16:03:51.978 | end=2026-06-12 16:03:51.981 | elapsed=3.136ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.982 | end=2026-06-12 16:03:51.984 | elapsed=1.881ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.984 | end=2026-06-12 16:03:51.987 | elapsed=3.131ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-06-12 16:03:51.981 | end=2026-06-12 16:03:51.988 | elapsed=6.914ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.991 | end=2026-06-12 16:03:51.992 | elapsed=1.136ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.990 | end=2026-06-12 16:03:51.992 | elapsed=2.170ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.989 | end=2026-06-12 16:03:51.992 | elapsed=3.074ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:51.988 | end=2026-06-12 16:03:51.992 | elapsed=3.839ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-06-12 16:03:51.993 | end=2026-06-12 16:03:51.995 | elapsed=1.512ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.995 | end=2026-06-12 16:03:51.995 | elapsed=0.385ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.995 | end=2026-06-12 16:03:51.996 | elapsed=0.384ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.996 | end=2026-06-12 16:03:51.996 | elapsed=0.384ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.996 | end=2026-06-12 16:03:51.997 | elapsed=0.385ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.997 | end=2026-06-12 16:03:51.997 | elapsed=0.385ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:51.995 | end=2026-06-12 16:03:51.997 | elapsed=1.959ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-06-12 16:03:51.997 | end=2026-06-12 16:03:51.998 | elapsed=0.635ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-06-12 16:03:51.997 | end=2026-06-12 16:03:51.998 | elapsed=0.690ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.998 | end=2026-06-12 16:03:51.998 | elapsed=0.193ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.998 | end=2026-06-12 16:03:51.998 | elapsed=0.192ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.998 | end=2026-06-12 16:03:51.998 | elapsed=0.193ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:51.998 | end=2026-06-12 16:03:51.999 | elapsed=0.938ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.999 | end=2026-06-12 16:03:51.999 | elapsed=0.135ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:51.999 | end=2026-06-12 16:03:51.999 | elapsed=0.154ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-06-12 16:03:52.048 | end=2026-06-12 16:03:52.048 | elapsed=7.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-06-12 16:03:52.048 | end=2026-06-12 16:03:52.051 | elapsed=3135.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.052 | end=2026-06-12 16:03:52.054 | elapsed=1881.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.054 | end=2026-06-12 16:03:52.057 | elapsed=2666.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.051 | end=2026-06-12 16:03:52.058 | elapsed=6461.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.060 | end=2026-06-12 16:03:52.061 | elapsed=1131.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.059 | end=2026-06-12 16:03:52.061 | elapsed=2153.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.058 | end=2026-06-12 16:03:52.061 | elapsed=3041.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.058 | end=2026-06-12 16:03:52.061 | elapsed=3800.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-06-12 16:03:52.063 | end=2026-06-12 16:03:52.064 | elapsed=1508.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.064 | end=2026-06-12 16:03:52.065 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.065 | end=2026-06-12 16:03:52.065 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.065 | end=2026-06-12 16:03:52.066 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.066 | end=2026-06-12 16:03:52.066 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.066 | end=2026-06-12 16:03:52.066 | elapsed=382.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.064 | end=2026-06-12 16:03:52.066 | elapsed=1947.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-06-12 16:03:52.066 | end=2026-06-12 16:03:52.067 | elapsed=636.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-06-12 16:03:52.066 | end=2026-06-12 16:03:52.067 | elapsed=694.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.067 | end=2026-06-12 16:03:52.067 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.067 | end=2026-06-12 16:03:52.068 | elapsed=193.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.068 | end=2026-06-12 16:03:52.068 | elapsed=194.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.067 | end=2026-06-12 16:03:52.068 | elapsed=942.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:52.068 | end=2026-06-12 16:03:52.068 | elapsed=154.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:52.068 | end=2026-06-12 16:03:52.068 | elapsed=154.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-06-12 16:03:52.117 | end=2026-06-12 16:03:52.117 | elapsed=3000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-06-12 16:03:52.117 | end=2026-06-12 16:03:52.120 | elapsed=3136000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.121 | end=2026-06-12 16:03:52.123 | elapsed=1886000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.123 | end=2026-06-12 16:03:52.126 | elapsed=3132000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-06-12 16:03:52.120 | end=2026-06-12 16:03:52.127 | elapsed=6928000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.130 | end=2026-06-12 16:03:52.131 | elapsed=1132000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.129 | end=2026-06-12 16:03:52.131 | elapsed=2146000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.128 | end=2026-06-12 16:03:52.131 | elapsed=3032000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-06-12 16:03:52.127 | end=2026-06-12 16:03:52.131 | elapsed=3792000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-06-12 16:03:52.132 | end=2026-06-12 16:03:52.134 | elapsed=1507000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.134 | end=2026-06-12 16:03:52.134 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.134 | end=2026-06-12 16:03:52.135 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.135 | end=2026-06-12 16:03:52.135 | elapsed=382000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.135 | end=2026-06-12 16:03:52.135 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.135 | end=2026-06-12 16:03:52.136 | elapsed=384000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-06-12 16:03:52.134 | end=2026-06-12 16:03:52.136 | elapsed=1944000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-06-12 16:03:52.136 | end=2026-06-12 16:03:52.136 | elapsed=638000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-06-12 16:03:52.136 | end=2026-06-12 16:03:52.136 | elapsed=713000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.137 | end=2026-06-12 16:03:52.137 | elapsed=193000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.137 | end=2026-06-12 16:03:52.137 | elapsed=191000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.137 | end=2026-06-12 16:03:52.137 | elapsed=190000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-06-12 16:03:52.137 | end=2026-06-12 16:03:52.137 | elapsed=891000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:52.138 | end=2026-06-12 16:03:52.138 | elapsed=153000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-06-12 16:03:52.138 | end=2026-06-12 16:03:52.138 | elapsed=158000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=4us  avg=4us  max=4us  →

[simpleWork]void simpleWork()
  count=1  min=3.141ms  avg=3.141ms  max=3.141ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.887ms  avg=1.887ms  max=1.887ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.130ms  avg=3.130ms  max=3.130ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.930ms  avg=6.930ms  max=6.930ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.140ms  avg=1.140ms  max=1.140ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.178ms  avg=2.178ms  max=2.178ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.088ms  avg=3.088ms  max=3.088ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.864ms  avg=3.864ms  max=3.864ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.508ms  avg=1.508ms  max=1.508ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=352us  avg=377us  max=386us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.922ms  avg=1.922ms  max=1.922ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=649us  avg=649us  max=649us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=745us  avg=745us  max=745us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=191us  avg=194us  max=197us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=1.045ms  avg=1.045ms  max=1.045ms  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=158us  avg=159us  max=160us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=779us  avg=779us  max=779us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=253us  avg=253us  max=253us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=544us  avg=555us  max=580us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=2.805ms  avg=2.805ms  max=2.805ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=631us  avg=631us  max=631us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.255ms  avg=1.255ms  max=1.255ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.256ms  avg=1.257ms  max=1.257ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.791ms  avg=3.791ms  max=3.791ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=2.991ms  avg=2.991ms  max=2.991ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=37.819ms  avg=37.819ms  max=37.819ms  →

[Demo::main]int main(int, char**)
  count=1  min=39.409ms  avg=39.409ms  max=39.409ms  →
```

<!-- markdownlint-enable MD013 -->
