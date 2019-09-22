#include "number/internal.h"

#include <memory>

#include "base/allocator.h"

namespace ppi {
namespace internal {

namespace {

constexpr int64 kThresholdKaratsuba = 80;

}  // namespace

void mult(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c,
          MultAlgorithm usables) {
  if (na < nb) {
    mult(b, nb, a, na, c);
    return ;
  }

  if ((usables & kKaratsuba) && na + nb > kThresholdKaratsuba) {
    mult_karatsuba(a, na, b, nb, c);
    return;
  }
  mult_basic(a, na, b, nb, c);
}

void mult_karatsuba(const Digit* a, const int64 na, const Digit* b, const int64 nb, Digit* c) {
  const int64 n = (na + 1) / 2;

  if (nb <= n) {
    mult(a + n, na - n, b, nb, c + n);
    Digit* a0 = base::Allocator::allocate<Digit>(n + nb);
    mult(a, n, b, nb, a0);
    for (int64 i = 0; i < n; ++i)
      c[i] = a0[i];
    add(c + n, na + nb - n, a0 + n, nb, c + n);
    return;
  }

  Digit* c00 = c;
  Digit* c11 = c + 2 * n;

  mult(a, n, b, n, c00);
  mult(a + n, na - n, b + n, nb - n, c11);

  Digit* a01 = base::Allocator::allocate<Digit>(n + 1);
  a01[n] = add(a, n, a + n, na - n, a01);
  int64 na1 = n + (a01[n] != 0);

  Digit* b01 = base::Allocator::allocate<Digit>(n + 1);
  b01[n] = add(b, n, b + n, nb - n, b01);
  int64 nb1 = n + (b01[n] != 0);

  Digit* ab = base::Allocator::allocate<Digit>(na1 + nb1);

  int64 nab = na1 + nb1;
  mult(a01, na1, b01, nb1, ab);

  subtract(ab, nab, c00, 2 * n, ab);
  subtract(ab, nab, c11, na + nb - 2 * n, ab);

  CHECK(add(c + n, na + nb - n, ab, nab, c + n) == 0);

  base::Allocator::deallocate(a01);
  base::Allocator::deallocate(b01);
  base::Allocator::deallocate(ab);
}


}  // namespace internal
}  // namespace ppi
