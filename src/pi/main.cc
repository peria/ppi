#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "base/allocator.h"
#include "base/base.h"
#include "base/timer.h"
#include "drm/chudnovsky.h"
#include "drm/drm.h"
#include "number/real.h"
#include "pi/arctan.h"

DEFINE_int32(type, 0, "0:Chudnovsky, 1:Machin");
DEFINE_int64(digits, 100, "Number of hexadeciaml digits to compute");
DEFINE_string(hex_output, "pi16.txt", "File name to output pi in hexadecimal.");
DEFINE_string(dec_output, "pi10.txt", "File name to output pi in decimal.");

using ppi::int64;

void ComputePi(ppi::number::Real& pi);
void DumpPiInFile(const ppi::number::Real& pi, const std::string& filename);

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (argc > 1) {
    FLAGS_digits = strtoll(argv[1], NULL, 10);
  }

  ppi::base::Timer timer_all;
  {
    ppi::number::Real pi;
    ppi::base::Timer timer_compute;
    ComputePi(pi);
    timer_compute.Stop();
    LOG(INFO) << "Computing Time: " << timer_compute.GetTimeInSec() << " sec.";
    std::cout << "Computing Time: " << timer_compute.GetTimeInSec() << " sec.\n";

    ppi::number::Real pi_dec(ppi::number::Integer::Base::kDecimal);
    if (FLAGS_dec_output != "") {
      ppi::base::Timer timer_base;
      const int64 prec_digits = (FLAGS_digits + 18) / 19;
      pi_dec.setPrecision(prec_digits);
      ppi::pi::Real::ConvertBase(pi, pi_dec);
      timer_base.Stop();
      LOG(INFO) << "Base conversion: " << timer_base.GetTimeInSec() << " sec.";
      std::cout << "Base conversion: " << timer_base.GetTimeInSec() << " sec.\n";
    }

    ppi::base::Timer timer_output;
    DumpPiInFile(pi, FLAGS_hex_output);
    if (FLAGS_dec_output != "") {
      DumpPiInFile(pi_dec, FLAGS_dec_output);
    }
    timer_output.Stop();
    LOG(INFO) << "Output: " << timer_output.GetTimeInSec() << " sec.";
    std::cout << "Output: " << timer_output.GetTimeInSec() << " sec.\n";
  }
  timer_all.Stop();
  LOG(INFO) << "Total elapsed Time: " << timer_all.GetTimeInSec() << " sec.";
  std::cout << "Total elapsed Time: " << timer_all.GetTimeInSec() << " sec.\n";

  int64 used_size = ppi::base::Allocator::allocated_size_peak();
  double used_size_mib = used_size / 1024.0 / 1024.0;
  if (used_size_mib > 1024) {
    LOG(INFO) << "Maximum memory usage: " << used_size_mib / 1024 << " GiB";
    std::cout << "Maximum memory usage: " << used_size_mib / 1024 << " GiB\n";
  } else {
    LOG(INFO) << "Maximum memory usage: " << used_size_mib << " MiB";
    std::cout << "Maximum memory usage: " << used_size_mib << " MiB\n";
  }

  return 0;
}

void ComputePi(ppi::number::Real& pi) {
  switch (FLAGS_type) {
  case 0: {
    std::unique_ptr<ppi::drm::Drm> drm(new ppi::drm::Chudnovsky);
    double error = drm->compute(FLAGS_digits, &pi);
    LOG(INFO) << "Maximum error in FFT: " << error;
    break;
  }
  case 1:
    pi.setPrecision(FLAGS_digits / 16 + 1);
    ppi::pi::Arctan::Machin(&pi);
    break;
  }
}

void DumpPiInFile(const ppi::number::Real& pi, const std::string& filename) {
  using namespace ppi::number;
  static constexpr int64 kDigitsPerLine = 100;
  static char buffer[kDigitsPerLine + 50];
  const char* fmt_digs = (pi.base() == Integer::Base::kHex) ? "%016" PRIX64 : "%019" PRIu64;
  const int64 digs_per_elem = (pi.base() == Integer::Base::kHex) ? 16 : 19;

  CHECK_EQ(pi[pi.size() - 1], 3);
  CHECK_EQ(pi.size() + pi.exponent(), 1);

  std::ofstream ofs(filename);
  ofs << "pi = 3.\n";

  int64 offset = 0;
  for (int64 i = pi.size() - 2; i >= 0; --i) {
    std::sprintf(buffer + offset, fmt_digs, pi[i]);
    offset += digs_per_elem;
    if (offset >= kDigitsPerLine) {
      ofs.write(buffer, kDigitsPerLine);
      ofs.write("\n", 1);
      for (int64 j = kDigitsPerLine; j < offset; ++j)
        buffer[j - kDigitsPerLine] = buffer[j];
      offset -= kDigitsPerLine;
    }
  }
  if (offset) {
    buffer[offset] = '\n';
    ofs.write(buffer, offset + 1);
  }
  LOG(INFO) << "Output " << (pi.size() - 1) * digs_per_elem << " "
            << (pi.base() == Integer::Base::kHex ? "hexa" : "") << "decimal digits.";

  ofs.close();
}

