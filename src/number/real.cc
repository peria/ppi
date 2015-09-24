#include "number/real.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>

#include "base/base.h"

namespace ppi {
namespace number {

namespace {

const double kPow2_64 = 18446744073709551616.0;  // 2^64

}  // namespace

double Real::InverseSqrt(uint64 a, Real* val) {
  Real tmp;

  val->Set(0, static_cast<uint64>(kPow2_64 / std::sqrt(a)));
  val->size_ = 1;

  double max_error = 0;
  for (int64 k = 0; k < 10; ++k) {
    double err = Mult(*val, *val, &tmp);
    max_error = std::max(err, max_error);

    // Computing "1 - |tmp|", assuming |tmp| is a bit smaller than 1.
    for (int64 i = 0; i < val->size_; ++i) {
      val->mantissa_[i] = ~(val->mantissa_[i]);
    }
    // TODO: check carrying up
    ++(tmp.mantissa_[0]);

    Div(tmp, 2, &tmp);
  }
  
  return 0;
}

double Real::Mult(const Real& a, const Real& b, Real* c) {
  double err = Integer::Mult(a, b, c);
  c->exponent_ = a.exponent_ + b.exponent_;
  return err;
}

void Real::Div(const Real& a, const uint32 b, Real* c) {
  Integer::Div(a, b, c);
  c->exponent_ = a.exponent_;
  // TODO: Shift mantissa and update the exponent, if the leading limb is 0.
}

}  // namespace number
}  // namespace ppi
