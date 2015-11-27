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

Real::Real() : precision_(0), exponent_(0) {}

Real::Real(double d) {
  if (d == 0.0) {
    clear();
    precision_ = 0;
    exponent_ = 0;
    return;
  }
  
  int64 e = 0;
  while (d >= kPow2_64) {
    ++e;
    d *= kPow2_m64;
  }
  while (d < 1) {
    --e;
    d *= kPow2_64;
  }

  uint64 lead = static_cast<uint64>(d);
  if (d == static_cast<double>(lead)) {
    resize(1);
    (*this)[0] = lead;
    precision_ = 1;
  } else {
    resize(2);
    (*this)[0] = static_cast<uint64>((d - lead) * kPow2_64);
    (*this)[1] = lead;
    precision_ = 2;
  }
  exponent_ = e;
}

void Real::fitInteger(int64 n) {
  exponent_ = 0;
  precision_ = n;
}

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
  }

  val->setPrecision(length);
  val->Normalize();

  return max_error;
}

double Real::Inverse(const Real& a, Real* val) {
  Real tmp;
  int64 length = val->precision();

  // If a == 1
  if (a.size() == 1 && a[0] == 1) {
    *val = 1;
    val->exponent_ = -a.exponent();
    return 0;
  }
  
  // Initialize
  double da = a.back();
  if (a.size() > 1) {
    da += a[a.size() - 2] * kPow2_m64;
  }
  *val = 1.0 / da;
  val->exponent_ = -(a.exponent() + a.size()) - 1;

  double max_error = 0;
  for (int64 k = val->size(); k < length * 2;) {
    k *= 2;
    tmp.setPrecision(k);
    double err = Mult(a, *val, &tmp);
    max_error = std::max(max_error, err);

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

    Mult(*val, tmp, &tmp);
    val->setPrecision(k);
    Add(*val, tmp, val);
  }

  val->setPrecision(length);
  val->Normalize();
  
  return max_error;
}

void Real::Add(const Real& a, const Real& b, Real* c) {
  int64 prec = c->precision();
  int64 a_lead = a.size() + a.exponent();
  int64 b_lead = b.size() + b.exponent();
  int64 c_lead = std::max(a_lead, b_lead);
  int64 c_exp = std::min(a.exponent(), b.exponent());

  Real sum;
  sum.resize(c_lead - c_exp, 0);
  sum.exponent_ = c_exp;

  int64 ica = a.exponent() - c_exp;
  int64 icb = b.exponent() - c_exp;
  int64 ia = 0, ib = 0, ic = 0;
  for (; ic < icb && ia < a.ssize(); ++ic, ++ia) {
    sum[ic] = a[ia];
  }
  for (; ic < ica && ib < b.ssize(); ++ic, ++ib) {
    sum[ic] = b[ib];
  }

  if (a_lead < b.exponent()) {
    ic = icb;
  }
  if (b_lead < a.exponent()) {
    ic = ica;
  }
  
  // Now our situation is either of 
  //   aaaaaa          aaa
  // +    bbb  or + bbbbbb
  uint64 carry = 0;
  for (; ia < a.ssize() && ib < b.ssize(); ++ia, ++ib, ++ic) {
    uint64 t = a[ia] + carry;
    carry = (t < carry) ? 1 : 0;
    sum[ic] = b[ib] + t;
    carry += (t > sum[ic]) ? 1 : 0;
  }
  for (; ia < a.ssize(); ++ia, ++ic) {
    sum[ic] = a[ia] + carry;
    carry = (sum[ic] < carry) ? 1 : 0;
  }
  for (; ib < b.ssize(); ++ib, ++ic) {
    sum[ic] = b[ib] + carry;
    carry = (sum[ic] < carry) ? 1 : 0;
  }
  if (carry)
    sum.push_back(carry);

  sum.setPrecision(prec);
  sum.Normalize();

  *c = sum;
}

void Real::Sub(const Real& a, const Real& b, Real* c) {
  int64 prec = c->precision();
  int64 a_lead = a.size() + a.exponent();
  int64 b_lead = b.size() + b.exponent();
  int64 c_lead = std::max(a_lead, b_lead);
  int64 c_exp = std::min(a.exponent(), b.exponent());

  Real diff;
  diff.resize(c_lead - c_exp, 0);
  diff.exponent_ = c_exp;

  uint64 borrow = 0;
  int64 ica = a.exponent() - c_exp;
  int64 icb = b.exponent() - c_exp;
  int64 ia = 0, ib = 0, ic = 0;
  for (; ic < icb && ia < a.ssize(); ++ic, ++ia) {
    diff[ic] = a[ia];
  }
  for (; ic < ica && ib < b.ssize(); ++ic, ++ib) {
    diff[ic] = ~b[ib];
    borrow = 1;
  }
  for (; ic < ica; ++ic) {
    diff[ic] = ~0ULL;
    borrow = 1;
  }

  // Now our situation is
  //   aaaaaa
  // -    bbb
  // because we are assuming a > b.
  for (; ia < a.ssize() && ib < b.ssize(); ++ia, ++ib, ++ic) {
    uint64 t = a[ia] - borrow;
    borrow = (t > a[ia]) ? 1 : 0;
    diff[ic] = t - b[ib];
    borrow += (t < diff[ic]) ? 1 : 0;
  }
  for (; ia < a.ssize(); ++ia, ++ic) {
    diff[ic] = a[ia] - borrow;
    borrow = (diff[ic] > a[ia]) ? 1 : 0;
  }
  for (; ib < b.ssize(); ++ib, ++ic) {
    uint64 t = -borrow;
    borrow = (t > 0) ? 1 : 0;
    diff[ic] = t - b[ib];
    borrow += (t < diff[ic]) ? 1 : 0;
  }

  diff.setPrecision(prec);
  diff.Normalize();

  *c = diff;
}

double Real::Mult(const Real& a, const Real& b, Real* c) {
  double err = Integer::Mult(a, b, c);
  c->exponent_ = a.exponent_ + b.exponent_;
  c->Normalize();
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
  int64 prec = c->precision();

  *c = a;
  c->exponent_ = a.exponent_;
  if (c->precision() > c->size()) {
    int64 diff = c->precision() - c->size();
    c->insert(c->begin(), diff, 0);
    c->exponent_ -= diff;
  }
  Integer::Div(*c, b, c);
  c->setPrecision(prec);
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
  if (d == 0.0) {
    clear();
    precision_ = 0;
    exponent_ = 0;
    return *this;
  }
  
  int64 e = 0;
  while (d >= kPow2_64) {
    ++e;
    d *= kPow2_m64;
  }
  while (d < 1) {
    --e;
    d *= kPow2_64;
  }

  uint64 lead = static_cast<uint64>(d);
  if (d == static_cast<double>(lead)) {
    resize(1);
    (*this)[0] = lead;
    precision_ = 1;
  } else {
    resize(2);
    (*this)[0] = static_cast<uint64>((d - lead) * kPow2_64);
    (*this)[1] = lead;
    precision_ = 2;
  }
  exponent_ = e;

  return *this;
}

void Real::Normalize() {
  // Remove leading zeros.
  Integer::Normalize();

  // Remove tailing zeros.
  int64 tail = TailingZero();
  if (tail > 0) {
    erase(begin(), begin() + tail);
    exponent_ += tail;
  }

  // Remove tailing limbs if the mantissa is long.
  int diff = size() - precision_;
  if (diff > 0) {
    erase(begin(), begin() + diff);
    exponent_ += diff;
  }
}

int64 Real::TailingZero() {
  auto itr = begin();
  while (itr != end() && *itr == 0)
    ++itr;
  return itr - begin();
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
