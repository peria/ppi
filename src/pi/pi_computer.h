#pragma once

#include "base/base.h"
#include "number/natural.h"

namespace ppi {

class PiComputer {
public:
  virtual void compute(int64 num_dec_digits, Natural& pi) = 0;
};

}  // namespace ppi
