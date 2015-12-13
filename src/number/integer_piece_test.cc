#include "number/integer_piece.h"

#include <gtest/gtest.h>
#include "base/base.h"

#include "number/integer.h"

namespace ppi {
namespace number {

class IntegerPieceForTest : public IntegerPiece {
 public:
  using IntegerPiece::Split4In8;
  using IntegerPiece::Gather4In8;
};

TEST(IntegerPieceTest, ErrorInMult) {
  const int n = 64;
  Integer a, b, c;

  a.resize(n);
  b.resize(n);
  for (int i = 0; i < n; ++i) {
    a[i] = b[i] = 0x123456789abcdefULL;
  }

  double err = Integer::Mult(a, b, &c);
  EXPECT_GT(1e-2, err);
}

TEST(IntegerPieceTest, SplitAndGather) {
  Integer a;
  double d[4 * 2];
  a.push_back(0x1234567890abcdefULL);

  // Integer -> double[4][2]
  IntegerPieceForTest::Split4In8(IntegerPiece(a), 1, d);
  EXPECT_EQ(static_cast<double>(0xcdef), d[0]);
  EXPECT_EQ(0.0, d[1]);
  EXPECT_EQ(static_cast<double>(0x90ab), d[2]);
  EXPECT_EQ(0.0, d[3]);
  EXPECT_EQ(static_cast<double>(0x5678), d[4]);
  EXPECT_EQ(0.0, d[5]);
  EXPECT_EQ(static_cast<double>(0x1234), d[6]);
  EXPECT_EQ(0.0, d[7]);

  // double[4][2] -> Integer
  d[1] = 3141.0;
  Integer b;
  b.resize(2);
  IntegerPiece bp(b);
  IntegerPieceForTest::Gather4In8(d, &bp);
  EXPECT_EQ(0x1234567890abcdefULL, b[0]);
  EXPECT_EQ(3141ULL, b[1]);
}

}  // namespace number
}  // namespace ppi
