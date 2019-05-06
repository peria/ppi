#pragma once

#include "number/natural.h"

namespace ppi {

// If the corresponding Natural interface represents n,
// the Float interface represents n * Base^exponent or -n * Base^exponent.
class Float : public Natural {
public:
  enum Sign { kPositive, kNegative };

private:
  int64 exponent_ = 0;
  Sign sign_ = kPositive;
};

}  // namespace ppi
