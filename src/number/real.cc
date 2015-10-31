#include "number/real.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ostream>
#include <vector>

#include "base/base.h"

namespace ppi {
namespace number {

namespace {

const double kPow2_64 = 18446744073709551616.0;  // 2^64

}  // namespace

double Real::InverseSqrt(uint64 a, Real* val) {
  Real tmp;

  val->resize(1);
  val->exponent_ = -1;
  (*val)[0] = static_cast<uint64>(kPow2_64 / std::sqrt(a));

  double max_error = 0;
  for (int64 k = 0; k < 5; ++k) {
    double err = Mult(*val, *val, &tmp);
    max_error = std::max(err, max_error);
    Mult(tmp, a, &tmp);

    // Computing "1 - |tmp|", assuming |tmp| is a bit smaller than 1.
    for (size_t i = 0; i < tmp.size(); ++i) {
      tmp[i] = ~tmp[i];
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
      ++tmp[i];
      if (tmp[i])
        break;
    }
    if (tmp.back() == 0)
      tmp.push_back(1);
    
    Div(tmp, 2, &tmp);
    Add(*val, tmp, val);
  }
  
  return max_error;
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
    (*c)[c->size()] = carry;
  }
  c->normalize();
}

void Real::Div(const Real& a, const uint32 b, Real* c) {
  Integer::Div(a, b, c);
  c->exponent_ = a.exponent_;
  // TODO: Shift mantissa and update the exponent, if the leading limb is 0.
}

std::ostream& operator<<(std::ostream& os, const Real& val) {
  os << "0." << *reinterpret_cast<const Integer*>(&val);
  int64 diff = val.size() + val.exponent();
  if (diff) {
    os << " * (2^64)^(" << diff << ")";
  }
  return os;
}

}  // namespace number
}  // namespace ppi
