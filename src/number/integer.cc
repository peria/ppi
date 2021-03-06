#include "number/integer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <ostream>

#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace number {

#ifndef UINT128
namespace {
constexpr int64 kHalfSize = 32;
constexpr uint64 kHalfBitMask = (1ULL << kHalfSize) - 1;
}  // namespace
#endif

Integer::Integer(const Base base)
    : data_(base::Allocator::Allocate<uint64>(0), &base::Allocator::Deallocate),
      base_(base) {}

Integer::Integer(uint64 value, const Base base)
    : data_(base::Allocator::Allocate<uint64>(1), &base::Allocator::Deallocate),
      base_(base) {
  (*this)[0] = value;
}

Integer::Integer(const Integer& other)
    : data_(base::Allocator::Allocate<uint64>(other.size()),
            &base::Allocator::Deallocate),
      base_(other.base()) {
  for (int64 i = 0; i < size(); ++i) {
    (*this)[i] = other[i];
  }
}

Integer::~Integer() = default;

void Integer::Normalize() {
  int64 i = size() - 1;
  while (i >= 0 && (*this)[i] == 0) {
    --i;
  }
  ++i;
  erase(i, size());
}

uint64 Integer::leading() const {
  return (*this)[size() - 1];
}

void Integer::resize(int64 sz) {
  Digits new_ptr_impl(base::Allocator::Allocate<uint64>(sz),
                      &base::Allocator::Deallocate);
  uint64* new_ptr = new_ptr_impl.get();
  if (data_) {
    for (int64 i = 0; i < std::min(sz, size()); ++i)
      new_ptr[i] = (*this)[i];
  }
  std::swap(data_, new_ptr_impl);
}

void Integer::erase(int64 begin, int64 end) {
  if (begin == end)
    return;

  int64 erase_size = end - begin;
  Digits new_ptr_impl(base::Allocator::Allocate<uint64>(size() - erase_size),
                      &base::Allocator::Deallocate);
  uint64* new_ptr = new_ptr_impl.get();
  for (int64 i = 0; i < begin; ++i) {
    new_ptr[i] = (*this)[i];
  }
  for (int64 i = end, j = begin; i < size(); ++i, ++j) {
    new_ptr[j] = (*this)[i];
  }

  std::swap(data_, new_ptr_impl);
}

void Integer::clear() {
  data_.reset(base::Allocator::Allocate<uint64>(0));
}

void Integer::insert(int64 from, int64 number, uint64 value) {
  Digits new_ptr_impl(base::Allocator::Allocate<uint64>(size() + number),
                      &base::Allocator::Deallocate);
  uint64* new_ptr = new_ptr_impl.get();

  for (int64 i = 0; i < from; ++i)
    new_ptr[i] = (*this)[i];
  for (int64 i = 0; i < number; ++i)
    new_ptr[i + from] = value;
  for (int64 i = from + number, j = from; j < size(); ++i, ++j)
    new_ptr[i] = (*this)[j];

  std::swap(data_, new_ptr_impl);
}

void Integer::push_leading(uint64 value) {
  resize(size() + 1);
  (*this)[size() - 1] = value;
}

// static
void Integer::Add(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  const int64 n = std::min(na, nb);
  c->resize(std::max(na, nb));

  uint64 carry = Natural::Add(a.data(), b.data(), n, c->data());
  carry = Natural::Add(a.data() + n, carry, na - n, c->data() + n);
  carry = Natural::Add(b.data() + n, carry, nb - n, c->data() + n);

  if (carry) {
    c->push_leading(carry);
  }
  c->Normalize();
}

// static
void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  CHECK_GE(na, nb);
  c->resize(na);

  uint64 carry = Natural::Subtract(a.data(), b.data(), nb, c->data());
  carry = Natural::Subtract(a.data() + nb, carry, na - nb, c->data() + nb);
  CHECK_EQ(0ULL, carry);

  c->Normalize();
}

namespace {

int64 MinPow2(int64 n) {
  static const int64 kCandidates[] = {
      1 << 0,  1 << 1,  1 << 2,  1 << 3,  1 << 4,  1 << 5,  1 << 6,  1 << 7,
      1 << 8,  1 << 9,  1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15,
      1 << 16, 1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1 << 22, 1 << 23,
      1 << 24, 1 << 25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1 << 30,
  };
  return *std::lower_bound(kCandidates, kCandidates + array_size(kCandidates),
                           n);
}
}  // namespace

// static
double Integer::Mult(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  const int64 n = MinPow2(na + nb);
  c->resize(n);

  double err = Natural::Mult(a.data(), na, b.data(), nb, n, c->data());

  c->Normalize();

  return err;
}

void Integer::Mult(const Integer& a, const uint64 b, Integer* c) {
  c->resize(a.size());
  uint64 carry = Natural::Mult(a.data(), b, a.size(), c->data());
  if (carry) {
    c->push_leading(carry);
  }
}

namespace {

uint64 getMSB(uint64 a) {
  static constexpr uint64 kHigh32Bits = 0xFFFFFFFF00000000ULL;
  static constexpr uint64 kHigh16Bits = 0xFFFF0000FFFF0000ULL;
  static constexpr uint64 kHigh8Bits = 0xFF00FF00FF00FF00ULL;
  static constexpr uint64 kHigh4Bits = 0xF0F0F0F0F0F0F0F0ULL;
  static constexpr uint64 kHigh2Bits = 0xCCCCCCCCCCCCCCCCULL;
  static constexpr uint64 kHigh1Bit = 0xAAAAAAAAAAAAAAAAULL;
  a = (a & kHigh32Bits) ? (a & kHigh32Bits) : a;
  a = (a & kHigh16Bits) ? (a & kHigh16Bits) : a;
  a = (a & kHigh8Bits) ? (a & kHigh8Bits) : a;
  a = (a & kHigh4Bits) ? (a & kHigh4Bits) : a;
  a = (a & kHigh2Bits) ? (a & kHigh2Bits) : a;
  a = (a & kHigh1Bit) ? (a & kHigh1Bit) : a;
  return a;
}

}  // namespace

void Integer::Power(const uint64 a, const uint64 e, Integer* c) {
  (*c) = 1;
  for (uint64 b = getMSB(e); b; b >>= 1) {
    Integer::Mult(*c, *c, c);
    if (b & e) {
      Integer::Mult(*c, a, c);
    }
  }
}

void Integer::Div(const Integer& a, const uint64 b, Integer* c) {
  c->resize(a.size());
  Natural::Div(a.data(), b, a.size(), c->data());
  c->Normalize();
}

void Integer::RBitShift(const Integer& a, const uint64 b, Integer* c) {
  CHECK_NE(&a, c);
  const uint64 limb_shift = b / 64;
  const uint64 bit_shift = b % 64;
  c->resize(a.size() - limb_shift);

  if (bit_shift == 0) {
    for (int64 i = 0; i < c->size(); ++i) {
      (*c)[i] = a[i + limb_shift];
    }
    return;
  }

  uint64 limb = (limb_shift == 0) ? 0 : (a[limb_shift - 1] >> bit_shift);
  for (int64 i = 0; i < c->size() - 1; ++i) {
    uint64 ai = a[i + limb_shift + 1];
    limb |= (ai << (64 - bit_shift));
    (*c)[i] = limb;
    limb = ai >> bit_shift;
  }
  (*c)[c->size() - 1] = limb;
  c->Normalize();
}

void Integer::LBitShift(const Integer& a, const uint64 b, Integer* c) {
  CHECK_NE(&a, c);
  const uint64 limb_shift = b / 64;
  const uint64 bit_shift = b % 64;

  if (bit_shift == 0) {
    c->resize(a.size() + limb_shift);
    for (int64 i = 0; i < limb_shift; ++i) {
      (*c)[i] = 0;
    }
    for (int64 i = 0; i < a.size(); ++i) {
      (*c)[i + limb_shift] = a[i];
    }
    return;
  }

  c->resize(a.size() + limb_shift + 1);
  uint64 limb = 0;
  for (int64 i = 0; i < a.size(); ++i) {
    limb |= a[i] << bit_shift;
    (*c)[i + limb_shift] = limb;
    limb = (a[i] >> (64 - bit_shift));
  }
  (*c)[a.size() + limb_shift] = limb;
  for (int64 i = 0; i < limb_shift; ++i) {
    (*c)[i] = 0;
  }
  c->Normalize();
}

Integer& Integer::operator=(const Integer& other) {
  this->resize(other.size());
  for (int64 i = 0; i < size(); ++i)
    (*this)[i] = other[i];
  return (*this);
}

Integer& Integer::operator=(uint64 a) {
  this->resize(1);
  (*this)[0] = a;
  return (*this);
}

std::ostream& operator<<(std::ostream& os, const Integer& val) {
  static char buffer[50];
  const char* fmt_lead =
      (val.base() == Integer::Base::kHex) ? "%" PRIX64 : "%" PRIu64;
  const char* fmt_digs =
      (val.base() == Integer::Base::kHex) ? "%016" PRIX64 : "%019" PRIu64;

  if (val.size() == 0) {
    os << "0";
    return os;
  }

  sprintf(buffer, fmt_lead, val[val.size() - 1]);
  os << buffer;
  for (int64 i = val.size() - 2; i >= 0; --i) {
    sprintf(buffer, fmt_digs, val[i]);
    os << buffer;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Integer::Base& base) {
  switch (base) {
  case Integer::Base::kHex:
    os << "[16]";
    break;
  case Integer::Base::kDecimal:
    os << "[10]";
    break;
  default:
    os << "[?]";
  }
  return os;
}

}  // namespace number
}  // namespace ppi
