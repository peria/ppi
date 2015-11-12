#include "number/real.h"

#include <gtest/gtest.h>
#include <glog/logging.h>
#include <sstream>

namespace ppi {
namespace number {

TEST(RealTest, InverseSqrt) {
  Real val;
  val.setPrecision(10);
  Real::InverseSqrt(2, &val);
  std::ostringstream oss;
  oss << val;

  const std::string expect = "0.b504f333f9de648459";
  EXPECT_EQ(expect, oss.str().substr(0, expect.size()));
}

}  // namespace number
}  // namespace ppi
