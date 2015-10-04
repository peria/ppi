#include "number/real.h"

#include <gtest/gtest.h>
#include <glog/logging.h>
#include <sstream>

namespace ppi {
namespace number {

// Segmentation fault, some Real instances have no memory for mantissa_.
TEST(RealTest, DISABLED_InverseSqrt) {
  Real val;
  Real::InverseSqrt(2, &val);
  std::ostringstream oss;
  Real::Show(val, oss);
  LOG(INFO) << oss.str();
}

}  // namespace number
}  // namespace ppi
