#include "number/integer_core.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

class IntegerCoreForTest : public IntegerCore {
 public:
  using IntegerCore::Split4;
  using IntegerCore::Gather4;
};

TEST(IntegerTest, SplitAndGather) {
  double c[] = {0x4444, 0x3333, 0x2222, 0x1111};

  // Complex[2] -> Integer
  uint64 a;
  IntegerCoreForTest::Gather4(c, 1, &a);
  EXPECT_EQ(0x1111222233334444ULL, a);

  uint64 b[] = {0x1234567890abcdefULL, 0xfedcba0987654321ULL};

  // Integer -> Complex[2]
  IntegerCoreForTest::Split4(b, 1, 1, c);
  EXPECT_EQ(static_cast<double>(0xcdef), c[0]);
  EXPECT_EQ(static_cast<double>(0x90ab), c[1]);
  EXPECT_EQ(static_cast<double>(0x5678), c[2]);
  EXPECT_EQ(static_cast<double>(0x1234), c[3]);

  // Nega-cyclic
  IntegerCoreForTest::Split4(b, 2, 1, c);
  EXPECT_EQ(static_cast<double>(0xcdef - 0x4321), c[0]);
  EXPECT_EQ(static_cast<double>(0x90ab - 0x8765), c[1]);
  EXPECT_EQ(static_cast<double>(0x5678 - 0xba09), c[2]);
  EXPECT_EQ(static_cast<double>(0x1234 - 0xfedc), c[3]);
}

}  // namespace number
}  // namespace ppi
