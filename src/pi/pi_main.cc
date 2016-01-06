#include <iostream>
#include <string>

#include "base/allocator.h"
#include "base/base.h"
#include "base/time.h"
#include "pi/drm.h"
#include "pi/pi.h"
#include "number/real.h"

DEFINE_int32(type, 0, "0:Chudnovsky, 1:arctan");
DEFINE_int64(digits, 100, "Number of hexadeciaml digits to compute");
DEFINE_string(refer, "", "file name which has another computing result");

using namespace ppi;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  double start = base::Time::Now();
  int64 limbs = FLAGS_digits / 16 + 1;
  ppi::number::Real pi;
  pi.setPrecision(limbs);
  switch (FLAGS_type) {
  case 0:
    ppi::pi::Drm::Chudnovsky(&pi);
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
  double end = base::Time::Now();
  LOG(INFO) << "Elapsed Time: " << (end - start) << " sec.";
  
#if !defined(BUILD_TYPE_release)
  int64 used_size = ppi::base::Allocator::allocated_size_peak();
  double used_size_kib = used_size / 1024.0;
  if (used_size > 1024 * 1024) {
    LOG(INFO) << "Maximum memory usage: " << used_size_kib / 1024 << " MiB";
  } else {
    LOG(INFO) << "Maximum memory usage: " << used_size_kib << " KiB";
  }
#endif
  
  return 0;
}
