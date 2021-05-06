#pragma once

#include "number/number.h"

namespace ppi {

// Integer class represents an unsigned (=non negative) integer in high
// precision format.
class Integer : public Number {
 public:
  Integer() = default;
  Integer(const Integer&) = default;
  Integer(Integer&&) = default;
  Integer(Digit);

  Integer& operator=(const Integer&) = default;
  Integer& operator=(Integer&&) = default;

  Integer& operator*=(Integer&);
  Integer& operator*=(Digit&);
};

}  // namespace ppi
