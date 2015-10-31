#include "number/real.h"

#include <gtest/gtest.h>
#include <glog/logging.h>
#include <sstream>

namespace ppi {
namespace number {

// Segmentation fault, some Real instances have no memory for mantissa_.
TEST(RealTest, InverseSqrt) {
  Real val;
  Real::InverseSqrt(2, &val);
  std::ostringstream oss;
  oss << val;

  const std::string expect = "0.b504f333f9d";
  EXPECT_EQ(expect, oss.str().substr(0, expect.size()));
}

}  // namespace number
}  // namespace ppi
