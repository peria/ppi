#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"

#include "bbp/bbp.h"
#ifdef __NVCC__  // Compiled with gpu_toolchain
#include "bbp/bbp_gpu.h"
static constexpr bool kDefaultUseGpu = true;
#else
static constexpr bool kDefaultUseGpu = false;
#endif

DEFINE_int32(formula, 1, "Choose formula. 0:BBP, 1:Bellard.");
DEFINE_bool(use_gpu, kDefaultUseGpu, "Whether to use GPU or not.");

namespace {

const ppi::Bbp::Formula kFormulas[] = {
  ppi::Bbp::Formula::kBbp,
  ppi::Bbp::Formula::kBellard,
};

}  // namespace

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (FLAGS_formula >= ppi::array_size(kFormulas)) {
    std::cerr << "Invliad formula id: " << FLAGS_formula << "\n";
    return 0;
  }
  const auto formula = kFormulas[FLAGS_formula];
  const std::int64_t hex_index = (argc < 2) ? 1 : std::strtoll(argv[1], nullptr, 10);

  std::unique_ptr<ppi::Bbp> bbp;
  if (FLAGS_use_gpu) {
#ifdef __NVCC__
    bbp.reset(new ppi::BbpGpu(formula));
#else
    std::cerr << "You can't use GPU version" << std::endl;
    return 0;
#endif
  } else {
    bbp.reset(new ppi::Bbp(formula));
  }

  std::vector<ppi::uint64> hexs = bbp->compute(hex_index);
  for (std::int64_t i = hexs.size() - 1; i >= 0; --i)
    printf("%016" PRIX64 " ", hexs[i]);
  puts("");

  return 0;
}
