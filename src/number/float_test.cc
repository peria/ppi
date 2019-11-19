#include "number/float.h"

#include <gtest/gtest.h>

#include "base/base.h"

namespace ppi {

class TestableFloat : public Float {
public:
  void setSign(int64 s) { sign_ = s; }
  void setExponent(int64 e) { exponent_ = e; }
  using Natural::kBaseInDouble;
};

TEST(FloatTest, Inverse) {
  TestableFloat a;
  a.resize(3);
  a[2] = 1;
  a[1] = 0;
  a[0] = 0;
  a.setExponent(-3);
  a.setPrecision(3);

  Float c;
  Float::inverse(a, c);
  LOG(INFO) << c;

  a[2] = TestableFloat::kBaseInDouble / 2;
  a[1] = 0;
  a[0] = 0;
  Float::inverse(a, c);
  LOG(INFO) << c;
}

}  // namespace ppi


