#include "computer/computer.h"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <memory>

#include "base/base.h"
#include "drm/chudnovsky.h"
#include "gflags/gflags.h"

namespace ppi {

void Computer::Parameter::ParseInitFile() {
  // TODO: Parse .ppirc file
}

void Computer::Parameter::ParseCommandLine(int argc, char** argv) {
  if (argc >= 2 && argv[1]) {
    decimal_digits = std::strtoll(argv[1], nullptr, 10);
  }
}

bool Computer::Parameter::IsValid() const {
  return true;
}

std::unique_ptr<Computer> Computer::Create(const Parameter& parameter) {
  // TODO: Implement more algorithms and formulae.
  return drm::Chudnovsky::Create(parameter);
}

void Computer::Run() {
  // Data input
  Real v = Compute();
  // Output(v);
}

}  // namespace ppi
