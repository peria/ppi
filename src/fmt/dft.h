#pragma once

#include "base/base.h"
#include "base/complex.h"
#include "fmt/fmt.h"

namespace ppi {
namespace fmt {

// Computes DFT in complex field.
class Dft {
  struct Setting {
    enum class Axis { kFirst, kLast };
    Setting(int64 n, const Axis axis = Axis::kLast);
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
  static void kernel(const Setting& setting, Complex* work, Complex* a);

  const Setting setting1_;
  const Setting setting2_;
};

}  // namespace fmt
}  // namespace ppi
