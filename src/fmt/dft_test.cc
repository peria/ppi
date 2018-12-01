#include "fmt/dft.h"

#include <gtest/gtest.h>

#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace fmt {

TEST(DftTest, Dft2ElemTest) {
  const double kEps = 1e-10;
  Dft dft(2);

  Complex a[] = {{1, 2}, {3, 4}};
  dft.Transform(Fmt::Direction::Forward, a);
  EXPECT_NEAR(4, a[0].real, kEps);
  EXPECT_NEAR(6, a[0].imag, kEps);
  EXPECT_NEAR(-2, a[1].real, kEps);
  EXPECT_NEAR(-2, a[1].imag, kEps);

  dft.Transform(Fmt::Direction::Backward, a);
  EXPECT_NEAR(1, a[0].real, kEps);
  EXPECT_NEAR(2, a[0].imag, kEps);
  EXPECT_NEAR(3, a[1].real, kEps);
  EXPECT_NEAR(4, a[1].imag, kEps);
}

TEST(DftTest, DftTest) {
  const double kEps = 1e-10;

  for (int64 k = 1; k <= 10; ++k) {
    const int n = 1 << k;
    Dft dft(n);
    Complex a[n];
    for (int i = 0; i < n; ++i) {
      a[i].real = i;
      a[i].imag = i + n;
    }
    dft.Transform(Fmt::Direction::Forward, a);
    dft.Transform(Fmt::Direction::Backward, a);
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i].real, kEps) << "index=" << i << ", n=2^" << k;
      ASSERT_NEAR(i + n, a[i].imag, kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
