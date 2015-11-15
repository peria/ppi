cmake_minimum_required(VERSION 2.8)

add_library(ppi_pi pi.cc)
target_link_libraries(ppi_pi ppi_number)

# ----------------------------------------------------------------------

function(ppi_pi_test target)
  ppi_test(pi_${target} ${ARGN})
  target_link_libraries(pi_${target}_test ppi_pi)
endfunction()

# ppi_pi_test(pi pi_test.cc)
