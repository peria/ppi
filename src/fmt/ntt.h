#pragma once

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Ntt class supports NTT (Number theorem transfer), with each element is
// a long precision number.
class Ntt {
  static const int kOmega = 2;

 public:
  // Processes NTT (number theorem transfer) on an array |a|, which has
  // |n| element.  Computations on those elements will be done in a nega-cyclic
  // ring N/(w^(n+1)+1)N, where w is 2^64.  It means each element is like
  // uint64[n+1], and carried up values will be substracted from [0].
  static void Transfer(const Direction dir, const int64 n, uint64* a);

 protected:
  static void Forward(const int64 n, uint64* a);
  static void Backward(const int64 n, uint64* a);

  // Basic operations in a ring N/(w^n+1)N.
  static void Add(const uint64* a, const uint64* b, const int64 n, uint64* c);
  static void Subtract(const uint64* a,
                       const uint64* b,
                       const int64 n,
                       uint64* c);
  // Shift |a| for |w| words to left in nega-cyclic ring of size |n|.
  static void ShiftLeftWords(const uint64* a,
                             const int64 w,
                             const int64 n,
                             uint64* b);
  // Shift |a| for |k| bits to right in nega-cyclic ring of size |n|.
  static void ShiftRightBits(const uint64* a,
                             const int64 k,
                             const int64 n,
                             uint64* b);
};

}  // namespace fmt
}  // namespace ppi
