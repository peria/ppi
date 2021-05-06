#include <cstdlib>
#include <memory>

#include "base/base.h"
#include "computer/computer.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DECLARE_int64(digits);

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  google::InitGoogleLogging(argv[0]);

  ppi::Computer::Parameter parameter;
  parameter.ParseInitFile();
  parameter.ParseCommandLine(argc, argv);

  if (!parameter.IsValid())
    return 0;

  std::unique_ptr<ppi::Computer> computer = ppi::Computer::Create(parameter);
  computer->Run();

  return 0;
}
