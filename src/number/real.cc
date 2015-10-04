#include "number/real.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ostream>

#include "base/base.h"

namespace ppi {
namespace number {

namespace {

const double kPow2_64 = 18446744073709551616.0;  // 2^64

}  // namespace

double Real::InverseSqrt(uint64 a, Real* val) {
  // TODO: assign memory on tmp.mantissa
  Real tmp;

  val->Set(0, static_cast<uint64>(kPow2_64 / std::sqrt(a)));
  val->size_ = 1;
  val->exponent_ = -1;

  double max_error = 0;
  for (int64 k = 0; k < 3; ++k) {
    double err = Mult(*val, *val, &tmp);
    max_error = std::max(err, max_error);
    Mult(tmp, a, &tmp);

    // Computing "1 - |tmp|", assuming |tmp| is a bit smaller than 1.
    for (int64 i = 0; i < val->size_; ++i) {
      tmp.mantissa_[i] = ~(tmp.mantissa_[i]);
    }
    // TODO: check carrying up
    ++(tmp.mantissa_[0]);

    Div(tmp, 2, &tmp);
    Add(*val, tmp, val);
  }
  
  return 0;
}

double Real::Mult(const Real& a, const Real& b, Real* c) {
  double err = Integer::Mult(a, b, c);
  c->exponent_ = a.exponent_ + b.exponent_;
  return err;
}

void Real::Mult(const Real& a, const uint32 b, Real* c) {
  uint64 carry = Integer::Mult(a, b, c);
  c->exponent_ = a.exponent_;
  if (carry) {
    c->Set(c->size(), carry);
    ++c->size_;
  }
}

void Real::Div(const Real& a, const uint32 b, Real* c) {
  Integer::Div(a, b, c);
  c->exponent_ = a.exponent_;
  // TODO: Shift mantissa and update the exponent, if the leading limb is 0.
}

void Real::Show(const Real& val, std::ostream& os) {
  os << "0.";
  Integer::Show(val, os);
  int64 diff = val.size() + val.exponent();
  if (diff) {
    os << " * (2^64)^(" << diff << ")";
  }
}

}  // namespace number
}  // namespace ppi
