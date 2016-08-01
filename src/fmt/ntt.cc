#include "fmt/ntt.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/allocator.h"
#include "base/base.h"
#include "fmt/fmt.h"
#include "number/integer_core.h"

namespace ppi {
namespace fmt {

void Ntt::Transfer(const Direction dir, const int64 n, uint64* a) {
  if (dir == Direction::Forward) {
    Forward(n, a);
  } else {
    Backward(n, a);
  }
}

void Ntt::Forward(const int64 n, uint64* a) {
  // n must be a power of 2.
  CHECK_EQ(0, (n & (n - 1)));

  uint64* x0 = base::Allocator::Allocate<uint64>(n);
  uint64* x1 = base::Allocator::Allocate<uint64>(n);
  uint64* x2 = base::Allocator::Allocate<uint64>(n);
  for (int64 m = n / 2; m >= 1; m /= 2) {
    int64 shift = n / 2 / m;
    for (int64 j = 0; j < n; j += 2 * m) {
      for (int64 k = 0; k < m; ++k) {
        int64 w = (shift * k) % n;
        std::copy_n(&a[(j + k) * n], n, x0);
        std::copy_n(&a[(j + m + k) * n], n, x1);
        number::IntegerCore::Add(x0, x1, n, &a[(j + k) * n]);
        number::IntegerCore::Subtract(x0, x1, n, x2);
        ShiftLeftWords(x2, w, n, &a[(j + k + m) * n]);
      }
    }
  }

  base::Allocator::Deallocate(x0);
  base::Allocator::Deallocate(x1);
  base::Allocator::Deallocate(x2);
}

void Ntt::Backward(const int64 /*n*/, uint64* /*a*/) {
}

void Ntt::ShiftLeftWords(const uint64* a, const int64 w, const int64 n, uint64* b) {
  for (int64 i = 0, j = w; j < n; ++i, ++j) {
    b[j] = a[i];
  }
  for (int64 i = n - w, j = 0; i < n; ++i, ++j) {
    b[j] = ~a[i];
  }

  ++b[0];
  for (int64 i = 0; i < w; ++i) {
    if (b[i])
      break;
    ++b[i + 1];
  }

  if (w != 0) {
    --b[w];
  }
}

void Ntt::ShiftRightBits(const uint64* a, const int64 k, const int64 n, uint64* b) {
  DCHECK_LT(0, k);
  DCHECK_LT(k, 64);

  const uint64 mask = (1ULL << k) - 1;
  uint64 word = a[0];
  uint64 add = (-word) & mask;
  word = (word >> k) + (add ? 1 : 0);
  // TODO: check if |word| carrys up.

  for (int64 i = 0; i < n - 1; ++i) {
    uint64 next = a[i + 1];
    b[i] = (next << (64 - k)) + word;
    word = next >> k;
  }
  b[n - 1] = (add << (64 - k)) + word;
}

}  // namespace fmt
}  // namespace ppi
