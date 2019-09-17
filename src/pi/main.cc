#include <cstdlib>
#include <iostream>
#include <memory>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"
#include "base/timer.h"
#include "number/natural.h"
#include "pi/arctan.h"
#include "pi/computer.h"

DEFINE_string(computer, "arctan", "Basic algorithm to compute pi.");
DEFINE_int32(formula, 0, "ID of a formula, if the algorithm works for.");

namespace {

ppi::Computer* GetComputer() {
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
  std::unique_ptr<ppi::Computer> computer(GetComputer());
  ppi::int64 num_dec_digits = std::strtoll(argv[1], nullptr, 10);
  LOG(INFO) << "Digits to compute: " << num_dec_digits;
  LOG(INFO) << "Algorithm: " << computer->name();
  LOG(INFO) << "Formula: " << computer->formula();
  ppi::Timer timer;
  computer->compute(num_dec_digits, pi);
  timer.stop();

  LOG(INFO) << "Total time: " << timer.timeInSec() << " sec.";

  std::cout << pi << "\n";

  return 0;
}
