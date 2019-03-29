#include "bbp/bbp.h"

#include <vector>

#include "number/montgomery.h"

namespace ppi {

namespace {

// Represents (-1)^sign * \sum_k (-1)^k 2^(a-b*k)/(c*k+d)
struct Term {
  enum class Sign : uint8 {
    kPositive,
    kNegative,
  };
  enum class Flip : uint8 {
    kNoFlip,
    kFlip,
  };

  int64 a;
  int64 b;
  int64 c;
  int64 d;  // d must be an odd number.
  Sign sign;
  Flip flip;
};

constexpr int64 kLength = 4;  // 4 * 64bit

std::vector<uint64> ComputeTerm(const Term& term, int64 bit_shift) {
  const int64 integer_n = (bit_shift + term.a) / term.b;
  // Integer
  for (int64 i = 0; i < integer_n; ++i) {
    int64 shift = bit_shift + term.a - i * term.b;
    const uint64 mod = term.c * i + term.d;
    uint64 rem = Montgomery::Power(2, shift, mod);

    // q = (rem << (kLength*64)) / mod
    std::vector<uint64> q = Div(rem, mod, kLength);
  }
  // < 1
}

}  // namespace

Bbp::Bbp(const Formula& formula)
    : formula_(formula) {}

std::vector<uint64> Bbp::compute(int64 hex_index) {
  std::vector<Term> terms {
    {2, 4, 8, 1, Sign::kPositive},  // (4/16^k)/(8k+1)
    {-1, 4, 2, 1, Sign::kNegative},  // (-2/16^k)/(8k+4)
    {0, 4, 8, 5, Sign::kNegative},  // (-1/16^k)/(8k+5)
    {-1, 4, 4, 3, Sign::kNegative},  // (-1/16^k)/(8k+6)
  };

  const int64 bit_shift = (hex_index - 1) * 4;
  std::vector<uint64> ret(kLength, 0);
  for (const auto& term : terms) {
    DCHECK_EQ(term.d % 2, 1);
    std::vector<uint64> v = ComputeTerm(term, bit_shift);
    switch (term.sign) {
    case Sign::kPositive:
      Add(ret, v);
      break;
    case Sign::kNegative:
      Subtract(ret, v);
      break;
    }
  }

  return ret;
}

}