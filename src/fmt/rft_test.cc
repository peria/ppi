#include "fmt/rft.h"

#include <gtest/gtest.h>

#include <vector>

namespace ppi {
namespace fmt {

TEST(RftTest, OverallTest) {
  const double kEps = 1e-10;

  for (int64 k = 2; k <= 10; ++k) {
    const int n = 1 << k;
    Rft rft(n);
    std::vector<double> a(n);
    for (int i = 0; i < n; ++i) {
      a[i] = i;
    }
    rft.Transform(Direction::Forward, a.data());
    rft.Transform(Direction::Backward, a.data());
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i], kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
