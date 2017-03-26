#include "fmt/fft.h"

#include <gtest/gtest.h>

#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace fmt {

TEST(FftTest, Fft2ElemTest) {
  const double kEps = 1e-10;
  Config config(2);

  Complex a[] = {{1, 2}, {3, 4}};
  Fft::Transform(config, Direction::Forward, a);
  EXPECT_NEAR(4, a[0].real, kEps);
  EXPECT_NEAR(6, a[0].imag, kEps);
  EXPECT_NEAR(-2, a[1].real, kEps);
  EXPECT_NEAR(-2, a[1].imag, kEps);

  Fft::Transform(config, Direction::Backward, a);
  EXPECT_NEAR(1, a[0].real, kEps);
  EXPECT_NEAR(2, a[0].imag, kEps);
  EXPECT_NEAR(3, a[1].real, kEps);
  EXPECT_NEAR(4, a[1].imag, kEps);
}

TEST(FftTest, FftTest) {
  const double kEps = 1e-10;

  for (int64 k = 1; k <= 10; ++k) {
    const int n = 1 << k;
    Config config(n);
    Complex a[n];
    for (int i = 0; i < n; ++i) {
      a[i].real = i;
      a[i].imag = i + n;
    }
    Fft::Transform(config, Direction::Forward, a);
    Fft::Transform(config, Direction::Backward, a);
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i].real, kEps) << "index=" << i << ", n=2^" << k;
      ASSERT_NEAR(i + n, a[i].imag, kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

TEST(FftTest, RftTest) {
  const double kEps = 1e-10;

  for (int64 k = 2; k <= 10; ++k) {
    const int n = 1 << k;
    Config config(n);
    double a[n];
    for (int i = 0; i < n; ++i) {
      a[i] = i;
    }
    Rft::Transform(Direction::Forward, n, a);
    Rft::Transform(Direction::Backward, n, a);
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i], kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
