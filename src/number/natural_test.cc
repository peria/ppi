#include "number/natural.h"

#include <gtest/gtest.h>

class TestableNatural : public ppi::Natural {
public:
  TestableNatural() = default;

  using ppi::Natural::resize;
  using ppi::Natural::push_lead;
};

TEST(NaturalTest, Construct) {
  TestableNatural n;
  EXPECT_EQ(n.size(), 0);
}

TEST(NaturalTest, Resize) {
  TestableNatural n;
  n.resize(1);
  EXPECT_EQ(n.size(), 1);
  EXPECT_EQ(n[0], ppi::Digit(0));

  n.resize(3);
  EXPECT_EQ(n.size(), 3);
  EXPECT_EQ(n[0], ppi::Digit(0));
  EXPECT_EQ(n[1], ppi::Digit(0));
  EXPECT_EQ(n[2], ppi::Digit(0));

  n.resize(0);
  EXPECT_EQ(n.size(), 0);
}

TEST(NaturalTest, Normalize) {
  TestableNatural n;
  n.resize(3);
  n[1] = 1;
  n.normalize();
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[1], ppi::Digit(1));
}

TEST(NaturalTest, Div) {
  TestableNatural n;
  n.resize(3);
  n[2] = 1;
  ppi::Natural::div(n, 10000, n);
  EXPECT_EQ(n.size(), 2);
  EXPECT_EQ(n[0], ppi::Digit(0));
  EXPECT_EQ(n[1], ppi::Digit(1000000000000));
}

