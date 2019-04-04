#include "bbp/bbp.h"

#include <vector>
#include <glog/logging.h>
#include <gflags/gflags.h>

#include "base/base.h"

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  ppi::Bbp bbp(ppi::Bbp::Formula::kBbp);
  std::vector<ppi::uint64> hexs = bbp.compute(1);
  for (int i = hexs.size() - 1; i >= 0; --i)
    printf("%016lX ", hexs[i]);
  puts("");
  return 0;
}