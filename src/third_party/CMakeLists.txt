set(CMAKE_SKIP_INSTALL_RULES ON)

set(gflags_BUILD_gflags_LIB ON)
set(gflags_BUILD_TESTING OFF)
add_subdirectory(gflags)

set(gflags_DIR ${CMAKE_BUILD_DIR}/third_party/gflags)
set(WITH_GFLAGS OFF)
set(gflags_FOUND ON)
set(gflags_BUILD_TESTING OFF)
add_subdirectory(glog)

set(gtest_build_tests OFF)
add_subdirectory(googletest/googletest)
