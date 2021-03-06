#include "number/real.h"

#include <glog/logging.h>

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace number {

namespace {

const double kPow2_64 = 18446744073709551616.0;  // 2^64
const double kPow2_m64 = 1.0 / kPow2_64;

}  // namespace

Real::Real(const Base base) : Integer(base), precision_(0), exponent_(0) {}

Real::Real(double d, const Base base) : Integer(base) {
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

Real::Real(const Real& other)
    : Integer(other),
      precision_(other.precision_),
      exponent_(other.exponent_) {}

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
    for (int64 i = 0; i < tmp.size(); ++i) {
      tmp[i] = ~tmp[i];
    }
    tmp.Normalize();
    for (int64 i = 0; i < tmp.size(); ++i) {
      if (++tmp[i])
        break;
    }
    if (tmp.leading() == 0)
      tmp.push_leading(1);

    Div(tmp, 2, &tmp);
    // TODO: in this multiplication, &tmp needs to be extended.
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
  DCHECK(&a != val);

  Real tmp;
  int64 length = val->precision();

  // If a == 1
  if (a.size() == 1 && a[0] == 1) {
    *val = 1;
    val->exponent_ = -a.exponent();
    return 0;
  }

  // Initialize
  double da = a.leading();
  if (a.size() > 1) {
    da += a[a.size() - 2] * kPow2_m64;
  }
  // TODO: Look for better method.
  // Here, (*val * da) <= 1.
  *val = (1.0 - (1.0 / (1ULL << 52))) / da;
  val->exponent_ = -(a.exponent() + a.size()) - val->size() + 1;

  double max_error = 0;
  for (int64 k = 2; k < length * 2; k *= 2) {
    tmp.setPrecision(k);
    double err = Mult(a, *val, &tmp);
    max_error = std::max(max_error, err);

    // Computing "1 - |tmp|", assuming |tmp| is a bit smaller than 1.
    for (int64 i = 0; i < tmp.size(); ++i) {
      tmp[i] = ~tmp[i];
    }
    tmp.Normalize();
    for (int64 i = 0; i < tmp.size(); ++i) {
      if (++tmp[i])
        break;
    }
    if (tmp.leading() == 0)
      tmp.push_leading(1);

    Mult(*val, tmp, &tmp);
    val->setPrecision(k * 2);
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
  sum.resize(c_lead - c_exp);
  for (int64 i = 0; i < c_lead - c_exp; ++i)
    sum[i] = 0;
  sum.exponent_ = c_exp;

  int64 ica = a.exponent() - c_exp;
  int64 icb = b.exponent() - c_exp;
  int64 ia = 0, ib = 0, ic = 0;
  for (; ic < icb && ia < a.size(); ++ic, ++ia) {
    sum[ic] = a[ia];
  }
  for (; ic < ica && ib < b.size(); ++ic, ++ib) {
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
  for (; ia < a.size() && ib < b.size(); ++ia, ++ib, ++ic) {
    uint64 t = a[ia] + carry;
    carry = (t < carry) ? 1 : 0;
    sum[ic] = b[ib] + t;
    carry += (t > sum[ic]) ? 1 : 0;
  }
  for (; ia < a.size(); ++ia, ++ic) {
    sum[ic] = a[ia] + carry;
    carry = (sum[ic] < carry) ? 1 : 0;
  }
  for (; ib < b.size(); ++ib, ++ic) {
    sum[ic] = b[ib] + carry;
    carry = (sum[ic] < carry) ? 1 : 0;
  }
  if (carry)
    sum.push_leading(carry);

  sum.setPrecision(prec);
  sum.Normalize();

  c->release();
  *c = sum;
  sum.release();
}

void Real::Sub(const Real& a, const Real& b, Real* c) {
  int64 prec = c->precision();
  int64 a_lead = a.size() + a.exponent();
  int64 b_lead = b.size() + b.exponent();
  int64 c_lead = std::max(a_lead, b_lead);
  int64 c_exp = std::min(a.exponent(), b.exponent());

  Real diff;
  diff.resize(c_lead - c_exp);
  for (int64 i = 0; i < c_lead - c_exp; ++i)
    diff[i] = 0;
  diff.exponent_ = c_exp;

  uint64 borrow = 0;
  int64 ica = a.exponent() - c_exp;
  int64 icb = b.exponent() - c_exp;
  int64 ia = 0, ib = 0, ic = 0;
  for (; ic < icb && ia < a.size(); ++ic, ++ia) {
    diff[ic] = a[ia];
  }
  for (; ic < ica && ib < b.size(); ++ic, ++ib) {
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
  for (; ia < a.size() && ib < b.size(); ++ia, ++ib, ++ic) {
    uint64 t = a[ia] - borrow;
    borrow = (t > a[ia]) ? 1 : 0;
    diff[ic] = t - b[ib];
    borrow += (t < diff[ic]) ? 1 : 0;
  }
  for (; ia < a.size(); ++ia, ++ic) {
    diff[ic] = a[ia] - borrow;
    borrow = (diff[ic] > a[ia]) ? 1 : 0;
  }
  for (; ib < b.size(); ++ib, ++ic) {
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
  c->exponent_ = a.exponent() + b.exponent();
  c->Normalize();
  return err;
}

void Real::Mult(const Real& a, const uint64 b, Real* c) {
  Integer::Mult(a, b, c);
  c->precision_ = c->size();
  c->exponent_ = a.exponent_;
  c->Normalize();
}

void Real::Div(const Real& a, const uint64 b, Real* c) {
  int64 prec = c->precision();

  *c = a;
  c->exponent_ = a.exponent_;
  if (c->precision() > c->size()) {
    int64 diff = c->precision() - c->size();
    c->insert(0, diff, 0);
    c->exponent_ -= diff;
  }
  Integer::Div(*c, b, c);
  c->setPrecision(prec);
  c->Normalize();
}

// static
void Real::ConvertBase(const Real& a, Real& b) {
  CHECK_NE(a.base(), b.base());
  if (a.base() == Integer::Base::kHex) {
    HexToDecimal(a, b);
  }
}

int64 Real::Compare(std::string& filename) {
  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    LOG(INFO) << "Cannot read " << filename << "\n";
    return 0;
  }

  char refer[20];
  ifs.read(refer, 2);
  if (std::strncmp(refer, "0.", 2)) {
    LOG(INFO) << filename << " may have a wrong format\n";
    return 0;
  }

  int64 match = -16;
  for (int64 i = size() - 1; i >= 0 && !ifs.eof(); --i) {
    char buffer[20];
    ifs.read(refer, 16);
    std::sprintf(buffer, "%016" PRIX64, (*this)[i]);
    for (int64 j = 0; j < 16; ++j) {
      if (buffer[j] != refer[j])
        return match + j;
    }
    match += 16;
  }
  return match;
}

void Real::setPrecision(int64 prec) {
  int64 diff = precision_ - prec;
  if (diff == 0)
    return;

  precision_ = prec;
  Normalize();
}

Real& Real::operator=(const Real& other) {
  DCHECK_EQ(base(), other.base());

  Integer::operator=(other);
  precision_ = other.precision();
  exponent_ = other.exponent();
  return (*this);
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

  // Remove tailing limbs if the mantissa is long.
  int diff = size() - precision_;
  if (diff > 0) {
    erase(0, diff);
    exponent_ += diff;
  }
}

int64 Real::TailingZero() {
  int64 val = 0;
  while (val < size() && (*this)[val] == 0)
    ++val;
  return val;
}

// static
void Real::HexToDecimal(const Real& a, Real& b) {
  static constexpr uint64 kDecBase = 10000000000000000000ULL;

  int64 integral_size = a.size() + a.exponent();
  // TODO: Make this routine general.
  CHECK_EQ(1, integral_size);
  CHECK_EQ(3, a[a.size() - 1]);

  int64 sz = b.precision();
  b.exponent_ = -sz;
  b.resize(sz + 1);
  b[b.size() - 1] = a[a.size() - 1];

  // Rational part
  Real tmp(a);
  tmp.resize(tmp.size() - 1);
  for (int64 i = b.size() - 2; i >= 0; --i) {
    b[i] = Natural::Mult(tmp.data(), kDecBase, tmp.size(), tmp.data());
  }
}

std::ostream& operator<<(std::ostream& os, const Real& val) {
  const int64 digs_per_elem = (val.base() == Integer::Base::kHex) ? 16 : 19;
  os << *reinterpret_cast<const Integer*>(&val) << "e"
     << (val.exponent() * digs_per_elem);
  return os;
}

}  // namespace number
}  // namespace ppi
