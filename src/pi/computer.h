#pragma once

#include "base/base.h"
#include "number/natural.h"

namespace ppi {

class Computer {
public:
  virtual ~Computer() = default;

  virtual void compute(int64 num_dec_digits, Natural& pi) = 0;
  virtual const char* name() const = 0;
};

}  // namespace ppi
