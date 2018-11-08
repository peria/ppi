cmake_minimum_required(VERSION 2.8)

add_library(ppi_number integer.cc real.cc montgomery.cc natural.cc)
ppi_target_link_libraries(ppi_number ppi_base ppi_fmt)

# ----------------------------------------------------------------------

function(ppi_number_test target)
  ppi_test(number_${target} ${ARGN})
  ppi_target_link_libraries(number_${target}_test ppi_number)
endfunction()

ppi_number_test(integer integer_test.cc)
ppi_number_test(natural natural_test.cc)
ppi_number_test(real real_test.cc)
