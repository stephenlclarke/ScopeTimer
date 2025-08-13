cmake_minimum_required(VERSION 3.16)
project(ScopeTimer LANGUAGES CXX)

# C++ standard configuration
function(configure_cpp_standard)
    if(NOT DEFINED CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to use" FORCE)
    endif()

    set(ALLOWED_CXX_STANDARDS 17 20 23)
    list(FIND ALLOWED_CXX_STANDARDS "${CMAKE_CXX_STANDARD}" _index)
    if(_index EQUAL -1)
        message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}. Allowed values: 17, 20, 23.")
    endif()

    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endfunction()

configure_cpp_standard()

# Linux/macOS only - no Win32 branches or libs
if(WIN32)
  message(FATAL_ERROR "This project targets Linux/macOS only.")
endif()


add_library(scopetimer INTERFACE)
target_include_directories(scopetimer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(scopetimer INTERFACE -Wall -Wextra -Wpedantic)

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
  add_executable(ScopeTimerTest example/ScopeTimerTest.cpp)
  target_link_libraries(ScopeTimerTest PRIVATE scopetimer)
endif()
