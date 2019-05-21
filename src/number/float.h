#pragma once

#include "number/natural.h"

namespace ppi {

// If the corresponding Natural interface represents n,
// the Float interface represents n * Base^exponent or -n * Base^exponent.
class Float : public Natural {
public:
  enum Sign { kPositive, kNegative };
  Float() = default;
  Float(Natural&& n);
  Float(Float& f);

  void resize(const int64 length);
  int64 exponent() const { return exponent_; }
  Sign sign() const { return sign_; }

  static Float&& iSqrt(Digit d, const int64 size);

private:
  int64 exponent_ = 0;
  Sign sign_ = kPositive;
};

Float Inverse(Float&& x, const int64 length);

}  // namespace ppi
