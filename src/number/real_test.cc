#include "number/real.h"

#include <gtest/gtest.h>

#include <sstream>

namespace ppi {
namespace number {

TEST(RealTest, InverseSqrt) {
  Real val;
  val.setPrecision(10);
  Real::InverseSqrt(2, &val);
  std::ostringstream oss;
  oss << std::hex << val;

  const std::string expect = "0.b504f333f9de648459";
  EXPECT_EQ(expect, oss.str().substr(0, expect.size()));
}

TEST(RealTest, Inverse) {
  Real val;
  val.setPrecision(10);
  Real::InverseSqrt(2, &val);
  Real inv;
  inv.setPrecision(10);
  Real::Inverse(val, &inv);
  std::ostringstream oss;
  oss << std::hex << inv;

  const std::string expect = "1.6a09e667f3bcc908b";
  EXPECT_EQ(expect, oss.str().substr(0, expect.size()));
}

}  // namespace number
}  // namespace ppi
