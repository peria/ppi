#include "pi/pi.h"

#include <cmath>

#include "base/base.h"
#include "base/time.h"
#include "number/real.h"

namespace ppi {
namespace pi {

double Pi::Arctan2(Real* pi) {
  int64 length = pi->precision();
  (*pi) = 0.0;
  pi->setPrecision(length);

  Real a = 16.0 * 5;
  Real sum = 0.0;
  Real b = 0.0;

  a.setPrecision(length);
  sum.setPrecision(length);
  b.setPrecision(length);
  const int64 n5 = length * 64 / std::log2(25) + 2;
  for (int64 i = 0; i < n5; ++i) {
    Real::Div(a, 25, &a);
    Real::Div(a, 2 * i + 1, &b);
    if (i % 2 == 0) {
      Real::Add(sum, b, &sum);
    } else {
      Real::Sub(sum, b, &sum);
    }
  }
  Real::Add(*pi, sum, pi);

  a = 4 * 239.0;
  sum = 0.0;
  b = 0.0;
  a.setPrecision(length);
  sum.setPrecision(length);
  b.setPrecision(length);
  const int64 n239 = length * 64 / std::log2(239 * 239) + 2;
  for (int64 i = 0; i < n239; ++i) {
    Real::Div(a, 239 * 239, &a);
    Real::Div(a, 2 * i + 1, &b);
    if (i % 2 == 0) {
      Real::Add(sum, b, &sum);
    } else {
      Real::Sub(sum, b, &sum);
    }
  }
  Real::Sub(*pi, sum, pi);

  return 0;
}

}  // namespace pi
}  // namespace ppi
