cmake_minimum_required(VERSION 2.8)

add_library(ppi_pi arctan.cc drm.cc)
target_link_libraries(ppi_pi ppi_number)

# ----------------------------------------------------------------------

function(ppi_pi_test target)
  ppi_test(pi_${target} ${ARGN})
  target_link_libraries(pi_${target}_test ppi_pi)
endfunction()

function(ppi_pi_add_executable target)
  ppi_add_executable(${target} ${ARGN})
  target_link_libraries(${target} ppi_pi)
endfunction()

function(ppi_pi_add_benchmark target)
  if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    ppi_add_benchmark(${target} ${ARGN})
    target_link_libraries(${target} ppi_pi)
  endif()
endfunction()

ppi_pi_add_executable(pi_main pi_main.cc)
ppi_pi_add_benchmark(pi_benchmark pi_benchmark.cc)

