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

find_package(Threads)
find_package(PkgConfig)

find_library(LIB_GFLAGS gflags)
pkg_search_module(GLOG REQUIRED libglog)

# ----------------------------------------------------------------------
# Set include directories, c++ options, etc.

string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)

function(ppi_add_cxx_flags flags)
  if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
  else()
    set(CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endif()
endfunction()

if(CYGWIN AND (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU"))
  ppi_add_cxx_flags("-std=gnu++11")
else()
  ppi_add_cxx_flags("-std=c++11")
endif()
ppi_add_cxx_flags("-Wall -Wextra")
ppi_add_cxx_flags("-O2")
ppi_add_cxx_flags("-march=native")
ppi_add_cxx_flags("-DSRC_DIR=\\\"${CMAKE_SOURCE_DIR}\\\"")
ppi_add_cxx_flags("-DBUILD_TYPE_${build_type}=1")

if(${build_type} MATCHES release)
  ppi_add_cxx_flags("-Wno-error=deprecated-declarations")
  ppi_add_cxx_flags("-Wno-error=missing-field-initializers")
  ppi_add_cxx_flags("-Wno-error=sign-compare")
  ppi_add_cxx_flags("-DNOINLINE_UNLESS_RELEASE=")
endif()

include_directories(.)

add_subdirectory(third_party/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${GLOG_INCLUDE_DIRS})

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
  target_link_libraries(${target} ${GLOG_LIBRARIES})
  target_link_libraries(${target} ${ARGN})
endfunction()

function(ppi_add_executable target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} ppi_base)
  ppi_target_link_libraries(${target})
  if(${build_type} MATCHES "relwithdebinfo")
    # if you want to profile CPU usage or memory usage, set CPUPROFILE
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

add_subdirectory(base)
add_subdirectory(fmt)
add_subdirectory(number)
add_subdirectory(pi)

# ----------------------------------------------------------------------
# Display settings
message(STATUS "Build type:" ${build_type})
