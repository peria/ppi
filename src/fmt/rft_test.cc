#include "fmt/rft.h"

#include <gtest/gtest.h>

namespace ppi {
namespace fmt {

TEST(RftTest, OverallTest) {
  const double kEps = 1e-10;

  for (int64 k = 2; k <= 10; ++k) {
    const int n = 1 << k;
    Config config(n / 2);
    double a[n];
    for (int i = 0; i < n; ++i) {
      a[i] = i;
    }
    Rft::Transform(config, Direction::Forward, a);
    Rft::Transform(config, Direction::Backward, a);
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i], kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
