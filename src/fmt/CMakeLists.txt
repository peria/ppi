cmake_minimum_required(VERSION 2.8)

add_library(ppi_fmt fmt.cc fft.cc)

# ----------------------------------------------------------------------

# TODO: Refactor out test specific dependencies to the root directory.
function(ppi_fmt_test target)
  add_executable(fmt_${target}_test ${target}_test.cc)
  target_link_libraries(fmt_${target}_test gtest gtest_main)
  target_link_libraries(fmt_${target}_test ppi_base)
  target_link_libraries(fmt_${target}_test ppi_fmt)
  ppi_target_link_libraries(fmt_${target}_test)
  add_test(fmt_${target}_test fmt_${target}_test)
endfunction()

ppi_fmt_test(fmt)
ppi_fmt_test(fft)
