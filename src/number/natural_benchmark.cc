#include "number/natural.h"

#include <benchmark/benchmark.h>

namespace {

void MultiplyBenchmark(benchmark::State& state) {
  using namespace ppi;
  // Perform setup here
  const int64 n = state.range(0);
  for (auto _ : state) {
    Natural a, b, c;
    a.resize(n);
    b.resize(n);
    Natural::mult(a, b, c);
  }
  state.SetComplexityN(state.range(0));
}

}  // namespace

// Register the function as a benchmark
BENCHMARK(MultiplyBenchmark)->Range(1 << 4, 1 << 15);

// Run the benchmark
BENCHMARK_MAIN();
