#include <benchmark/benchmark.h>

#include <memory>

#include "base/base.h"
#include "number/real.h"
#include "drm/chudnovsky.h"
#include "drm/drm.h"

using ppi::int64;

void BM_DrmChudnovsky(benchmark::State& state) {
  std::unique_ptr<ppi::drm::Drm> drm(new ppi::drm::Chudnovsky);
  while (state.KeepRunning()) {
    ppi::number::Real pi;
    drm->compute(state.range(0), &pi);
  }
}

BENCHMARK(BM_DrmChudnovsky)->Arg(100000)->Arg(1000000);
