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
  // Compute NTT of |a|, assuming each element of |a| is a long precision number
  // of length |n|, and the size of |a| is also |n|.
  static void Transfer(const Direction dir, const int64 n, uint64* a);

 private:
  static void Forward(const int64 n, uint64* a);
  static void Backward(const int64 n, uint64* a);

  // Shift |a| for |w| words to left in nega-cyclic ring of size |n|.
  static void ShiftLeftWords(const uint64* a, const int64 w, const int64 n, uint64* b);
  // Shift |a| for |k| bits to right in nega-cyclic ring of size |n|.
  static void ShiftRightBits(const uint64* a, const int64 k, const int64 n, uint64* b);
};

}  // namespace fmt
}  // namespace ppi
