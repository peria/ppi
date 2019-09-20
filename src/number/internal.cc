#include "number/internal.h"

namespace ppi {
namespace internal {

void mult(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c,
          MultAlgorithm /*usables*/) {
  mult_basic(a, na, b, nb, c);
}

}  // namespace internal
}  // namespace ppi
