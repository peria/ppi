#include "pi/arctan.h"

#include <gflags/gflags.h>

#include "base/base.h"
#include "number/natural.h"

namespace ppi {

namespace {

const ArcTan::Formula kFormulae[] = {
  {"Machin", {{16, 5}, {-4, 239}}},
};

}  // namespace

ArcTan::ArcTan(int formula_id)
  : formula_(kFormulae[formula_id]) {}

void ArcTan::compute(int64 num_dec_digits, Natural& pi) {
  const int64 num_digits = num_dec_digits / Natural::kDigitsPerDigit +
    (num_dec_digits % Natural::kDigitsPerDigit != 0) + 1;

  pi.resize(num_digits);
  for (int64 i = 0; i < num_digits; ++i)
    pi[i] = 0;

  for (auto& term : formula_.terms) {
  }
}

}  // namespace ppi
