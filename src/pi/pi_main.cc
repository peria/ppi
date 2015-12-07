#include <iostream>
#include <string>

#include "base/base.h"
#include "pi/pi.h"
#include "number/real.h"

DEFINE_int32(type, 0, "0:Chudnovsky, 1:arctan");
DEFINE_int64(digits, 100, "Number of hexadeciaml digits to compute");
DEFINE_string(refer, "", "file name which has another computing result");

using namespace ppi;

int main(int, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

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
  if (!FLAGS_refer.empty()) {
    int64 size = pi.size() * 16;
    int64 same = pi.Compare(FLAGS_refer);
    std::cerr << same << " out of " << size << " digits are same.\n";
  }
  
  return 0;
}
