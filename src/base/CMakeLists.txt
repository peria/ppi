cmake_minimum_required(VERSION 2.8)

add_library(ppi_base base.cc util.cc timer.cc allocator.cc)

# ----------------------------------------------------------------------

function(ppi_base_test target)
  ppi_test(base_${target} ${ARGN})
  # ppi_base is added in ppi_test.
endfunction()

