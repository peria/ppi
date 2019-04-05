#pragma once

#include <vector>
#include "base/base.h"

namespace ppi {

class Bbp {
public:
  enum class Formula {
    kBbp,
    kBellard,
  };
  static const int64 kLength;

  Bbp(const Formula& formula);

  // Compute a part of pi, 64*4 bits from |hex_index|-th hexadecimal digit
  // after the decimal point.
  std::vector<uint64> compute(int64 hex_index) const;

private:
  const Formula formula_;
};

}  // namespace ppi
