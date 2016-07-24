#include "fmt/dwt.h"

#include <gtest/gtest.h>

namespace ppi {
namespace fmt {

// TODO: Do not use convolution.  Each test should test each transform.
TEST(DwtTest, Dwt4Convolution) {
  const int64 n = 4;
  double a[] = {1, 0, 2, 0, 3, 0, 4, 0};

  Dwt::Dwt4(Direction::Forward, n, reinterpret_cast<Complex*>(a));
  for (int i = 0; i < n; ++i) {
    double ar = a[2*i  ];
    double ai = a[2*i+1];
    a[2*i  ] = ar * ar - ai * ai;
    a[2*i+1] = 2 * ar * ai;
  }
  Dwt::Dwt4(Direction::Backward, n, reinterpret_cast<Complex*>(a));

  const double kEps = 1e-10;
  const double answer[] = {1, 25, 4, 24, 10, 16, 20, 0};
  for (int64 i = 0; i < 2 * n; ++i) {
    EXPECT_NEAR(answer[i], a[i], kEps) << "index: " << i;
  }
}

TEST(DwtTest, Dwt2) {
  const int64 n = 4;
  double a[] = {1, 2, 3, 4, 0, 0, 0, 0};

  Dwt::Dwt2(Direction::Forward, n, reinterpret_cast<Complex*>(a));
  for (int i = 0; i < n; ++i) {
    double ar = a[2*i  ];
    double ai = a[2*i+1];
    a[2*i  ] = ar * ar - ai * ai;
    a[2*i+1] = 2 * ar * ai;
  }
  Dwt::Dwt2(Direction::Backward, n, reinterpret_cast<Complex*>(a));

  const double kEps = 1e-10;
  const double answer[] = {-3, 4, -10, 20, -7, 24, 0, 0};
  for (int64 i = 0; i < 2 * n; ++i) {
    EXPECT_NEAR(answer[i], a[i], kEps) << "index: " << i;
  }
}

TEST(DwtTest, Dwt2ComplexNegaCyclic) {
  const int64 n = 2;
  double a[] = {1, 2, 3, 4};

  Dwt::Dwt2(Direction::Forward, n, reinterpret_cast<Complex*>(a));
  for (int i = 0; i < n; ++i) {
    double ar = a[2*i  ];
    double ai = a[2*i+1];
    a[2*i  ] = ar * ar - ai * ai;
    a[2*i+1] = 2 * ar * ai;
  }
  Dwt::Dwt2(Direction::Backward, n, reinterpret_cast<Complex*>(a));

  const double kEps = 1e-10;
  const double answer[] = {4, -20, -10, 20};
  for (int64 i = 0; i < 2 * n; ++i) {
    EXPECT_NEAR(answer[i], a[i], kEps) << "index: " << i;
  }
}

TEST(DwtTest, Dwt2RealConvolution) {
  const int64 n = 4;
  double a[] = {1, 2, 0, 0};

  Dwt::Dwt2Real(Direction::Forward, n, a);
  for (int i = 0; i < n / 2; ++i) {
    double ar = a[2*i  ];
    double ai = a[2*i+1];
    a[2*i  ] = ar * ar - ai * ai;
    a[2*i+1] = 2 * ar * ai;
  }
  Dwt::Dwt2Real(Direction::Backward, n, a);

  const double kEps = 1e-10;
  const double answer[] = {1, 4, 4, 0};
  for (int64 i = 0; i < n; ++i) {
    EXPECT_NEAR(answer[i], a[i], kEps) << "index: " << i;
  }
}

TEST(DwtTest, Dwt2RealNegaCyclic) {
  const int64 n = 4;
  double a[] = {1, 2, 3, 4};

  Dwt::Dwt2Real(Direction::Forward, n, a);
  for (int i = 0; i < n / 2; ++i) {
    double ar = a[2*i  ];
    double ai = a[2*i+1];
    a[2*i  ] = ar * ar - ai * ai;
    a[2*i+1] = 2 * ar * ai;
  }
  Dwt::Dwt2Real(Direction::Backward, n, a);

  const double kEps = 1e-10;
  const double answer[] = {-24, -20, -6, 20};
  for (int64 i = 0; i < n; ++i) {
    EXPECT_NEAR(answer[i], a[i], kEps) << "index: " << i;
  }
}

}  // namespace fmt
}  // namespace ppi
