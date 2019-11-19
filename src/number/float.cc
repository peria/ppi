#include "number/float.h"

#include <glog/logging.h>

#include <cmath>

#include "base/allocator.h"
#include "number/internal.h"

namespace ppi {

Float::Float() : Natural() {}

void Float::add(const Float& a, const Float& b, Float& c) {
  const Digit* dig_a = a.digits_;
  const Digit* dig_b = b.digits_;

  const int64 nc = c.precision();
  Digit* dig_c = base::Allocator::allocate<Digit>(nc);
  int64 sign_c = 0;
  int64 exp_c = 0;

  if (a.sign() == b.sign()) {
    Digit carry = 0;
    if (cmpAbs(a, b) > 0) {
      exp_c = a.size() + a.exponent() - nc;
      carry = internal::fadd(dig_a, a.size(), a.exponent(),
                             dig_b, b.size(), b.exponent(),
                             nc, exp_c, dig_c);
    } else {
      exp_c = b.size() + b.exponent() - nc;
      carry = internal::fadd(dig_b, b.size(), b.exponent(),
                             dig_a, a.size(), a.exponent(),
                             nc, exp_c, dig_c);
    }
    if (carry) {
      for (int64 i = 0; i < nc - 1; ++i)
        dig_c[i] = dig_c[i + 1];
      dig_c[nc - 1] = carry;
    }
    sign_c = a.sign();
  } else {
    if (cmpAbs(a, b) > 0) {
      exp_c = a.size() + a.exponent() - nc;
      internal::fsubtract(dig_a, a.size(), a.exponent(),
                          dig_b, b.size(), b.exponent(),

                          nc, exp_c, dig_c);
      sign_c = a.sign();
    } else {
      exp_c = b.size() + b.exponent() - nc;
      internal::fsubtract(dig_b, b.size(), b.exponent(),
                          dig_a, a.size(), a.exponent(),
                          nc, exp_c, dig_c);
      sign_c = b.sign();
    }
  }

  if (c.digits_)
    base::Allocator::deallocate(c.digits_);

  c.digits_ = dig_c;
  c.size_ = nc;
  c.exponent_ = exp_c;
  c.sign_ = sign_c;

  c.normalize();
}

void Float::subtract(const Float& a, const Float& b, Float& c) {
  const Digit* dig_a = a.digits_;
  const Digit* dig_b = b.digits_;

  const int64 nc = c.precision();
  Digit* dig_c = base::Allocator::allocate<Digit>(nc);
  int64 sign_c = 0;
  int64 exp_c = 0;

  if (a.sign() != b.sign()) {
    Digit carry = 0;
    if (cmpAbs(a, b) > 0) {
      exp_c = a.size() + a.exponent() - nc;
      carry = internal::fadd(dig_a, a.size(), a.exponent(),
                             dig_b, b.size(), b.exponent(),
                             nc, exp_c, dig_c);
    } else {
      exp_c = b.size() + b.exponent() - nc;
      carry = internal::fadd(dig_b, b.size(), b.exponent(),
                             dig_a, a.size(), a.exponent(),
                             nc, exp_c, dig_c);
    }
    if (carry) {
      for (int64 i = 0; i < nc - 1; ++i)
        dig_c[i] = dig_c[i + 1];
      dig_c[nc - 1] = carry;
    }
    sign_c = a.sign();
  } else {
    if (cmpAbs(a, b) > 0) {
      exp_c = a.size() + a.exponent() - nc;
      internal::fsubtract(dig_a, a.size(), a.exponent(),
                          dig_b, b.size(), b.exponent(),

                          nc, exp_c, dig_c);
      sign_c = a.sign();
    } else {
      exp_c = b.size() + b.exponent() - nc;
      internal::fsubtract(dig_b, b.size(), b.exponent(),
                          dig_a, a.size(), a.exponent(),
                          nc, exp_c, dig_c);
      sign_c = b.sign();
    }
  }

  if (c.digits_)
    base::Allocator::deallocate(c.digits_);

  c.digits_ = dig_c;
  c.size_ = nc;
  c.exponent_ = exp_c;
  c.sign_ = sign_c;

  c.normalize();
}

double Float::mult(const Float& a, const Float& b, Float& c) {
  double err = Natural::mult(a, b, c);
  c.exponent_ = a.exponent_ + b.exponent_ + 1;
  c.normalize();
  return err;
}

int Float::cmpAbs(const Float& a, const Float& b) {
  int64 ea = a.size() + a.exponent();
  int64 eb = b.size() + b.exponent();
  if (ea != eb) return ea - eb;

  int64 ia = a.size() - 1;
  int64 ib = a.size() - 1;
  for (; ia >= 0 && ib >= 0; --ia, --ib) {
    if (a[ia] > b[ib]) return 1;
    if (a[ia] < b[ib]) return -1;
  }
  if (ia >= 0) {
    for (; ia >= 0; --ia)
      if (a[ia])
        return 1;
  }
  if (ib >= 0) {
    for (; ib >= 0; --ib)
      if (b[ib])
        return -1;
  }
  return 0;
}

void Float::inverse(const Float& a, Float& c) {
  double da = a[a.size() - 1];
  if (a.size() > 2) {
    da += a[a.size() - 2] / kBaseInDouble;
  }
  // Set the initial value a bit smaller than the exact value.
  double dc = (1.0 - 8e-17) / da * kBaseInDouble;
  c.resize(2);
  c[1] = std::floor(dc);
  dc -= c[1];
  c[0] = dc * kBaseInDouble;
  c.exponent_ = -(a.exponent() + a.size()) - 2;
  //for (int64 pa = a.precision(), pc = 4; pa > 2; pa = (pa + 1) / 2, pc *= 2) {
  c.setPrecision(2);
  Float t;
  t.setPrecision(2);
  Float::mult(a, c, t);
  LOG(INFO) << c << " " << t;
  for (int64 i = 0; i < t.size(); ++i) {
    t[i] = kBase - 1 - t[i];
  }
  // t.normalize();
  LOG(INFO) << t;
  //}
  // c = t;  // doesn't work with optimization.
  c.resize(t.size());
  for (int64 i = 0; i < c.precision(); ++i) {
    c[i] = t[i];
  }
  c.exponent_ = t.exponent();
}

std::ostream& operator<<(std::ostream& os, const Float& a) {
  return internal::operator<<(os, a);
}

}  // namespace ppi
