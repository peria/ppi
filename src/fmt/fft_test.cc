#include "fmt/fmt.h"

#include <gtest/gtest.h>

namespace ppi {
namespace fmt {

TEST(FftTest, Fft2ElemTest) {
  const double kEps = 1e-10;

  Complex a[] = {{1, 2}, {3, 4}};
  Fft::Transfer(Fft::Forward, 2, a);
  EXPECT_NEAR(4, a[0].real, kEps);
  EXPECT_NEAR(6, a[0].imag, kEps);
  EXPECT_NEAR(-2, a[1].real, kEps);
  EXPECT_NEAR(-2, a[1].imag, kEps);

  Fft::Transfer(Fft::Inverse, 2, a);
  EXPECT_NEAR(1, a[0].real, kEps);
  EXPECT_NEAR(2, a[0].imag, kEps);
  EXPECT_NEAR(3, a[1].real, kEps);
  EXPECT_NEAR(4, a[1].imag, kEps);
}

TEST(FftTest, FftTest) {
  const double kEps = 1e-10;

  const int n = 1 << 10;
  Complex a[n];
  for (int i = 0; i < n; ++i) {
    a[i].real = i;
    a[i].imag = i + n;
  }    
  Fft::Transfer(Fft::Forward, n, a);
  Fft::Transfer(Fft::Inverse, n, a);
  for (int i = 0; i < n; ++i) {
    EXPECT_NEAR(i, a[i].real, kEps);
    EXPECT_NEAR(i + n, a[i].imag, kEps);
  }
}

}  // namespace fmt
}  // namespace ppi
