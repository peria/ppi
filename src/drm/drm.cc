#include "drm/drm.h"

#include <memory>

#include "number/natural.h"
#include "number/float.h"

namespace ppi {

void Drm::compute(int64 n_dec, Float& pi) {
  int64 n_terms = numberOfTerms(n_dec);
  n_terms += n_terms & 1;  // Ceiling up to an even number.

  Natural a;
  Natural b;
  Natural c;
  {
    computeCore(0, n_terms, a, b, c, kNoUse);
  }
  c.clear();

  postCompute(n_dec, std::move(a), std::move(b), pi);
}

void Drm::computeCore(int64 n0, int64 n1, Natural& a0, Natural& b0, Natural& c0,
                      UseElementC use_c) {
  if (n0 + 2 == n1) {
    setValues(n0, a0, b0, c0);
    return;
  }

  int64 m = (n1 - n0) / 2;
  m += m & 1;
  Natural a1, b1, c1;
  computeCore(n0, m, a0, b0, c0, kUse);
  computeCore(m, n1, a1, b1, c1, use_c);

  b1 *= c0;
  b0 *= a1;
  b0 += b1;
  a0 *= a1;
  if (use_c == kUse)
    c0 *= c1;
}

}  // namespace ppi
