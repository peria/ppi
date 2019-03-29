#include "bbp/bbp.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  return 0;
}