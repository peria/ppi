#pragma once

#include "base/base.h"
#include "base/allocator.h"
#include "number/natural.h"

#include <ostream>

namespace ppi {

class Float : public Natural {
public:
  Float();
  ~Float() = default;

  int64 sign() const { return sign_; }
  int64 exponent() const { return exponent_; }
  int64 precision() const { return precision_; }

  void setPrecision(int64 p) { precision_ = p; }
  void normalize();

  // Operations including memory size management.
  static void add(const Float& a, const Float& b, Float& c);
  static void subtract(const Float& a, const Float& b, Float& c);
  static double mult(const Float& a, const Float& b, Float& c);
  static int cmpAbs(const Float& a, const Float& b);
  static void inverse(const Float& a, Float& c);

protected:
  int64 sign_ = 1;
  int64 exponent_ = 0;
  int64 precision_ = 0;
};

std::ostream& operator<<(std::ostream&, const Float&);

}  // namespace ppi
