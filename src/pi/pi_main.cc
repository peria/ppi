#include <iostream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "pi/pi.h"
#include "number/real.h"

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  ppi::number::Real pi;
  pi.setPrecision(10);
  ppi::pi::Pi::Arctan2(&pi);
  std::cout << pi << "\n";
  
  return 0;
}
