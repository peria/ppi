#include "number/internal.h"

namespace ppi {
namespace internal {
namespace {

constexpr int64 kThresholdKaratsuba = 10000000;
constexpr int64 kThresholdToomCook = 10000000;
constexpr int64 kThresholdFFT = 10000000;

}  // namespace

void mult(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c,
          MultAlgorithm usables) {
  if ((usables & kKaratsuba) && na + nb > kThresholdKaratsuba) {
    mult_karatsuba(a, na, b, nb, c);
    return;
  }
  mult_basic(a, na, b, nb, c);
}

}  // namespace internal
}  // namespace ppi
