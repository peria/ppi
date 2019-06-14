#include "fmt/dft.h"

#include <gtest/gtest.h>

#include <vector>

#include "fmt/fmt.h"
#include "fmt/rft.h"

namespace ppi {
namespace fmt {

TEST(DftTest, Dft2ElemTest) {
  static constexpr double kEps = 1e-10;
  Dft dft(2);

  Complex a[] = {{1, 2}, {3, 4}};
  dft.Transform(Direction::Forward, a);
  EXPECT_NEAR(4, a[0].real, kEps);
  EXPECT_NEAR(6, a[0].imag, kEps);
  EXPECT_NEAR(-2, a[1].real, kEps);
  EXPECT_NEAR(-2, a[1].imag, kEps);

  dft.Transform(Direction::Backward, a);
  EXPECT_NEAR(1, a[0].real, kEps);
  EXPECT_NEAR(2, a[0].imag, kEps);
  EXPECT_NEAR(3, a[1].real, kEps);
  EXPECT_NEAR(4, a[1].imag, kEps);
}

TEST(DftTest, DftTest) {
  const double kEps = 1e-10;

  for (int64 k = 1; k <= 10; ++k) {
    const int64 n = 1 << k;
    Dft dft(n);
    std::vector<Complex> a(n);
    for (int i = 0; i < n; ++i) {
      a[i].real = i;
      a[i].imag = i + n;
    }
    dft.Transform(Direction::Forward, a.data());
    dft.Transform(Direction::Backward, a.data());
    for (int i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i].real, kEps) << "index=" << i << ", n=2^" << k;
      ASSERT_NEAR(i + n, a[i].imag, kEps) << "index=" << i << ", n=2^" << k;
    }
  }
}

TEST(DftTest, SixStepFftTest) {
  const double kEps = 1e-10;

  for (int64 k = 2; k <= 10; ++k) {
    const int64 k1 = k / 2;
    const int64 k2 = k - k1;
    const int64 n1 = 1 << k1;
    const int64 n2 = 1 << k2;
    const int64 n = n1 * n2;
    Dft dft(n1, n2);
    std::vector<Complex> a(n);
    for (int i = 0; i < n; ++i) {
      a[i].real = i;
      a[i].imag = i + n;
    }
    dft.Transform(Direction::Forward, a.data());
    dft.Transform(Direction::Backward, a.data());
    for (int64 i = 0; i < n; ++i) {
      ASSERT_NEAR(i, a[i].real, kEps)
          << "index=" << i << ", n=2^" << k1 << "*2^" << k2;
      ASSERT_NEAR(i + n, a[i].imag, kEps)
          << "index=" << i << ", n=2^" << k1 << "*2^" << k2;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
