#include "bbp/bbp_gpu.h"

#include <array>
#include <vector>

#include "number/number.h"
#include "number/natural.h"

namespace ppi {

BbpGpu::BbpGpu(const Formula& formula) : Bbp(formula) {}

void BbpGpu::computeIntegralTerm(const Term& term, int64 bit_shift, int64 from,
                              int64 to, uint64* ret) const {
  uint64 value[kLength] {};
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

}  // namespace ppi
