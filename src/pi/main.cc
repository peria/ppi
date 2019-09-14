#include <cstdlib>
#include <iostream>
#include <memory>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"
#include "number/natural.h"
#include "pi/arctan.h"
#include "pi/pi_computer.h"

DEFINE_string(computer, "arctan", "Basic algorithm to compute pi.");
DEFINE_int32(formula, 0, "ID of a formula, if the algorithm works for.");

namespace {

ppi::PiComputer* GetComputer() {
  if (FLAGS_computer == "arctan") {
    return new ppi::Arctan(FLAGS_formula);
  }
  return new ppi::Arctan(0);
}

}  // namespace

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  ppi::Natural pi;
  std::unique_ptr<ppi::PiComputer> computer(GetComputer());
  ppi::int64 num_dec_digits = std::strtoll(argv[1], nullptr, 10);
  computer->compute(num_dec_digits, pi);

  std::cout << pi << "\n";

  return 0;
}
