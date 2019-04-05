#include "bbp/bbp.h"

#include <vector>
#include <glog/logging.h>

#include "base/timer.h"
#include "number/number.h"
#include "number/natural.h"

namespace ppi {

const int64 Bbp::kLength = 4;  // 4 * 64bit

namespace {

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

std::vector<Term> getFormula(const Bbp::Formula& formula) {
  using Sign = Term::Sign;
  using Flip = Term::Flip;

  switch (formula) {
  case Bbp::Formula::kBbp:
    return {
        { 2, 4, 8, 1, Sign::kPositive, Flip::kNoFlip},  //  2^(-4k+2)/(8k+1)
        {-1, 4, 2, 1, Sign::kNegative, Flip::kNoFlip},  // -2^(-4k-1)/(2k+1)
        { 0, 4, 8, 5, Sign::kNegative, Flip::kNoFlip},  // -2^(-4k  )/(8k+5)
        {-1, 4, 4, 3, Sign::kNegative, Flip::kNoFlip},  // -2^(-4k-1)/(4k+3)
    };
  case Bbp::Formula::kBellard:
    return {
        {-1, 10,  4, 1, Sign::kNegative, Flip::kFlip},  // -(-1)^k 2^(-10k-1)/(4k+1)
        {-6, 10,  4, 3, Sign::kNegative, Flip::kFlip},  // -(-1)^k 2^(-10k-6)/(4k+3)
        { 2, 10, 10, 1, Sign::kPositive, Flip::kFlip},  //  (-1)^k 2^(-10k+2)/(10k+1)
        { 0, 10, 10, 3, Sign::kNegative, Flip::kFlip},  // -(-1)^k 2^(-10k  )/(10k+3)
        {-4, 10, 10, 5, Sign::kNegative, Flip::kFlip},  // -(-1)^k 2^(-10k-4)/(10k+5)
        {-4, 10, 10, 7, Sign::kNegative, Flip::kFlip},  // -(-1)^k 2^(-10k-4)/(10k+7)
        {-6, 10, 10, 9, Sign::kPositive, Flip::kFlip},  //  (-1)^k 2^(-10k-6)/(10k+9)
    };
  }

  CHECK(false);
  return {};
}

std::vector<uint64> Div(uint64 a, uint64 b, const int64 n) {
  std::vector<uint64> r(n);
  number::Natural::Div(a, b, n, r.data());
  return r;
}

void Add(std::vector<uint64>& val, const std::vector<uint64>& rval) {
  uint64 carry = 0;
  for (int64 i = 0; i < Bbp::kLength; ++i) {
    uint64 sum = val[i] + carry;
    carry = (sum < val[i]);  // 0 or 1
    val[i] = sum + rval[i];
    if (val[i] < rval[i])
      carry = 1;
  }
}

void Subtract(std::vector<uint64>& val, const std::vector<uint64>& rval) {
  uint64 borrow = 0;
  for (int64 i = 0; i < Bbp::kLength; ++i) {
    uint64 sum = val[i] - borrow;
    borrow = (sum > val[i]);  // 0 or 1
    val[i] = sum - rval[i];
    if (val[i] > sum)
      borrow = 1;
  }
}

std::vector<uint64> ComputeTerm(const Term& term, int64 bit_index) {
  CHECK_GE(bit_index, 1);
  std::vector<uint64> ret(Bbp::kLength, 0);

  const int64 bit_shift = bit_index - 1 + term.a;

  const int64 integer_n = (bit_shift - (bit_shift % term.b + term.b) % term.b) / term.b;
  const int64 zero_n = (bit_shift + 64 * Bbp::kLength) / term.b;
  VLOG(1) << "Compute terms: " << integer_n << " + " << (zero_n - integer_n);

  // Where (2^A/2^(k*B)) is integral
  for (int64 i = 0; i <= integer_n; ++i) {
    int64 shift = bit_shift - i * term.b;
    const uint64 mod = term.c * i + term.d;
    uint64 rem = number::Power(2, shift, mod);
    std::vector<uint64> q = Div(rem, mod, Bbp::kLength);
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(ret, q);
    else
      Add(ret, q);
  }

  // Where (2^A/2^(k*B)) < 1
  for (int64 i = integer_n + 1; i <= zero_n; ++i) {
    int64 shift = bit_shift + 64 * Bbp::kLength - i * term.b;
    DCHECK_GE(shift, 0);
    const uint64 mod = term.c * i + term.d;
    std::vector<uint64> q(Bbp::kLength, 0);
    q[shift / 64] = 1ULL << (shift % 64);
    // q = (1 << shift) / mod
    number::Natural::Div(q.data(), mod, Bbp::kLength, q.data());
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(ret, q);
    else
      Add(ret, q);
  }

  return ret;
}

}  // namespace

Bbp::Bbp(const Formula& formula)
    : formula_(formula) {}

std::vector<uint64> Bbp::compute(int64 hex_index) const {
  const std::vector<Term> terms(getFormula(formula_));

  base::Timer all_timer;
  const int64 bit_index = hex_index * 4 - 3;
  std::vector<uint64> ret(kLength, 0);
  for (const auto& term : terms) {
    base::Timer term_timer;
    DCHECK_EQ(term.d % 2, 1);
    std::vector<uint64> v = ComputeTerm(term, bit_index);
    switch (term.sign) {
    case Term::Sign::kPositive:
      Add(ret, v);
      break;
    case Term::Sign::kNegative:
      Subtract(ret, v);
      break;
    }
    term_timer.Stop();
    LOG(INFO) << "per term time: " << term_timer.GetTimeInSec() << " src";
  }
  all_timer.Stop();
  LOG(INFO) << "total time: " << all_timer.GetTimeInSec() << " src";

  return ret;
}

}
