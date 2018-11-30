#include "fmt/fmt.h"

#include <glog/logging.h>

#include <cmath>

#include "base/allocator.h"
#include "base/util.h"

namespace ppi {
namespace fmt {

Fmt::Config::Config(int64 m) : n(m), log2n(PopCount(m - 1)) {
  DCHECK_EQ(n, 1LL << (log2n));

  table = reinterpret_cast<Complex*>(base::Allocator::Allocate<double>(2 * n));
  const int64 log4n = log2n / 2;
  int64 width = 1;
  Complex* ptr = table;
  for (int64 i = 0; i < log4n; ++i) {
    double th = -M_PI / (2 * width);
    for (int64 j = 0; j < width; ++j) {
      double w1r = ptr[3 * j].real = std::cos(th * j);
      double w1i = ptr[3 * j].imag = std::sin(th * j);
      double w2r = ptr[3 * j + 1].real = w1r * w1r - w1i * w1i;
      double w2i = ptr[3 * j + 1].imag = 2 * w1r * w1i;
      ptr[3 * j + 2].real = w2r * w1r - w2i * w1i;
      ptr[3 * j + 2].imag = w2r * w1i + w1r * w2i;
    }
    ptr += 3 * width;
    width *= 4;
  }
  if (log2n % 2) {
    double th = -M_PI / width;
    for (int64 j = 0; j < width; ++j) {
      ptr[j].real = std::cos(th * j);
      ptr[j].imag = std::sin(th * j);
    }
  }
}

Fmt::Config::~Config() {
  base::Allocator::Deallocate(table);
}

}  // namespace fmt
}  // namespace ppi
