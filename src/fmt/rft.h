#pragma once

#include "base/base.h"
#include "fmt/dft.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in real number field.
// (Actually, this is a proxy between real and complex number fields
// to compute DFTs)
class Rft : public Dft {
 public:
  Rft(const int64 n);

  // Compute DFT of |a|.
  void Transform(const Direction dir, double* a) const;

 private:
  const int64 n_;
};

}  // namespace fmt
}  // namespace ppi
