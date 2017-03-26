#pragma once

#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in real number field.
// (Actually, this is a proxy between real and complex number fields
// to compute DFTs)
class Rft {
 public:
  // Compute DFT of |a|.
  static void Transform(const Direction dir, int64 n, double* a);
};

}  // namespace fmt
}  // namespace ppi
