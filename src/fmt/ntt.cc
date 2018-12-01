#include "fmt/ntt.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/allocator.h"
#include "base/base.h"
#include "fmt/fmt.h"
#include "number/natural.h"

namespace ppi {
namespace fmt {

namespace {

inline uint64* element(uint64* a, const int64 n, int64 id) {
  return &a[(n + 1) * id];
}

}  // namespace

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

  // We assume that the primitive root w is to shift 2 words, so we compute in
  // a ring N/(w^(n/2)+1)N.  It means that the size of each element is also
  // |n|, if the number of elements is |n|.

  uint64* x0 = base::Allocator::Allocate<uint64>(n + 1);
  uint64* x1 = base::Allocator::Allocate<uint64>(n + 1);
  uint64* x2 = base::Allocator::Allocate<uint64>(n + 1);

  for (int64 m = n / 2; m >= 1; m /= 2) {
    int64 shift = n / 2 / m;
    for (int64 j = 0; j < n; j += 2 * m) {
      for (int64 k = 0; k < m; ++k) {
        int64 w = (shift * k) & (n - 1);
        std::copy_n(element(a, n, j + k), n, x0);
        std::copy_n(element(a, n, j + m + k), n, x1);
        number::Natural::Add(x0, x1, n, element(a, n, j + k));
        number::Natural::Subtract(x0, x1, n, x2);
        ShiftLeftWords(x2, w, n, element(a, n, j + k + m));
      }
    }
  }

  base::Allocator::Deallocate(x0);
  base::Allocator::Deallocate(x1);
  base::Allocator::Deallocate(x2);
}

void Ntt::Backward(const int64 n, uint64* a) {
  // n must be a power of 2.
  CHECK_EQ(0, (n & (n - 1)));

  uint64* x0 = base::Allocator::Allocate<uint64>(n + 1);
  uint64* x1 = base::Allocator::Allocate<uint64>(n + 1);

  for (int64 m = 1; m < n; m *= 2) {
    int64 shift = -(n / 2 / m);
    for (int64 j = 0; j < n; j += 2 * m) {
      for (int64 k = 0; k < m; ++k) {
        int64 w = (shift * k) & (n - 1);
        std::copy_n(element(a, n, j + k), n, x0);
        ShiftLeftWords(element(a, n, j + k + m), w, n, x1);
        number::Natural::Add(x0, x1, n, element(a, n, j + k));
        number::Natural::Subtract(x0, x1, n, element(a, n, j + k + m));
      }
    }
  }

  int64 k = 0;
  while ((1LL << k) < n)
    ++k;

  for (int64 i = 0; i < n; ++i) {
    std::copy_n(element(a, n, i), n, x0);
    ShiftRightBits(x0, k, n, element(a, n, i));
  }

  base::Allocator::Deallocate(x0);
  base::Allocator::Deallocate(x1);
}

void Ntt::Add(const uint64* a, const uint64* b, const int64 n, uint64* c) {
  if (number::Natural::Add(a, b, n, c) == 0)
    return;

  for (int64 i = 0; i < n; ++i) {
    --c[i];
    if (~c[i])
      break;
  }
}

void Ntt::Subtract(const uint64* a, const uint64* b, const int64 n, uint64* c) {
  if (number::Natural::Add(a, b, n, c) == 0)
    return;

  for (int64 i = 0; i < n; ++i) {
    ++c[i];
    if (c[i])
      break;
  }
}

void Ntt::ShiftLeftWords(const uint64* a,
                         const int64 w,
                         const int64 n,
                         uint64* b) {
  DCHECK_LT(w, 2 * n);

  if (w == 0) {
    std::memcpy(b, a, sizeof(uint64) * (n + 1));
    return;
  }

  // TODO: Remove this edge case handling.
  if (a[n] == 1) {
    if (w < n) {
      b[0] = 1;
      for (int64 i = 1; i < w; ++i)
        b[i] = 0;
      for (int64 i = w; i < n; ++i)
        b[i] = ~0ULL;
      b[n] = 0;
      return;
    }
    for (int64 i = 0; i <= n; ++i)
      b[i] = 0;
    b[w - n] = 1;
    return;
  }

  b[n] = 0;
  if (w < n) {
    for (int64 i = 0, j = w; j < n; ++i, ++j)
      b[j] = a[i];
    for (int64 i = n - w, j = 0; i < n; ++i, ++j)
      b[j] = ~a[i];
    for (int64 i = 0; i <= n; ++i) {
      ++b[i];
      if (b[i])
        break;
    }
    for (int64 i = w; i <= n; ++i) {
      --b[i];
      if (~b[i])
        break;
    }
    if (~b[n] == 0) {
      b[n] = 0;
      for (int64 i = 0; i <= n; ++i) {
        ++b[i];
        if (b[i])
          break;
      }
    }
  } else {
    for (int64 i = 0, j = w - n; j < n; ++i, ++j)
      b[j] = ~a[i];
    for (int64 i = 2 * n - w, j = 0; i < n; ++i, ++j)
      b[j] = a[i];
    for (int64 i = w - n; i <= n; ++i) {
      ++b[i];
      if (b[i])
        break;
    }
    for (int64 i = 0; i <= n; ++i) {
      ++b[i];
      if (b[i])
        break;
    }
  }
}

void Ntt::ShiftRightBits(const uint64* a,
                         const int64 k,
                         const int64 n,
                         uint64* b) {
  DCHECK_LT(0, k);
  DCHECK_LT(k, 64);

  const uint64 mask = (1ULL << k) - 1;
  uint64 word = a[0];
  uint64 add = (-word) & mask;
  word = (word >> k) + (add ? 1 : 0);

  for (int64 i = 0; i < n - 1; ++i) {
    uint64 next = a[i + 1];
    b[i] = (next << (64 - k)) + word;
    word = next >> k;
  }
  b[n - 1] = (add << (64 - k)) + word;
}

}  // namespace fmt
}  // namespace ppi
