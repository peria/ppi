#include "number/integer.h"

#include <algorithm>
#include <cmath>
#include <ostream>

#include "base/allocator.h"
#include "base/base.h"
#include "fmt/fmt.h"
#include "number/integer_core.h"

namespace ppi {
namespace number {

Integer::Integer() : data_(base::Allocator::Allocate<uint64>(0)) {}

Integer::Integer(uint64 value) : data_(base::Allocator::Allocate<uint64>(1)) {
  (*this)[0] = value;
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
  const int64 n = std::min(a.size(), b.size());
  c->resize(std::max(a.size(), b.size()));

  uint64 carry = 0;
  int64 i = 0;
  for (i = 0; i < n; ++i) {
    uint64 s = b[i] + carry;
    carry = (s < b[i]) ? 1 : 0;
    (*c)[i] = a[i] + s;
    carry += ((*c)[i] < s) ? 1 : 0;
  }
  for (; i < a.size(); ++i) {
    (*c)[i] = a[i] + carry;
    carry = ((*c)[i] < carry) ? 1 : 0;
  }
  for (; i < b.size(); ++i) {
    (*c)[i] = b[i] + carry;
    carry = ((*c)[i] < carry) ? 1 : 0;
  }
  if (carry) {
    c->push_back(carry);
  }

  c->Normalize();
}

// static
void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  c->resize(a.size());

  uint64 carry = 0;
  for (int64 i = 0; i < b.size(); ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    (*c)[i] = s - b[i];
    carry += ((*c)[i] > s) ? 1 : 0;
  }
  for (int64 i = b.size(); i < a.size(); ++i) {
    uint64 s = a[i] - carry;
    carry = (s > a[i]) ? 1 : 0;
    (*c)[i] = s;
  }

  c->Normalize();
}

namespace {

int64 MinPow2(int64 n) {
  static const int64 kCandidates[] = {
      1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7,
      1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14,
      1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21,
      1 << 22, 1 << 23, 1 << 24, 1 << 25, 1 << 26, 1 << 27, 1 << 28,
      1 << 29, 1 << 30,};
  return *std::lower_bound(kCandidates,
                           kCandidates + ARRAY_SIZE(kCandidates), n);
}

}

// static
double Integer::Mult(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  const int64 n = MinPow2(na + nb);
  c->resize(n);

  double err = IntegerCore::Mult(a.data_, na, b.data_, nb, n, c->data_);

  c->Normalize();

  return err;
}

namespace {
// TODO: Move these constant values to a header in base/.
const int64 kHalfSize = 32;
const int64 kHalfBitMask = (1ULL << kHalfSize) - 1;
}

// TODO: Accept uint64 as a second argument.
void Integer::Mult(const Integer& a, const uint32 b, Integer* c) {
  c->resize(a.size());
  uint64 carry = 0;
  for (int64 i = 0; i < a.size(); ++i) {
    uint64 a_low = a[i] & kHalfBitMask;
    uint64 a_high = a[i] >> kHalfSize;
    uint64 c_low = a_low * b + carry;
    uint64 c_high = a_high * b;
    (*c)[i] = c_low + (c_high << kHalfSize);
    carry = c_high >> kHalfSize;
    if ((*c)[i] < c_low)
      ++carry;
  }
  if (carry) {
    c->push_back(carry);
  }
}

void Integer::Div(const Integer& a, const uint32 b, Integer* c) {
  uint64 limb = 0;
  for (int64 i = a.size() - 1; i >= 0; --i) {
    uint64 ia = a[i];
    limb = (limb << kHalfSize) + (ia >> kHalfSize);
    uint64 ic = limb / b;
    limb %= b;
    limb = (limb << kHalfSize) + (ia & kHalfBitMask);
    ic = (ic << kHalfSize) + limb / b;
    limb %= b;
    (*c)[i] = ic;
  }
  c->resize(a.size());
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
    sprintf(buffer, "%016lx", val[i]);
    os << buffer;
  }
  return os;
}

}  // namespace number
}  // namespace ppi
