cmake_minimum_required(VERSION 2.8)

add_library(ppi_fmt dwt.cc fft.cc fmt.cc ntt.cc rft.cc)
ppi_target_link_libraries(ppi_fmt ppi_base ppi_number)

# ----------------------------------------------------------------------

function(ppi_fmt_test target)
  ppi_test(fmt_${target} ${ARGN})
  target_link_libraries(fmt_${target}_test ppi_fmt)
endfunction()

function(ppi_fmt_add_executable target)
  ppi_add_executable(${target} ${ARGN})
  target_link_libraries(${target} ppi_fmt)
endfunction()

ppi_fmt_test(dwt dwt_test.cc)
ppi_fmt_test(fft fft_test.cc)
ppi_fmt_test(ntt ntt_test.cc)
ppi_fmt_test(rft rft_test.cc)

ppi_fmt_add_executable(fft_examine fft_examine.cc)