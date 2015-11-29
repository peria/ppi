#include <iostream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"
#include "pi/pi.h"
#include "number/real.h"

DEFINE_int32(type, 0, "0:Chudnovsky, 1:arctan");
DEFINE_int64(digits, 100, "Number of hexadeciaml digits to compute");

using namespace ppi;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  int64 limbs = FLAGS_digits / 16 + 1;
  ppi::number::Real pi;
  pi.setPrecision(limbs);
  switch (FLAGS_type) {
  case 0:
    ppi::pi::Pi::Chudnovsky(&pi);
    break;
  case 1:
    ppi::pi::Pi::Arctan2(&pi);
    break;
  }
  std::cout << pi << "\n";
  
  return 0;
}
