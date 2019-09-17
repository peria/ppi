#pragma once

#include <vector>

#include "base/base.h"
#include "pi/computer.h"

namespace ppi {

class Arctan : public Computer {
public:
  struct Formula {
    const char* name;
    std::vector<std::pair<int64, int64>> terms;
  };

  Arctan(int formula_id);

  void compute(int64 num_dec_digits, Natural& pi) override;
  const char* name() const override { return "arctan"; }
  const char* formula() const override { return formula_.name; }

private:
  const Formula& formula_;
};

}  // namespace ppi
