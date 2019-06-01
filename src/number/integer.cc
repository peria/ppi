#include "number/integer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <ostream>

#include "base/allocator.h"
#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace number {

#ifndef UINT128
namespace {
constexpr int64 kHalfSize = 32;
constexpr uint64 kHalfBitMask = (1ULL << kHalfSize) - 1;
} // namespace
#endif

Integer::Integer() : data_(base::Allocator::Allocate<uint64>(0)) {}

Integer::Integer(uint64 value) : data_(base::Allocator::Allocate<uint64>(1)) {
  (*this)[0] = value;
}

Integer::Integer(const Integer& other)
    : data_(base::Allocator::Allocate<uint64>(other.size())) {
  for (int64 i = 0; i < size(); ++i) {
    (*this)[i] = other[i];
  }
}

Integer::~Integer() {
  if (data_)
    base::Allocator::Deallocate(data_);
}

void Integer::Normalize() {
  int64 i = size() - 1;
  while (i >= 0 && (*this)[i] == 0) {
    --i;
  }
  ++i;
  erase(i, size());
}

uint64 Integer::back() const {
  return (*this)[size() - 1];
}

void Integer::resize(int64 sz) {
  uint64* new_ptr = base::Allocator::Allocate<uint64>(sz);
  for (int64 i = 0; i < std::min(sz, size()); ++i)
    new_ptr[i] = (*this)[i];
  std::swap(data_, new_ptr);
  base::Allocator::Deallocate(new_ptr);
}

void Integer::erase(int64 begin, int64 end) {
  if (begin == end)
    return;

  int64 erase_size = end - begin;
  uint64* new_ptr = base::Allocator::Allocate<uint64>(size() - erase_size);
  for (int64 i = 0; i < begin; ++i) {
    new_ptr[i] = (*this)[i];
  }
  for (int64 i = end, j = begin; i < size(); ++i, ++j) {
    new_ptr[j] = (*this)[i];
  }

  std::swap(data_, new_ptr);
  base::Allocator::Deallocate(new_ptr);
}

void Integer::clear() {
  base::Allocator::Deallocate(data_);
  data_ = base::Allocator::Allocate<uint64>(0);
}

void Integer::insert(int64 from, int64 number, uint64 value) {
  uint64* new_ptr = base::Allocator::Allocate<uint64>(size() + number);

  for (int64 i = 0; i < from; ++i)
    new_ptr[i] = (*this)[i];
  for (int64 i = 0; i < number; ++i)
    new_ptr[i + from] = value;
  for (int64 i = from + number, j = from; j < size(); ++i, ++j)
    new_ptr[i] = (*this)[j];

  std::swap(data_, new_ptr);
  base::Allocator::Deallocate(new_ptr);
}

void Integer::push_back(uint64 value) {
  resize(size() + 1);
  (*this)[size() - 1] = value;
}

// static
void Integer::Add(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  const int64 n = std::min(na, nb);
  c->resize(std::max(na, nb));

  uint64 carry = Natural::Add(a.data_, b.data_, n, c->data_);
  carry = Natural::Add(a.data_ + n, carry, na - n, c->data_ + n);
  carry = Natural::Add(b.data_ + n, carry, nb - n, c->data_ + n);

  if (carry) {
    c->push_back(carry);
  }
  c->Normalize();
}

// static
void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  CHECK_GE(na, nb);
  c->resize(na);

  uint64 carry = Natural::Subtract(a.data_, b.data_, nb, c->data_);
  carry = Natural::Subtract(a.data_ + nb, carry, na - nb, c->data_ + nb);
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

  double err = Natural::Mult(a.data_, na, b.data_, nb, n, c->data_);

  c->Normalize();

  return err;
}

void Integer::Mult(const Integer& a, const uint64 b, Integer* c) {
  c->resize(a.size());
  uint64 carry = 0;
#if defined(UINT128)
  uint128 b128 = b;
  for (int64 i = 0; i < a.size(); ++i) {
    uint128 ab = a[i] * b128 + carry;
    (*c)[i] = ab;
    carry = ab >> 64;
  }
#else
  uint64 bl = b & kHalfBitMask;
  uint64 bh = b >> kHalfSize;
  for (int64 i = 0; i < a.size(); ++i) {
    uint64 al = a[i] & kHalfBitMask;
    uint64 ah = a[i] >> kHalfSize;
    uint64 c00 = al * bl;
    uint64 c01 = al * bh;
    uint64 c10 = ah * bl;
    uint64 c11 = ah * bh;
    c01 += c10;
    if (c01 < c10) {
      c11 += 1ULL << kHalfSize;
    }
    uint64 u = c00 + (c01 << kHalfSize);
    if (u < c00) {
      ++c11;
    }
    c11 += c01 >> kHalfSize;

    (*c)[i] = u + carry;
    if ((*c)[i] < u) {
      ++c11;
    }
    carry = c11;
  }
#endif
  if (carry) {
    c->push_back(carry);
  }
}

void Integer::Div(const Integer& a, const uint64 b, Integer* c) {
  c->resize(a.size());
  Natural::Div(a.data_, b, a.size(), c->data_);
  c->Normalize();
}

Integer& Integer::operator=(uint64 a) {
  this->resize(1);
  (*this)[0] = a;
  return (*this);
}

std::ostream& operator<<(std::ostream& os, const Integer& val) {
  static char buffer[50];
  for (int64 i = val.size() - 1; i >= 0; --i) {
    sprintf(buffer, "%016" PRIX64, val[i]);
    os << buffer;
  }
  return os;
}

}  // namespace number
}  // namespace ppi
