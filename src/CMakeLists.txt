project(ppi)

# ----------------------------------------------------------------------
# cmake settings

cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

enable_testing()

# ----------------------------------------------------------------------
# Check packages, libraries, etc.

find_package(Threads   REQUIRED)
#find_package(PkgConfig REQUIRED)
find_package(glog      REQUIRED)

find_library(LIB_GFLAGS gflags)

add_subdirectory(third_party/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(third_party/benchmark)
include_directories(third_party/benchmark/include third_party/benchmark/src)

# ----------------------------------------------------------------------
# Set include directories, c++ options, etc.

function(ppi_add_cxx_flags flags)
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
  else()
    set(CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endif()
endfunction()

ppi_add_cxx_flags("-std=c++11")
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
endif()

# Set up coverage build type.
if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
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
endif()

include_directories(.)

# ----------------------------------------------------------------------
# Define test dependencies
function(ppi_test target)
  add_executable(${target}_test ${ARGN})
  target_link_libraries(${target}_test gtest gtest_main)
  target_link_libraries(${target}_test ppi_base)
  ppi_target_link_libraries(${target}_test)
  add_test(${target}_test ${target}_test)
endfunction()

# ----------------------------------------------------------------------
# Add subdirectories

function(ppi_target_link_libraries target)
  target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(${target} ${LIB_GFLAGS})
  target_link_libraries(${target} glog::glog)
  target_link_libraries(${target} ${ARGN})
endfunction()

function(ppi_add_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} ppi_base)
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

function(ppi_add_benchmark target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} ppi_base)
  target_link_libraries(${target} benchmark)
  ppi_target_link_libraries(${target})
endfunction()

add_subdirectory(base)
add_subdirectory(fmt)
add_subdirectory(number)
add_subdirectory(pi)

# ----------------------------------------------------------------------
# Display settings
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})
