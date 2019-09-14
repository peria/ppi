#pragma once

#include <vector>

#include "base/base.h"
#include "pi/computer.h"

namespace ppi {

class ArcTan : public Computer {
public:
  struct Formula {
    const char* name;
    std::vector<std::vector<int64>> terms;
  };

  ArcTan(int formula_id);

  void compute(int64 num_dec_digits, Natural& pi) override;

private:
  const Formula& formula_;
};

}  // namespace ppi
