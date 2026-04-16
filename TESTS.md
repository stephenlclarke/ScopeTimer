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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:50:23.788 | end=2026-04-16 11:50:23.788 | elapsed=3.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:50:23.789 | end=2026-04-16 11:50:23.792 | elapsed=3.132ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.793 | end=2026-04-16 11:50:23.795 | elapsed=1.881ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.795 | end=2026-04-16 11:50:23.798 | elapsed=3.133ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.792 | end=2026-04-16 11:50:23.799 | elapsed=6.920ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.802 | end=2026-04-16 11:50:23.803 | elapsed=1.131ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.801 | end=2026-04-16 11:50:23.803 | elapsed=2.144ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.800 | end=2026-04-16 11:50:23.803 | elapsed=3.028ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.799 | end=2026-04-16 11:50:23.803 | elapsed=3.786ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:50:23.804 | end=2026-04-16 11:50:23.806 | elapsed=1.503ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.806 | end=2026-04-16 11:50:23.806 | elapsed=385.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.806 | end=2026-04-16 11:50:23.807 | elapsed=382.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.807 | end=2026-04-16 11:50:23.807 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.807 | end=2026-04-16 11:50:23.807 | elapsed=381.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.807 | end=2026-04-16 11:50:23.808 | elapsed=382.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.806 | end=2026-04-16 11:50:23.808 | elapsed=1.943ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:50:23.808 | end=2026-04-16 11:50:23.808 | elapsed=632.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:50:23.808 | end=2026-04-16 11:50:23.808 | elapsed=678.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.809 | end=2026-04-16 11:50:23.809 | elapsed=194.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.809 | end=2026-04-16 11:50:23.809 | elapsed=183.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.809 | end=2026-04-16 11:50:23.809 | elapsed=191.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.808 | end=2026-04-16 11:50:23.809 | elapsed=719.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.809 | end=2026-04-16 11:50:23.809 | elapsed=155.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.809 | end=2026-04-16 11:50:23.810 | elapsed=154.000us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:50:23.838 | end=2026-04-16 11:50:23.838 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:50:23.838 | end=2026-04-16 11:50:23.841 | elapsed=0.002s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.842 | end=2026-04-16 11:50:23.844 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.844 | end=2026-04-16 11:50:23.847 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.841 | end=2026-04-16 11:50:23.848 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.850 | end=2026-04-16 11:50:23.851 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.849 | end=2026-04-16 11:50:23.851 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.848 | end=2026-04-16 11:50:23.851 | elapsed=0.002s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.848 | end=2026-04-16 11:50:23.851 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:50:23.853 | end=2026-04-16 11:50:23.854 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.854 | end=2026-04-16 11:50:23.855 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.855 | end=2026-04-16 11:50:23.855 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.855 | end=2026-04-16 11:50:23.856 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.856 | end=2026-04-16 11:50:23.856 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.856 | end=2026-04-16 11:50:23.856 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.854 | end=2026-04-16 11:50:23.856 | elapsed=0.001s
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:50:23.856 | end=2026-04-16 11:50:23.857 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:50:23.856 | end=2026-04-16 11:50:23.857 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.857 | end=2026-04-16 11:50:23.857 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.857 | end=2026-04-16 11:50:23.857 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.857 | end=2026-04-16 11:50:23.858 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.857 | end=2026-04-16 11:50:23.858 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.858 | end=2026-04-16 11:50:23.858 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.858 | end=2026-04-16 11:50:23.858 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:50:23.885 | end=2026-04-16 11:50:23.885 | elapsed=0.002ms
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:50:23.885 | end=2026-04-16 11:50:23.888 | elapsed=3.131ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.890 | end=2026-04-16 11:50:23.892 | elapsed=1.885ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.892 | end=2026-04-16 11:50:23.895 | elapsed=3.132ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.888 | end=2026-04-16 11:50:23.895 | elapsed=6.928ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.898 | end=2026-04-16 11:50:23.899 | elapsed=1.131ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.897 | end=2026-04-16 11:50:23.899 | elapsed=2.141ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.896 | end=2026-04-16 11:50:23.899 | elapsed=3.029ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.895 | end=2026-04-16 11:50:23.899 | elapsed=3.795ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:50:23.901 | end=2026-04-16 11:50:23.902 | elapsed=1.508ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.902 | end=2026-04-16 11:50:23.903 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.903 | end=2026-04-16 11:50:23.903 | elapsed=0.381ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.903 | end=2026-04-16 11:50:23.903 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.903 | end=2026-04-16 11:50:23.904 | elapsed=0.380ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.904 | end=2026-04-16 11:50:23.904 | elapsed=0.379ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.902 | end=2026-04-16 11:50:23.904 | elapsed=1.925ms
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:50:23.904 | end=2026-04-16 11:50:23.905 | elapsed=0.634ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:50:23.904 | end=2026-04-16 11:50:23.905 | elapsed=0.684ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.905 | end=2026-04-16 11:50:23.905 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.905 | end=2026-04-16 11:50:23.905 | elapsed=0.192ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.905 | end=2026-04-16 11:50:23.905 | elapsed=0.192ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.905 | end=2026-04-16 11:50:23.905 | elapsed=0.704ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.905 | end=2026-04-16 11:50:23.906 | elapsed=0.153ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.906 | end=2026-04-16 11:50:23.906 | elapsed=0.154ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:50:23.934 | end=2026-04-16 11:50:23.934 | elapsed=2.000us
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:50:23.934 | end=2026-04-16 11:50:23.937 | elapsed=3136.000us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.938 | end=2026-04-16 11:50:23.940 | elapsed=1880.000us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.940 | end=2026-04-16 11:50:23.943 | elapsed=3130.000us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.937 | end=2026-04-16 11:50:23.944 | elapsed=6910.000us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.946 | end=2026-04-16 11:50:23.947 | elapsed=1055.000us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.945 | end=2026-04-16 11:50:23.947 | elapsed=2067.000us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.944 | end=2026-04-16 11:50:23.947 | elapsed=2950.000us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.944 | end=2026-04-16 11:50:23.947 | elapsed=3710.000us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:50:23.949 | end=2026-04-16 11:50:23.950 | elapsed=1505.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.950 | end=2026-04-16 11:50:23.951 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.951 | end=2026-04-16 11:50:23.951 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.951 | end=2026-04-16 11:50:23.952 | elapsed=379.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.952 | end=2026-04-16 11:50:23.952 | elapsed=380.000us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.952 | end=2026-04-16 11:50:23.952 | elapsed=380.000us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.950 | end=2026-04-16 11:50:23.952 | elapsed=1919.000us
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:50:23.952 | end=2026-04-16 11:50:23.953 | elapsed=635.000us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:50:23.952 | end=2026-04-16 11:50:23.953 | elapsed=683.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.953 | end=2026-04-16 11:50:23.953 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.953 | end=2026-04-16 11:50:23.953 | elapsed=192.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.953 | end=2026-04-16 11:50:23.954 | elapsed=191.000us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:23.953 | end=2026-04-16 11:50:23.954 | elapsed=720.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.954 | end=2026-04-16 11:50:23.954 | elapsed=153.000us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:23.954 | end=2026-04-16 11:50:23.954 | elapsed=155.000us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char**) | start=2026-04-16 11:50:23.980 | end=2026-04-16 11:50:23.980 | elapsed=1000ns
[simpleWork] TID=001 | void simpleWork() | start=2026-04-16 11:50:23.981 | end=2026-04-16 11:50:23.984 | elapsed=3132000ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.985 | end=2026-04-16 11:50:23.987 | elapsed=1889000ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.987 | end=2026-04-16 11:50:23.990 | elapsed=3142000ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-04-16 11:50:23.984 | end=2026-04-16 11:50:23.991 | elapsed=6909000ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.993 | end=2026-04-16 11:50:23.994 | elapsed=1130000ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.992 | end=2026-04-16 11:50:23.994 | elapsed=2138000ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.991 | end=2026-04-16 11:50:23.994 | elapsed=3000000ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-04-16 11:50:23.991 | end=2026-04-16 11:50:23.994 | elapsed=3759000ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-04-16 11:50:23.996 | end=2026-04-16 11:50:23.997 | elapsed=1489000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.997 | end=2026-04-16 11:50:23.998 | elapsed=381000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.998 | end=2026-04-16 11:50:23.998 | elapsed=378000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.998 | end=2026-04-16 11:50:23.999 | elapsed=379000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.999 | end=2026-04-16 11:50:23.999 | elapsed=378000ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.999 | end=2026-04-16 11:50:23.999 | elapsed=379000ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-04-16 11:50:23.997 | end=2026-04-16 11:50:23.999 | elapsed=1916000ns
[threadedWork:worker] TID=002 | threadedWork(int)::<lambda()> | start=2026-04-16 11:50:23.999 | end=2026-04-16 11:50:24.000 | elapsed=633000ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-04-16 11:50:23.999 | end=2026-04-16 11:50:24.000 | elapsed=679000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:24.000 | end=2026-04-16 11:50:24.000 | elapsed=191000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:24.000 | end=2026-04-16 11:50:24.000 | elapsed=191000ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:24.000 | end=2026-04-16 11:50:24.001 | elapsed=191000ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-04-16 11:50:24.000 | end=2026-04-16 11:50:24.001 | elapsed=695000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:24.001 | end=2026-04-16 11:50:24.001 | elapsed=155000ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-04-16 11:50:24.001 | end=2026-04-16 11:50:24.001 | elapsed=153000ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-docs/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char**)
  count=1  min=1us  avg=1us  max=1us  →

[simpleWork]void simpleWork()
  count=1  min=3.134ms  avg=3.134ms  max=3.134ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.881ms  avg=1.881ms  max=1.881ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.130ms  avg=3.130ms  max=3.130ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.905ms  avg=6.905ms  max=6.905ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.135ms  avg=1.135ms  max=1.135ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.169ms  avg=2.169ms  max=2.169ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.052ms  avg=3.052ms  max=3.052ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.810ms  avg=3.810ms  max=3.810ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.506ms  avg=1.506ms  max=1.506ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=379us  avg=380us  max=382us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.925ms  avg=1.925ms  max=1.925ms  →

[threadedWork:worker]threadedWork(int)::<lambda()>
  count=1  min=637us  avg=637us  max=637us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=685us  avg=685us  max=685us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=192us  avg=194us  max=196us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=710us  avg=710us  max=710us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=153us  avg=154us  max=156us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=577us  avg=577us  max=577us  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=51us  avg=51us  max=51us  →

[hotPath:processBatch]uint64_t processTelemetryBatchExample(const std::vector<xyzzy::scopetimer::example_workload::TelemetryEvent>&, TelemetryTotals&)
  count=5  min=303us  avg=306us  max=310us  →

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.537ms  avg=1.537ms  max=1.537ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=629us  avg=629us  max=629us  →

[Worker:singleTask]void Worker::doTask(std::string_view) const
  count=1  min=1.256ms  avg=1.256ms  max=1.256ms  →

[Worker:task]void Worker::doTask(std::string_view) const
  count=5  min=1.254ms  avg=1.256ms  max=1.258ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.774ms  avg=3.774ms  max=3.774ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.144ms  avg=3.144ms  max=3.144ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=34.088ms  avg=34.088ms  max=34.088ms  →

[Demo::main]int main(int, char**)
  count=1  min=34.540ms  avg=34.540ms  max=34.540ms  →
```

<!-- markdownlint-enable MD013 -->
