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
const double kPow2_m64 = 1.0 / kPow2_64;

}  // namespace

double Real::InverseSqrt(uint64 a, Real* val) {
  Real tmp;
  int64 length = val->precision();

  // Initialize
  *val = 1.0 / std::sqrt(a);

  double max_error = 0;
  for (int64 k = 1; k < length;) {
    k *= 2;
    tmp.setPrecision(k);
    double err = Mult(*val, *val, &tmp);
    max_error = std::max(err, max_error);
    Mult(tmp, a, &tmp);

    // Computing "1 - |tmp|", assuming |tmp| is a bit smaller than 1.
    for (size_t i = 0; i < tmp.size(); ++i) {
      tmp[i] = ~tmp[i];
    }
    tmp.Normalize();
    for (size_t i = 0; i < tmp.size(); ++i) {
      if (++tmp[i])
        break;
    }
    if (tmp.back() == 0)
      tmp.push_back(1);

    Div(tmp, 2, &tmp);
    // FIXME: in this multiplication, &tmp needs to be extended.
    // Currently, the size of |tmp| is not updated.
    Mult(*val, tmp, &tmp);
    tmp.Normalize();

    val->setPrecision(k);
    Add(*val, tmp, val);
    LOG(INFO) << *val;
  }

  val->setPrecision(length);
  val->Normalize();
  
  return max_error;
}

void Real::Add(const Real& a, const Real& b, Real* c) {
  if (&b == c) {
    Add(a, c);
    return;
  }
  if (&a == c) {
    Add(b, c);
    return;
  }
}

void Real::Add(const Real& a, Real* c) {
  int64 a_lead = a.size() + a.exponent();
  int64 c_lead = c->size() + c->exponent();

  uint64 carry = 0;
  if (a_lead <= c_lead) {
    if (c->precision() > c->size()) {
      int64 diff = c->precision() - c->size();
      c->insert(c->begin(), diff, 0);
      c->exponent_ -= diff;
    }

    size_t ia = 0, ic = 0;
    if (a.exponent() >= c->exponent()) {
      // a:    xxxxxxxx
      // c: yyyyyyyyyyyyyy
      ic = a.exponent() - c->exponent();
    } else {
      // a:    xxxxxxx
      // c: yyyyyyy
      ia = c->exponent() - a.exponent();
    }
    for (; ia < a.size() && ic < c->size(); ++ia, ++ic) {
      uint64 tmp = a[ia] + carry;
      carry = (tmp < carry) ? 1 : 0;
      (*c)[ic] += tmp;
      carry += (tmp > (*c)[ic]) ? 1 : 0;
    }
    for (; ic < c->size() && carry; ++ic) {
      (*c)[ic] += carry;
      carry = ((*c)[ic] == 0) ? 1 : 0;
    }
  } else {

  }

  if (carry) {
    c->push_back(carry);
    c->erase(c->begin());
    ++(c->exponent_);
  }
}

double Real::Mult(const Real& a, const Real& b, Real* c) {
  double err = Integer::Mult(a, b, c);
  c->precision_ = c->size();
  c->exponent_ = a.exponent_ + b.exponent_;
  return err;
}

void Real::Mult(const Real& a, const uint32 b, Real* c) {
  uint64 carry = Integer::Mult(a, b, c);
  c->precision_ = c->size();
  c->exponent_ = a.exponent_;
  if (carry) {
    (*c)[c->size()] = carry;
  }
  c->Normalize();
}

void Real::Div(const Real& a, const uint32 b, Real* c) {
  Integer::Div(a, b, c);
  c->exponent_ = a.exponent_;
  // TODO: Shift mantissa and update the exponent, if the leading limb is 0.
  c->Normalize();
}

void Real::setPrecision(int64 prec) {
  int64 diff = precision_ - prec;
  if (diff == 0)
    return;

  precision_ = prec;
  Normalize();
}

Real& Real::operator=(double d) {
  DCHECK(d > 0);
  
  int64 e = 0;
  while (d >= kPow2_64) {
    ++e;
    d *= kPow2_m64;
  }
  while (d < 1) {
    --e;
    d *= kPow2_64;
  }

  resize(1);
  (*this)[0] = static_cast<uint64>(d);
  precision_ = 1;
  exponent_ = e;

  return *this;
}

void Real::Normalize() {
  Integer::Normalize();

  int64 diff = size() - precision_;
  if (diff > 0) {
    erase(begin(), begin() + diff);
    exponent_ += diff;
  }
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
