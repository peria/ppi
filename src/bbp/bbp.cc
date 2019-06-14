#include "bbp/bbp.h"

#include <glog/logging.h>

#include <array>
#include <ostream>
#include <vector>

#include "base/macros.h"
#include "base/timer.h"
#include "number/natural.h"
#include "number/number.h"

namespace ppi {

std::ostream& operator<<(std::ostream& ost, const Bbp::ProcessType& pt) {
  switch (pt) {
  case Bbp::ProcessType::kCpu:
    return ost << "CPU";
  case Bbp::ProcessType::kGpu:
    return ost << "GPU";
  }

  NOTREACHED();
  // Otherwise, ignore
  return ost;
}

Bbp::Bbp(const Formula& formula)
    : process_type_(ProcessType::kCpu), formula_(formula) {
  LOG(INFO) << "BBP in " << process_type_ << " is enabled";
}

std::vector<uint64> Bbp::compute(int64 hex_index) const {
  base::Timer all_timer;
  const int64 bit_index = hex_index * 4 - 3;
  std::vector<uint64> ret(kLength, 0);
  for (const auto& term : getTerms()) {
    base::Timer term_timer;
    DCHECK_EQ(term.d % 2, 1);
    std::vector<uint64> v(kLength, 0);
    computeTerm(term, bit_index, v.data());
    switch (term.sign) {
    case Term::Sign::kPositive:
      Add(ret.data(), v.data());
      break;
    case Term::Sign::kNegative:
      Subtract(ret.data(), v.data());
      break;
    }
    term_timer.Stop();
    VLOG(1) << "per term time: " << term_timer.GetTimeInSec() << " src";
  }
  all_timer.Stop();
  LOG(INFO) << "total time: " << all_timer.GetTimeInSec() << " src";

  return ret;
}

void Bbp::computeTerm(const Term& term, int64 bit_index, uint64* ret) const {
  CHECK_GE(bit_index, 1);

  const int64 bit_shift = bit_index - 1 + term.a;
  const int64 integer_n =
      (bit_shift - (bit_shift % term.b + term.b) % term.b) / term.b + 1;
  const int64 zero_n = (bit_shift + 64 * kLength) / term.b;
  VLOG(1) << "Compute terms: " << integer_n << " + " << (zero_n - integer_n);

  // Where 2^(A-k*B) is integral
  computeIntegralTerm(term, bit_shift, 0, integer_n, ret);

  // Where (2^A/2^(k*B)) < 1. This part is very short.
  uint64 q[kLength];
  for (int64 i = integer_n; i <= zero_n; ++i) {
    int64 shift = bit_shift + 64 * kLength - i * term.b;
    DCHECK_GE(shift, 0);
    const uint64 mod = term.c * i + term.d;
    for (int64 i = 0; i < kLength; ++i)
      q[i] = 0;
    q[shift / 64] = 1ULL << (shift % 64);
    // q = (1 << shift) / mod
    number::Natural::Div(q, mod, kLength, q);
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(ret, q);
    else
      Add(ret, q);
  }
}

void Bbp::computeIntegralTerm(const Term& term,
                              int64 bit_shift,
                              int64 from,
                              int64 to,
                              uint64* ret) const {
  uint64 value[kLength]{};
  uint64 q[kLength];
  for (int64 i = from; i < to; ++i) {
    int64 shift = bit_shift - i * term.b;
    const uint64 mod = term.c * i + term.d;
    uint64 rem = number::Power(2, shift, mod);
    number::Natural::Div(rem, mod, kLength, q);
    if (term.flip == Term::Flip::kFlip && i % 2 == 1)
      Subtract(value, q);
    else
      Add(value, q);
  }

  Add(ret, value);
}

std::vector<Bbp::Term> Bbp::getTerms() const {
  using Sign = Term::Sign;
  using Flip = Term::Flip;

  switch (formula_) {
  case Bbp::Formula::kBbp:
    LOG(INFO) << "Use BBP formula";
    return {
        {2, 4, 8, 1, Sign::kPositive, Flip::kNoFlip},   //  2^(-4k+2)/(8k+1)
        {-1, 4, 2, 1, Sign::kNegative, Flip::kNoFlip},  // -2^(-4k-1)/(2k+1)
        {0, 4, 8, 5, Sign::kNegative, Flip::kNoFlip},   // -2^(-4k  )/(8k+5)
        {-1, 4, 4, 3, Sign::kNegative, Flip::kNoFlip},  // -2^(-4k-1)/(4k+3)
    };
  case Bbp::Formula::kBellard:
    LOG(INFO) << "Use Bellard's formula";
    return {
        {-1, 10, 4, 1, Sign::kNegative,
         Flip::kFlip},  // -(-1)^k 2^(-10k-1)/(4k+1)
        {-6, 10, 4, 3, Sign::kNegative,
         Flip::kFlip},  // -(-1)^k 2^(-10k-6)/(4k+3)
        {2, 10, 10, 1, Sign::kPositive,
         Flip::kFlip},  //  (-1)^k 2^(-10k+2)/(10k+1)
        {0, 10, 10, 3, Sign::kNegative,
         Flip::kFlip},  // -(-1)^k 2^(-10k  )/(10k+3)
        {-4, 10, 10, 5, Sign::kNegative,
         Flip::kFlip},  // -(-1)^k 2^(-10k-4)/(10k+5)
        {-4, 10, 10, 7, Sign::kNegative,
         Flip::kFlip},  // -(-1)^k 2^(-10k-4)/(10k+7)
        {-6, 10, 10, 9, Sign::kPositive,
         Flip::kFlip},  //  (-1)^k 2^(-10k-6)/(10k+9)
    };
  }

  CHECK(false);
  return {};
}

// static
void Bbp::Add(uint64* val, const uint64* rval) {
  uint64 carry = 0;
  for (int64 i = 0; i < kLength; ++i) {
    uint64 sum = val[i] + carry;
    carry = (sum < val[i]);  // 0 or 1
    val[i] = sum + rval[i];
    if (val[i] < rval[i])
      carry = 1;
  }
}

// static
void Bbp::Subtract(uint64* val, const uint64* rval) {
  uint64 borrow = 0;
  for (int64 i = 0; i < kLength; ++i) {
    uint64 sum = val[i] - borrow;
    borrow = (sum > val[i]);  // 0 or 1
    val[i] = sum - rval[i];
    if (val[i] > sum)
      borrow = 1;
  }
}

}  // namespace ppi
