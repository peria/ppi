#pragma once

#include "base/base.h"
#include "base/complex.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in complex field.
class Dft {
  struct Setting {
    Setting(int64 n);
    ~Setting();

    int64 n;
    int64 log2n;
    int64 log4n;
    int64 log8n;
    Complex* table;
  };

 public:
  Dft(const int64 n);

  // Compute DFT of |a|.
  void Transform(const Direction, Complex* a) const;

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

  const Setting setting_;
};

}  // namespace fmt
}  // namespace ppi
