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
  LOG(INFO) << val;
}

}  // namespace number
}  // namespace ppi
