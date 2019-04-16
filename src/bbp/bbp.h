#pragma once

#include <vector>
#include "base/base.h"

namespace ppi {

class Bbp {
  struct Term {
    enum class Sign : uint8 {
      kPositive,
      kNegative,
    };
    enum class Flip : uint8 {
      kNoFlip,
      kFlip,
    };

    // Represents T[k]
    //   T[k] = t[k]    if sign==kPositive
    //          -t[k]   if sign==kNegative
    //   t[k] = \sum_k 2^(a-b*k)/(c*k+d)          if flip==kNoFlip
    //          \sum_k (-1)^k 2^(a-b*k)/(c*k+d)   if flip==kFlip
    int64 a, b, c, d;
    Sign sign;
    Flip flip;
  };

public:
  enum class Formula {
    kBbp,
    kBellard,
  };
  static const int64 kLength;

  Bbp(const Formula& formula);

  // Compute a part of pi, 64*4 bits from |hex_index|-th hexadecimal digit
  // after the decimal point.
  std::vector<uint64> compute(int64 hex_index) const;

private:
  // Compute |bit_index|-th bit of a term |term| using BBP algorithm.
  void computeTerm(const Term& term, int64 bit_index, uint64* value) const;
  // Compute a part of BBP term |term| wich shifting |bit_shift|, from |from|-th
  // to |to|-th index.
  void computeIntegralTerm(const Term& term, int64 bit_shift, int64 from,
                           int64 to, uint64* value) const;
  std::vector<Term> getTerms() const;

  // |val| += |rval|
  static void Add(uint64* val, const uint64* rval);
  // |val| -= |rval|
  static void Subtract(uint64* val, const uint64* rval);

  const Formula formula_;
};

}  // namespace ppi
