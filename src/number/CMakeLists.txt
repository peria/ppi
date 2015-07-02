cmake_minimum_required(VERSION 2.8)

add_library(ppi_number integer.cc)

# ----------------------------------------------------------------------

function(ppi_number_add_test target)
  add_executable(${target}_test ${target}_test.cc)
  target_link_libraries(${target}_test gtest gtest_main)
  target_link_libraries(${target}_test ppi_base)
  ppi_target_link_libraries(${target}_test)
endfunction()

ppi_number_add_test(integer)
