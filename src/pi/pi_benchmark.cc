#include "pi/drm.h"

#include <benchmark/benchmark.h>

#include "base/base.h"
#include "number/real.h"

using ppi::int64;

void BM_PiChudnovsky(benchmark::State& state) {
  ppi::number::Real pi;
  int64 limbs = state.range_x() / 16 + 1;
  pi.setPrecision(limbs);
  ppi::pi::Drm::Chudnovsky(&pi);
}

BENCHMARK(BM_PiChudnovsky)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
