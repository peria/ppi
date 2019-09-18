#include "number/natural.h"

#include <gtest/gtest.h>

#include "base/base.h"

namespace ppi {

class TestableNatural : public Natural {
public:
  TestableNatural() = default;

  using Natural::resize;
  using Natural::push_lead;
};

TEST(NaturalTest, Construct) {
  TestableNatural n;
  EXPECT_EQ(n.size(), 0);
}

TEST(NaturalTest, Resize) {
  TestableNatural n;
  n.resize(1);
  EXPECT_EQ(n.size(), 1);
  EXPECT_EQ(n[0], Digit(0));

  n.resize(3);
  EXPECT_EQ(n.size(), 3);
  EXPECT_EQ(n[0], Digit(0));
  EXPECT_EQ(n[1], Digit(0));
  EXPECT_EQ(n[2], Digit(0));

  n.resize(0);
  EXPECT_EQ(n.size(), 0);
}

TEST(NaturalTest, Normalize) {
  TestableNatural n;
  n.resize(3);
  n[1] = 1;
  n.normalize();
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[1], Digit(1));
}

TEST(NaturalTest, Mult) {
  TestableNatural n;
  n.resize(1);
  n[0] = 1;
  for (int64 i = 2; i <= 30; ++i) {
    Natural::mult(n, i, n);
  }

#if NUMBER_BASE == 10
  // 265252859812191058636308480000000
  EXPECT_EQ(n.size(), 3);
  EXPECT_EQ(n[0], Digit(8636308480000000));
  EXPECT_EQ(n[1], Digit(6525285981219105));
  EXPECT_EQ(n[2], Digit(2));
#else  // NUMBER_BASE == 16
  // 0xd13f6370f96865df5dd54000000
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[0], Digit(0x865df5dd54000000ULL));
  EXPECT_EQ(n[1], Digit(0xd13f6370f96));
#endif
}

namespace {

void FactDC(int64 a, int64 b, Natural& c) {
  if (a + 1 == b) {
    c.resize(1);
    c[0] = a;
    return;
  }
  int64 m = (a + b) / 2;
  Natural c1, c2;
  FactDC(a, m, c1);
  FactDC(m, b, c2);
  Natural::mult(c1, c2, c);
}

}  // namespace

TEST(NaturalTest, Mult2) {
  TestableNatural n;
  n.resize(1);
  n[0] = 1;
  for (int64 i = 2; i <= 50; ++i) {
    Natural::mult(n, i, n);
  }

  TestableNatural m;
  FactDC(1, 51, m);
  
  EXPECT_EQ(m.size(), n.size());
  for (int64 i = 0; i < m.size(); ++i) {
    EXPECT_EQ(m[i], n[i]);
  }
}

TEST(NaturalTest, Div) {
  TestableNatural n;
  n.resize(3);
  n[2] = 1;
#if NUMBER_BASE == 10
  Natural::div(n, 10000, n);
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[0], Digit(0));
  EXPECT_EQ(n[1], Digit(1000000000000));
#else
  Natural::div(n, 0x10000, n);
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[0], Digit(0));
  EXPECT_EQ(n[1], Digit(0x1000000000000));
#endif
}

}  // namespace ppi
