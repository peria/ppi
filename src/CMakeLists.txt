project(ppi)

# ----------------------------------------------------------------------
# cmake settings

cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# ----------------------------------------------------------------------
# Check packages, libraries, etc.

find_package(Threads REQUIRED)
find_package(PkgConfig)

set(CMAKE_SKIP_INSTALL_RULES ON)

set(GFLAGS_BUILD_gflags_LIB ON)
set(GFLAGS_BUILD_TESTING OFF)
add_subdirectory(third_party/gflags)
set(WITH_GFLAGS ON)
set(gflags_FOUND ON)
set(BUILD_TESTING OFF)
add_subdirectory(third_party/glog)

set(gtest_build_tests OFF)
add_subdirectory(third_party/googletest/googletest)

# ----------------------------------------------------------------------
# Set include directories, c++ options, etc.

include_directories(.)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

function(ppi_add_cxx_flags flags)
  set(CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()

ppi_add_cxx_flags("-std=c++17")
ppi_add_cxx_flags("-Wall -Wextra")
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  ppi_add_cxx_flags("-O0")
else()
  ppi_add_cxx_flags("-O2")
endif()
ppi_add_cxx_flags("-march=native")
ppi_add_cxx_flags("-DSRC_DIR=\\\"${CMAKE_SOURCE_DIR}\\\"")
ppi_add_cxx_flags("-DBUILD_TYPE_${CMAKE_BUILD_TYPE}=1")

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  ppi_add_cxx_flags("-Wno-error=deprecated-declarations")
  ppi_add_cxx_flags("-Wno-error=missing-field-initializers")
  ppi_add_cxx_flags("-Wno-error=sign-compare")
  ppi_add_cxx_flags("-DNOINLINE_UNLESS_RELEASE=")
  ppi_add_cxx_flags("-DNDEBUG=1")
endif()

# Set up coverage build type.
if (${CMAKE_BUILD_TYPE} MATCHES "Coverage")
  set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the C++ compiler during coverage builds."
    FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE)
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the shared libraries linker during coverage builds."
    FORCE)
  mark_as_advanced(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE)
  set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE)
  ppi_add_cxx_flags(--coverage COVERAGE)
endif()

# ----------------------------------------------------------------------
# Define CMake functions

function(ppi_target_link_libraries target)
  target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(${target} ${ARGN})
  target_link_libraries(${target} gflags)
  target_link_libraries(${target} glog)
endfunction()

function(ppi_test target)
  add_executable(${target}_test ${ARGN})
  ppi_target_link_libraries(${target}_test gtest gtest_main)
  add_test(${target}_test ${target}_test)
endfunction()

function(ppi_add_executable target)
  add_executable(${target} ${ARGN})
  ppi_target_link_libraries(${target})
  if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    # If you want to profile CPU usage or memory usage, set CPUPROFILE
    # or HEAPPROFILE environment to figure an output file.
    # e.g. $ CPUPROFILE=cpu.prof /path/to/execute_file
    #      $ HEAPPROFILE=heap.prof /path/to/execute_file
    # then you can see the result with pprof.
    # e.g. $ pprof --text /path/to/execute_file cpu.prof
    #      $ pprof --text /path/to/execute_file heap.prof.0001.heap
    target_link_libraries(${target} libprofiler.so)
    target_link_libraries(${target} libtcmalloc.so)
  endif()
endfunction()

# ----------------------------------------------------------------------
# Add subdirectories

enable_testing()

add_subdirectory(base)
add_subdirectory(fmt)
add_subdirectory(number)
add_subdirectory(pi)

# ----------------------------------------------------------------------
# Display settings
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})
