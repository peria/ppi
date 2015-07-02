#include "number/integer.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

class IntegerTest : public testing::Test {
 public:
  virtual void SetUp() {
    UNUSED_VARIABLE(storage_);
  }

  virtual void TearDown() {
  }

 private:
  uint64* storage_;
};

TEST_F(IntegerTest, DoNothing) {
}

}  // namespace number
}  // namespace ppi
