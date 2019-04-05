#include "bbp/bbp.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"

DEFINE_int32(formula, 0, "Choose formula. 0:BBP, 1:Bellard.");

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

  const ppi::Bbp bbp(formula);
  std::vector<ppi::uint64> hexs = bbp.compute(hex_index);
  for (std::int64_t i = hexs.size() - 1; i >= 0; --i)
    printf("%016lX ", hexs[i]);
  puts("");

  return 0;
}
