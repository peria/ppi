#include "drm/chudnovsky.h"

#include <memory>

#include "number/integer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

static constexpr double kDigitsPerTerm = 14.1816474627;

std::unique_ptr<Computer> Chudnovsky::Create(const Parameter& parameter) {
  return std::unique_ptr<Computer>(new Chudnovsky(parameter));
}

Real&& Chudnovsky::Compute() {
  const int64 terms = parameter_.decimal_digits / kDigitsPerTerm + 1;
  Integer x;
  Integer y;
  runDRM(terms, x, y);
  x *= 4270934400;

  Real fx(x);
  fy = Real::Inverse(y);
  fx *= fy;
  Real fy = Real::InverseSqrt(10005);
  fx *= fy;

  return std::move(fx);
}

void Chudnovsky::SetValues(const int64 n, Integer& x, Integer& y, Integer& z) {
  static constexpr int64 A = 13591409;
  static constexpr int64 B = 545140134;
  static constexpr int64 C = 640320;

  if (n == 0) {
    x = 1;
  } else {
    x = C * n;
    x *= C * n;
    x *= C * n / 24;
  }
  y = A + B * n;
  z = 6 * n + 5;
  z *= 6 * n + 1;
  z *= 2 * n + 1;
}

Chudnovsky::Chudnovsky(const Parameter& parameter) : DRM3(parameter) {}

}  // namespace drm
}  // namespace ppi
