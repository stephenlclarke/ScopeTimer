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
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-21 07:42:27.102 | end=2026-07-21 07:42:27.102 | elapsed=5.416us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-21 07:42:27.102 | end=2026-07-21 07:42:27.106 | elapsed=3.149ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.107 | end=2026-07-21 07:42:27.109 | elapsed=1.887ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.109 | end=2026-07-21 07:42:27.112 | elapsed=3.147ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.106 | end=2026-07-21 07:42:27.113 | elapsed=6.967ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.115 | end=2026-07-21 07:42:27.116 | elapsed=1.147ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.114 | end=2026-07-21 07:42:27.116 | elapsed=2.192ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.113 | end=2026-07-21 07:42:27.117 | elapsed=3.101ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.113 | end=2026-07-21 07:42:27.117 | elapsed=3.885ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-21 07:42:27.118 | end=2026-07-21 07:42:27.120 | elapsed=1.507ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.120 | end=2026-07-21 07:42:27.120 | elapsed=388.667us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.120 | end=2026-07-21 07:42:27.120 | elapsed=386.292us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.120 | end=2026-07-21 07:42:27.121 | elapsed=382.917us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.121 | end=2026-07-21 07:42:27.121 | elapsed=383.958us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.121 | end=2026-07-21 07:42:27.122 | elapsed=388.333us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.120 | end=2026-07-21 07:42:27.122 | elapsed=1.990ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-21 07:42:27.122 | end=2026-07-21 07:42:27.122 | elapsed=650.834us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-21 07:42:27.122 | end=2026-07-21 07:42:27.122 | elapsed=764.500us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.122 | end=2026-07-21 07:42:27.123 | elapsed=198.167us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.123 | end=2026-07-21 07:42:27.123 | elapsed=195.125us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.123 | end=2026-07-21 07:42:27.123 | elapsed=194.833us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.122 | end=2026-07-21 07:42:27.123 | elapsed=607.875us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.123 | end=2026-07-21 07:42:27.123 | elapsed=155.167us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.123 | end=2026-07-21 07:42:27.123 | elapsed=154.666us
```

## Testing with elapsed time formatted in seconds

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=SECONDS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-21 07:42:27.142 | end=2026-07-21 07:42:27.142 | elapsed=0.000s
[simpleWork] TID=001 | void simpleWork() | start=2026-07-21 07:42:27.143 | end=2026-07-21 07:42:27.145 | elapsed=0.002s
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.147 | end=2026-07-21 07:42:27.149 | elapsed=0.001s
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.149 | end=2026-07-21 07:42:27.152 | elapsed=0.003s
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.146 | end=2026-07-21 07:42:27.152 | elapsed=0.006s
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.155 | end=2026-07-21 07:42:27.156 | elapsed=0.001s
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.154 | end=2026-07-21 07:42:27.156 | elapsed=0.002s
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.153 | end=2026-07-21 07:42:27.156 | elapsed=0.003s
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.153 | end=2026-07-21 07:42:27.156 | elapsed=0.003s
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-21 07:42:27.158 | end=2026-07-21 07:42:27.159 | elapsed=0.001s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.159 | end=2026-07-21 07:42:27.160 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.160 | end=2026-07-21 07:42:27.160 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.160 | end=2026-07-21 07:42:27.161 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.161 | end=2026-07-21 07:42:27.161 | elapsed=0.000s
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.161 | end=2026-07-21 07:42:27.161 | elapsed=0.000s
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.159 | end=2026-07-21 07:42:27.161 | elapsed=0.002s
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-21 07:42:27.162 | end=2026-07-21 07:42:27.162 | elapsed=0.000s
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-21 07:42:27.161 | end=2026-07-21 07:42:27.162 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.162 | end=2026-07-21 07:42:27.162 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.162 | end=2026-07-21 07:42:27.163 | elapsed=0.000s
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.163 | end=2026-07-21 07:42:27.163 | elapsed=0.000s
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.162 | end=2026-07-21 07:42:27.163 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.163 | end=2026-07-21 07:42:27.163 | elapsed=0.000s
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.163 | end=2026-07-21 07:42:27.163 | elapsed=0.000s
```

## Testing with elapsed time formatted in millis

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MILLIS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-21 07:42:27.183 | end=2026-07-21 07:42:27.183 | elapsed=0.005ms
[simpleWork] TID=001 | void simpleWork() | start=2026-07-21 07:42:27.183 | end=2026-07-21 07:42:27.186 | elapsed=3.146ms
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.187 | end=2026-07-21 07:42:27.189 | elapsed=1.895ms
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.189 | end=2026-07-21 07:42:27.192 | elapsed=3.147ms
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.186 | end=2026-07-21 07:42:27.193 | elapsed=6.985ms
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.195 | end=2026-07-21 07:42:27.197 | elapsed=1.135ms
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.194 | end=2026-07-21 07:42:27.197 | elapsed=2.155ms
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.194 | end=2026-07-21 07:42:27.197 | elapsed=3.038ms
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.193 | end=2026-07-21 07:42:27.197 | elapsed=3.799ms
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-21 07:42:27.198 | end=2026-07-21 07:42:27.200 | elapsed=1.510ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.200 | end=2026-07-21 07:42:27.200 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.200 | end=2026-07-21 07:42:27.200 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.200 | end=2026-07-21 07:42:27.201 | elapsed=0.382ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.201 | end=2026-07-21 07:42:27.201 | elapsed=0.383ms
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.201 | end=2026-07-21 07:42:27.202 | elapsed=0.381ms
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.200 | end=2026-07-21 07:42:27.202 | elapsed=1.950ms
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-21 07:42:27.202 | end=2026-07-21 07:42:27.202 | elapsed=0.637ms
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-21 07:42:27.202 | end=2026-07-21 07:42:27.202 | elapsed=0.681ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.202 | end=2026-07-21 07:42:27.203 | elapsed=0.195ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.203 | end=2026-07-21 07:42:27.203 | elapsed=0.199ms
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.203 | end=2026-07-21 07:42:27.203 | elapsed=0.192ms
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.202 | end=2026-07-21 07:42:27.203 | elapsed=0.610ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.203 | end=2026-07-21 07:42:27.203 | elapsed=0.154ms
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.203 | end=2026-07-21 07:42:27.203 | elapsed=0.154ms
```

## Testing with elapsed time formatted in micros

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=MICROS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-21 07:42:27.223 | end=2026-07-21 07:42:27.223 | elapsed=4.750us
[simpleWork] TID=001 | void simpleWork() | start=2026-07-21 07:42:27.223 | end=2026-07-21 07:42:27.226 | elapsed=3130.917us
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.227 | end=2026-07-21 07:42:27.229 | elapsed=1899.959us
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.229 | end=2026-07-21 07:42:27.232 | elapsed=3146.750us
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.226 | end=2026-07-21 07:42:27.233 | elapsed=6991.666us
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.236 | end=2026-07-21 07:42:27.237 | elapsed=1154.666us
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.235 | end=2026-07-21 07:42:27.237 | elapsed=2199.791us
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.234 | end=2026-07-21 07:42:27.237 | elapsed=3104.417us
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.233 | end=2026-07-21 07:42:27.237 | elapsed=3880.916us
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-21 07:42:27.238 | end=2026-07-21 07:42:27.240 | elapsed=1527.750us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.240 | end=2026-07-21 07:42:27.240 | elapsed=382.708us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.240 | end=2026-07-21 07:42:27.241 | elapsed=387.292us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.241 | end=2026-07-21 07:42:27.241 | elapsed=380.125us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.241 | end=2026-07-21 07:42:27.241 | elapsed=394.417us
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.241 | end=2026-07-21 07:42:27.242 | elapsed=383.792us
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.240 | end=2026-07-21 07:42:27.242 | elapsed=1972.166us
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-21 07:42:27.242 | end=2026-07-21 07:42:27.242 | elapsed=650.875us
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-21 07:42:27.242 | end=2026-07-21 07:42:27.243 | elapsed=725.208us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.243 | elapsed=196.000us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.243 | elapsed=192.458us
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.243 | elapsed=198.917us
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.243 | elapsed=609.167us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.243 | elapsed=155.333us
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.243 | end=2026-07-21 07:42:27.244 | elapsed=154.792us
```

## Testing with elapsed time formatted in nanos

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; sed -n '1,24p' ./ScopeTimer.log
[Demo::parseOptions] TID=001 | DemoOptions parseOptions(int, char **) | start=2026-07-21 07:42:27.263 | end=2026-07-21 07:42:27.263 | elapsed=5125ns
[simpleWork] TID=001 | void simpleWork() | start=2026-07-21 07:42:27.263 | end=2026-07-21 07:42:27.266 | elapsed=3146542ns
[nestedScopes:inner 1] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.268 | end=2026-07-21 07:42:27.270 | elapsed=1896459ns
[nestedScopes:inner 2] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.270 | end=2026-07-21 07:42:27.273 | elapsed=3142500ns
[nestedScopes:outer] TID=001 | void nestedScopes() | start=2026-07-21 07:42:27.266 | end=2026-07-21 07:42:27.273 | elapsed=6989083ns
[multi:fourth] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.276 | end=2026-07-21 07:42:27.277 | elapsed=1149084ns
[multi:third] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.275 | end=2026-07-21 07:42:27.277 | elapsed=2187542ns
[multi:second] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.274 | end=2026-07-21 07:42:27.277 | elapsed=3095166ns
[multi:first] TID=001 | void multipleTimersSameScope() | start=2026-07-21 07:42:27.273 | end=2026-07-21 07:42:27.277 | elapsed=3879167ns
[conditionalWork] TID=001 | void conditionalWork(bool) | start=2026-07-21 07:42:27.279 | end=2026-07-21 07:42:27.280 | elapsed=1524292ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.280 | end=2026-07-21 07:42:27.281 | elapsed=386667ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.281 | end=2026-07-21 07:42:27.281 | elapsed=386834ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.281 | end=2026-07-21 07:42:27.282 | elapsed=380125ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.282 | end=2026-07-21 07:42:27.282 | elapsed=396625ns
[loopedWork:iteration] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.282 | end=2026-07-21 07:42:27.282 | elapsed=388792ns
[loopedWork:total] TID=001 | void loopedWork(int) | start=2026-07-21 07:42:27.280 | end=2026-07-21 07:42:27.282 | elapsed=1980250ns
[threadedWork:worker] TID=002 | auto threadedWork(int)::(anonymous class)::operator()() const | start=2026-07-21 07:42:27.282 | end=2026-07-21 07:42:27.283 | elapsed=650709ns
[threadedWork:total] TID=001 | void threadedWork(int) | start=2026-07-21 07:42:27.282 | end=2026-07-21 07:42:27.283 | elapsed=730125ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.283 | end=2026-07-21 07:42:27.283 | elapsed=200459ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.283 | end=2026-07-21 07:42:27.284 | elapsed=199834ns
[bufferedSinkExample:iteration] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.284 | end=2026-07-21 07:42:27.284 | elapsed=197959ns
[bufferedSinkExample] TID=001 | void bufferedSinkExample() | start=2026-07-21 07:42:27.283 | end=2026-07-21 07:42:27.284 | elapsed=618417ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.284 | end=2026-07-21 07:42:27.284 | elapsed=155541ns
[asyncSinkExample:iteration] TID=001 | void asyncSinkExample() | start=2026-07-21 07:42:27.284 | end=2026-07-21 07:42:27.284 | elapsed=156458ns
```

## Summarise the logging

```bash
> rm -f ./ScopeTimer.log; env SCOPE_TIMER_DIR=. SCOPE_TIMER_FORMAT=NANOS ./build-review/docs-refresh/Demo --iterations=1 >/dev/null 2>&1; scripts/process_scope_times.sh ./ScopeTimer.log | scripts/summarize_scope_times.sh
===== Summary (count / min / avg / max) =====
[Demo::parseOptions]DemoOptions parseOptions(int, char **)
  count=1  min=5us  avg=5us  max=5us  →

[simpleWork]void simpleWork()
  count=1  min=3.132ms  avg=3.132ms  max=3.132ms  →

[nestedScopes:inner 1]void nestedScopes()
  count=1  min=1.887ms  avg=1.887ms  max=1.887ms  →

[nestedScopes:inner 2]void nestedScopes()
  count=1  min=3.135ms  avg=3.135ms  max=3.135ms  →

[nestedScopes:outer]void nestedScopes()
  count=1  min=6.934ms  avg=6.934ms  max=6.934ms  →

[multi:fourth]void multipleTimersSameScope()
  count=1  min=1.131ms  avg=1.131ms  max=1.131ms  →

[multi:third]void multipleTimersSameScope()
  count=1  min=2.149ms  avg=2.149ms  max=2.149ms  →

[multi:second]void multipleTimersSameScope()
  count=1  min=3.034ms  avg=3.034ms  max=3.034ms  →

[multi:first]void multipleTimersSameScope()
  count=1  min=3.794ms  avg=3.794ms  max=3.794ms  →

[conditionalWork]void conditionalWork(bool)
  count=1  min=1.506ms  avg=1.506ms  max=1.506ms  →

[loopedWork:iteration]void loopedWork(int)
  count=5  min=380us  avg=381us  max=384us  →

[loopedWork:total]void loopedWork(int)
  count=1  min=1.938ms  avg=1.938ms  max=1.938ms  →

[threadedWork:worker]auto threadedWork(int)::(anonymous class)::operator()() const
  count=1  min=633us  avg=633us  max=633us  →

[threadedWork:total]void threadedWork(int)
  count=1  min=684us  avg=684us  max=684us  →

[bufferedSinkExample:iteration]void bufferedSinkExample()
  count=3  min=192us  avg=193us  max=195us  →

[bufferedSinkExample]void bufferedSinkExample()
  count=1  min=601us  avg=601us  max=601us  →

[asyncSinkExample:iteration]void asyncSinkExample()
  count=3  min=153us  avg=154us  max=154us  →

[asyncSinkExample]void asyncSinkExample()
  count=1  min=464us  avg=464us  max=464us  →

[hotPathMacroExample:record
  count=24  min=291ns  avg=365ns  max=584ns  →

[hotPathMacroExample:total]void hotPathMacroExample()
  count=1  min=106us  avg=106us  max=106us  →

[hotPath:processBatch]std::uint64_t processTelemetryBatchExample(const std::vector<TelemetryEvent> &, TelemetryTotals &)
  count=5  min=342us  avg=363us  max=373us  ↘

[hotPath:total]void hotPathIngestion(int)
  count=1  min=1.831ms  avg=1.831ms  max=1.831ms  →

[Worker:constructor]Worker::Worker()
  count=1  min=634us  avg=634us  max=634us  →

[Worker:singleTask]void Worker::doTask(const std::string_view) const
  count=1  min=1.255ms  avg=1.255ms  max=1.255ms  →

[Worker:task]void Worker::doTask(const std::string_view) const
  count=5  min=1.254ms  avg=1.257ms  max=1.260ms  →

[Worker:doMultipleTasks]void Worker::doMultipleTasks(int, bool) const
  count=1  min=3.792ms  avg=3.792ms  max=3.792ms  →

[ScopeTimer]LifetimeTracked
  count=1  min=3.143ms  avg=3.143ms  max=3.143ms  →

[Demo::runDemoSuite]void runDemoSuite(int)
  count=1  min=35.691ms  avg=35.691ms  max=35.691ms  →

[Demo::main]int main(int, char **)
  count=1  min=36.049ms  avg=36.049ms  max=36.049ms  →
```

<!-- markdownlint-enable MD013 -->
