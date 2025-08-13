# ScopeTimer

```bash
❯ rm -rf build; mkdir -p build; cd build; cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Debug ..; make -j ; cd ..
-- The CXX compiler identification is AppleClang 17.0.0.17000013
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- The C compiler identification is AppleClang 17.0.0.17000013
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Performing Test HAVE_CXX_ATOMICS_WITHOUT_LIB
-- Performing Test HAVE_CXX_ATOMICS_WITHOUT_LIB - Success
-- Could NOT find benchmark (missing: benchmark_DIR)
-- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY
-- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY - Success
-- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY
-- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY - Success
-- Performing Test COMPILER_HAS_DEPRECATED_ATTR
-- Performing Test COMPILER_HAS_DEPRECATED_ATTR - Success
-- Found ZLIB: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libz.tbd (found version "1.2.12")
-- Found CURL: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libcurl.tbd (found version "8.7.1")
-- The following features have been enabled:

 * Pull, support for pulling metrics
 * Push, support for pushing metrics to a push-gateway
 * Compression, support for zlib compression of metrics
 * pkg-config, generate pkg-config files

-- The following REQUIRED packages have been found:

 * googlemock-3rdparty
 * civetweb-3rdparty
 * ZLIB
 * CURL

-- The following features have been disabled:

 * IYWU, include-what-you-use

-- The following OPTIONAL packages have not been found:

 * benchmark

-- Configuring done (15.6s)
-- Generating done (0.1s)
-- Build files have been written to: /Users/sclarke/github/ScopeTimer/build
[  1%] Building C object _deps/prometheus-cpp-build/pull/CMakeFiles/civetweb.dir/__/3rdparty/civetweb/src/civetweb.c.o
[  3%] Building CXX object _deps/prometheus-cpp-build/CMakeFiles/gmock_main.dir/3rdparty/googletest/googletest/src/gtest-all.cc.o
[  5%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/gauge.cc.o
[  8%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/counter.cc.o
[  8%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/detail/ckms_quantiles.cc.o
[ 10%] Building CXX object _deps/prometheus-cpp-build/CMakeFiles/gmock_main.dir/3rdparty/googletest/googlemock/src/gmock_main.cc.o
[ 13%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/civetweb.dir/__/3rdparty/civetweb/src/CivetServer.cpp.o
[ 13%] Building CXX object _deps/prometheus-cpp-build/CMakeFiles/gmock_main.dir/3rdparty/googletest/googlemock/src/gmock-all.cc.o
[ 15%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/text_serializer.cc.o
[ 18%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/serializer.cc.o
[ 18%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/detail/builder.cc.o
[ 20%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/detail/time_window_quantiles.cc.o
[ 22%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/registry.cc.o
[ 23%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/family.cc.o
[ 25%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/detail/utils.cc.o
[ 27%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/check_names.cc.o
[ 28%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/info.cc.o
[ 30%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/histogram.cc.o
[ 32%] Building CXX object _deps/prometheus-cpp-build/core/CMakeFiles/core.dir/src/summary.cc.o
[ 32%] Built target civetweb
[ 33%] Linking CXX static library ../lib/libprometheus-cpp-core.a
[ 33%] Built target core
[ 42%] Building CXX object _deps/prometheus-cpp-build/push/CMakeFiles/push.dir/src/gateway.cc.o
[ 42%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/pull.dir/src/handler.cc.o
[ 42%] Building CXX object CMakeFiles/ScopeTimerTest.dir/example/ScopeTimerTest.cpp.o
[ 42%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/pull.dir/src/basic_auth.cc.o
[ 42%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/pull.dir/src/metrics_collector.cc.o
[ 44%] Building CXX object _deps/prometheus-cpp-build/push/CMakeFiles/push.dir/src/curl_wrapper.cc.o
[ 45%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/pull.dir/src/exposer.cc.o
[ 47%] Building CXX object _deps/prometheus-cpp-build/pull/CMakeFiles/pull.dir/src/endpoint.cc.o
[ 49%] Linking CXX executable ScopeTimerTest
[ 50%] Linking CXX static library lib/libgmock_main.a
[ 50%] Built target ScopeTimerTest
[ 50%] Built target gmock_main
[ 52%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/builder_test.cc.o
[ 54%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/check_metric_name_test.cc.o
[ 55%] Building CXX object _deps/prometheus-cpp-build/pull/tests/internal/CMakeFiles/prometheus_pull_internal_test.dir/base64_test.cc.o
[ 57%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/text_serializer_test.cc.o
[ 59%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/histogram_test.cc.o
[ 61%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/gauge_test.cc.o
[ 64%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/counter_test.cc.o
[ 64%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/summary_test.cc.o
[ 66%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/utils_test.cc.o
[ 71%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/registry_test.cc.o
[ 71%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/serializer_test.cc.o
[ 71%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/check_label_name_test.cc.o
[ 72%] Building CXX object _deps/prometheus-cpp-build/core/tests/CMakeFiles/prometheus_core_test.dir/family_test.cc.o
[ 74%] Linking CXX static library ../lib/libprometheus-cpp-pull.a
[ 76%] Linking CXX static library ../lib/libprometheus-cpp-push.a
[ 76%] Built target pull
[ 77%] Building CXX object _deps/prometheus-cpp-build/pull/tests/integration/CMakeFiles/sample_server_auth.dir/sample_server_auth.cc.o
[ 79%] Building CXX object _deps/prometheus-cpp-build/pull/tests/integration/CMakeFiles/prometheus_pull_integration_test.dir/integration_test.cc.o
[ 79%] Built target push
[ 81%] Building CXX object _deps/prometheus-cpp-build/pull/tests/integration/CMakeFiles/sample_server_multi.dir/sample_server_multi.cc.o
[ 83%] Building CXX object _deps/prometheus-cpp-build/pull/tests/unit/CMakeFiles/prometheus_pull_test.dir/exposer_test.cc.o
[ 84%] Building CXX object _deps/prometheus-cpp-build/pull/tests/integration/CMakeFiles/sample_server.dir/sample_server.cc.o
[ 86%] Building CXX object _deps/prometheus-cpp-build/push/tests/integration/CMakeFiles/sample_client.dir/sample_client.cc.o
[ 88%] Linking CXX executable ../../../bin/prometheus_pull_internal_test
[ 88%] Built target prometheus_pull_internal_test
[ 89%] Linking CXX executable ../../../bin/sample_server_multi
[ 91%] Linking CXX executable ../../../bin/sample_server_auth
[ 93%] Linking CXX executable ../../../bin/sample_server
[ 93%] Built target sample_server_multi
[ 94%] Linking CXX executable ../../../bin/sample_client
[ 96%] Linking CXX executable ../../../bin/prometheus_pull_test
[ 96%] Built target sample_server
[ 96%] Built target sample_server_auth
[ 96%] Built target sample_client
[ 96%] Built target prometheus_pull_test
[ 98%] Linking CXX executable ../../bin/prometheus_core_test
[100%] Linking CXX executable ../../../bin/prometheus_pull_integration_test
[100%] Built target prometheus_core_test
[100%] Built target prometheus_pull_integration_test
```
