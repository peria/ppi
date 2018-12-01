#pragma once

#include "base/base.h"
#include "base/complex.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in complex field.
class Dft {
 public:
  Dft(const int64 n);

  // Compute DFT of |a|.
  void Transform(const Fmt::Direction, Complex* a) const;

 private:
  void radix2(const int64 height,
              const Complex* table,
              Complex* x,
              Complex* y) const;
  void radix4(const int64 width,
              const int64 height,
              const Complex* table,
              Complex* x,
              Complex* y) const;
  void radix8(const int64 width,
              const int64 height,
              const Complex* table,
              Complex* x,
              Complex* y) const;

  const int64 n_;
  const int64 log2n_;
  const int64 log4n_;
  const int64 log8n_;
  const Complex* table_;
};

}  // namespace fmt
}  // namespace ppi
