#include "bbp/bbp.h"

#include <vector>
#include <glog/logging.h>

#include "number/montgomery.h"
#include "number/natural.h"

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

std::vector<uint64> Div(uint64 a, uint64 b, const int64 n) {
  std::vector<uint64> r(n);
  number::Natural::Div(a, b, n, r.data());
  return r;
}

void Add(std::vector<uint64>& val, const std::vector<uint64>& rval) {
  uint64 carry = 0;
  for (int64 i = 0; i < kLength; ++i) {
    uint64 sum = val[i] + carry;
    carry = (sum < val[i]);  // 0 or 1
    val[i] = sum + rval[i];
    if (val[i] < rval[i])
      carry = 1;
  }
}

void Subtract(std::vector<uint64>& val, const std::vector<uint64>& rval) {
  uint64 borrow = 0;
  for (int64 i = 0; i < kLength; ++i) {
    uint64 sum = val[i] - borrow;
    borrow = (sum > val[i]);  // 0 or 1
    val[i] = sum - rval[i];
    if (val[i] > sum)
      borrow = 1;
  }
}

std::vector<uint64> ComputeTerm(const Term& term, int64 bit_shift) {
  std::vector<uint64> ret(kLength, 0);

  // Integer
  const int64 integer_n = (bit_shift + term.a) / term.b;
  for (int64 i = 0; i < integer_n; ++i) {
    int64 shift = bit_shift + term.a - i * term.b;
    const uint64 mod = term.c * i + term.d;
    uint64 rem = number::Montgomery::Power(2, shift, mod);
    // q = (rem << (kLength*64)) / mod
    std::vector<uint64> q = Div(rem, mod, kLength);
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(ret, q);
    else
      Add(ret, q);
  }

  // < 1
  const int64 zero_n = (bit_shift + term.a + 64 * kLength) / term.b;
  for (int64 i = integer_n; i < zero_n; ++i) {
    int64 shift = bit_shift + term.a + 64 * kLength - i * term.b;
    DCHECK_GE(shift, 0);
    const uint64 mod = term.c * i + term.d;
    std::vector<uint64> q(kLength, 0);
    q[shift / 64] = 1ULL << (shift % 64);
    // q = (1 << shift) / mod
    number::Natural::Div(q.data(), mod, kLength, q.data());
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(ret, q);
    else
      Add(ret, q);
  }

  return ret;
}

void Dump(const std::vector<uint64>& x) {
  for (int i = x.size() - 1; i >= 0; --i) {
    printf("%016lX ", x[i]);
  }
  puts("");
}

}  // namespace

Bbp::Bbp(const Formula& formula)
    : formula_(formula) {
  (void)(formula_);
}

std::vector<uint64> Bbp::compute(int64 hex_index) {
  using Sign = Term::Sign;
  using Flip = Term::Flip;
  std::vector<Term> terms {
    {2, 4, 8, 1, Sign::kPositive, Flip::kNoFlip},  // (4/16^k)/(8k+1)
    {-1, 4, 2, 1, Sign::kNegative, Flip::kNoFlip},  // (-2/16^k)/(8k+4)
    {0, 4, 8, 5, Sign::kNegative, Flip::kNoFlip},  // (-1/16^k)/(8k+5)
    {-1, 4, 4, 3, Sign::kNegative, Flip::kNoFlip},  // (-1/16^k)/(8k+6)
  };

  const int64 bit_shift = (hex_index - 1) * 4;
  std::vector<uint64> ret(kLength, 0);
  for (const auto& term : terms) {
    DCHECK_EQ(term.d % 2, 1);
    std::vector<uint64> v = ComputeTerm(term, bit_shift);
    Dump(v);
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