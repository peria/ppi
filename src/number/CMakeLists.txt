cmake_minimum_required(VERSION 2.8)

add_library(ppi_number integer.cc integer_piece.cc real.cc montgomery.cc)
ppi_target_link_libraries(ppi_number ppi_fmt)

# ----------------------------------------------------------------------

function(ppi_number_test target)
  ppi_test(number_${target} ${ARGN})
  target_link_libraries(number_${target}_test ppi_number)
endfunction()

ppi_number_test(integer_piece integer_piece_test.cc)
ppi_number_test(integer integer_test.cc)
ppi_number_test(real real_test.cc)
