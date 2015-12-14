#include "number/integer.h"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <vector>

#include "base/base.h"
#include "fmt/fmt.h"
#include "number/integer_piece.h"

namespace ppi {
namespace number {

Integer::Integer() : std::vector<uint64>() {}

Integer::Integer(uint64 value) {
  resize(1);
  (*this)[0] = value;
}

void Integer::Normalize() {
  int64 i = size() - 1;
  while (i >= 0 && at(i) == 0) {
    --i;
  }
  ++i;
  erase(begin() + i, end());
}

// static
void Integer::Add(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  c->resize(std::max(a.size(), b.size()));
  IntegerPiece cp(*c);
  uint64 carry = IntegerPiece::Add(IntegerPiece(a.data(), na), IntegerPiece(b.data(), nb), &cp);
  if (carry) {
    c->push_back(carry);
  }
  c->Normalize();
}

// static
void Integer::Subtract(const Integer& a, const Integer& b, Integer* c) {
  const int64 na = a.size();
  const int64 nb = b.size();
  c->resize(a.size());
  IntegerPiece cp(*c);
  IntegerPiece::Subtract(IntegerPiece(a.data(), na), IntegerPiece(b.data(), nb), &cp);
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
  const int64 n = MinPow2(na + nb) / 2;
  c->resize(2 * n);
  IntegerPiece cp(*c);
  double err = IntegerPiece::Mult(IntegerPiece(a.data(), na), IntegerPiece(b.data(), nb), &cp);
  c->Normalize();

  return err;
}

void Integer::Mult(const Integer& a, const uint64 b, Integer* c) {
  const int64 na = a.size();
  c->resize(a.size());
  IntegerPiece cp(*c);
  uint64 carry = IntegerPiece::Mult(IntegerPiece(a.data(), na), b, &cp);
  if (carry) {
    c->push_back(carry);
  }
}

void Integer::Div(const Integer& a, const uint64 b, Integer* c) {
  const int64 na = a.size();
  c->resize(a.size());
  IntegerPiece cp(*c);
  IntegerPiece::Div(IntegerPiece(a.data(), na), b, &cp);
  c->Normalize();
}

Integer& Integer::operator=(uint64 a) {
  this->resize(1);
  (*this)[0] = a;
  return (*this);
}

std::ostream& operator<<(std::ostream& os, const Integer& val) {
  os << IntegerPiece(val);
  return os;
}

}  // namespace number
}  // namespace ppi
