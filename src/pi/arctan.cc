#include "pi/arctan.h"

#include <cmath>

#include <glog/logging.h>

#include "base/base.h"
#include "number/natural.h"

namespace ppi {

namespace {

const Arctan::Formula kFormulae[] = {
  {"Machin", {{16, 5}, {-4, 239}}},
};

}  // namespace

Arctan::Arctan(int formula_id)
  : formula_(kFormulae[formula_id]) {}

void Arctan::compute(int64 num_dec_digits, Natural& pi) {
  // The number of Digit's after the decimal point.
  const int64 num_digits = num_dec_digits / Natural::kDigitsPerDigit +
    (num_dec_digits % Natural::kDigitsPerDigit != 0);
  LOG(INFO) << num_digits;

  pi = 0;
  for (auto& term : formula_.terms) {
    const int64 a = term.first;
    const int64 x = term.second;

    Natural ax;
    ax.resize(num_digits + 1);
    ax[num_digits] = a * x;
    for (int64 i = 0; i < num_digits; ++i)
      ax[i] = 0;

    const int64 num_terms = num_dec_digits / std::log10(term.second) / 2 + 1;
    const int64 x2 = x * x;
    for (int64 i = 0; i < num_terms; ++i) {
      Natural t;
      Natural::div(ax, x2, ax);
      Natural::div(ax, 2 * i + 1, t);
      if ((i % 2 == 0) == (a > 0)) {
        Natural::add(pi, t, pi);
      } else {
        Natural::subtract(pi, t, pi);
      }
    }
  }
}

}  // namespace ppi
