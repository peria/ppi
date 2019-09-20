#pragma once

#include "base/base.h"
#include "number/natural.h"

namespace ppi {
namespace internal {

enum MultAlgorithm : uint8 {
  kBasic = 1 << 0,
  kKaratsuba = 1 << 1,
  kToomCook = 1 << 2,
  kFFT = 1 << 3,
  kAll = kBasic | kKaratsuba | kToomCook | kFFT,
};

Digit add(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);
Digit subtract(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);
void mult(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c,
          MultAlgorithm=kAll);
Digit mult(const Digit* a, const int64 na, const Digit b, Digit* c);
Digit div(const Digit* a, const int64 na, const Digit b, Digit* c);

// Algorithms in multiplication
void mult_basic(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c);

std::ostream& operator<<(std::ostream& os, const Natural& a);

}  // namespace internal
}  // namespace ppi

