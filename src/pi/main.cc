#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "base/allocator.h"
#include "base/base.h"
#include "base/timer.h"
#include "drm/chudnovsky.h"
#include "drm/drm.h"
#include "number/real.h"
#include "pi/arctan.h"

DEFINE_int32(type, 0, "0:Chudnovsky, 1:Machin");
DEFINE_int64(digits, 100, "Number of hexadeciaml digits to compute");
DEFINE_string(refer, "", "file name which has another computing result");
DEFINE_int32(base, 10, "Base to output. Accepts only 10 or 16.");

using ppi::int64;

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (argc > 1) {
    FLAGS_digits = strtoll(argv[1], NULL, 10);
  }

  ppi::base::Timer timer_all;
  ppi::base::Timer timer_compute;
  int64 limbs = FLAGS_digits / 16 + 1;

  ppi::number::Real pi;
  pi.setPrecision(limbs);
  switch (FLAGS_type) {
  case 0: {
    std::unique_ptr<ppi::drm::Drm> drm(new ppi::drm::Chudnovsky);
    double error = drm->compute(FLAGS_digits, &pi);
    LOG(INFO) << "Maximum error in FFT: " << error;
    break;
  }
  case 1:
    ppi::pi::Arctan::Machin(&pi);
    break;
  }
  timer_compute.Stop();
  LOG(INFO) << "Computing Time: " << timer_compute.GetTimeInSec() << " sec.";

  ppi::base::Timer timer_output;
  if (FLAGS_base == 16) {
    std::cout << std::hex << pi << "\n";
  } else {
    std::cout << pi << "\n";
  }
  timer_output.Stop();
  LOG(INFO) << "Output(Base conversion): " << timer_output.GetTimeInSec()
            << " sec.";

  if (!FLAGS_refer.empty()) {
    int64 size = pi.size() * 16;
    int64 same = pi.Compare(FLAGS_refer);
    std::cerr << same << " out of " << size << " digits are same.\n";
  }

  timer_all.Stop();
  LOG(INFO) << "Elapsed Time: " << timer_all.GetTimeInSec() << " sec.";

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
