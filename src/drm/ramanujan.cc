#include "drm/ramanujan.h"

#include <cmath>
#include <memory>
#include <iostream>

#include "number/natural.h"
#include "number/float.h"

namespace ppi {

namespace {
// Refer https://qiita.com/peria/items/c02ef9fc18fb0362fb89
const double kNumDecPerTerm = 14.1816474627;
const int64 kConstA = 13591409;
const int64 kConstB = 545140134;
const int64 kConstC = 640320;

inline void setA(const int64 k, Natural& a) {
  if (k == 0) {
    a = 1;
    return;
  }

  a = kConstC / 24;
  a *= kConstC;
  a *= kConstC;
  a *= k;
  a *= k;
  a *= k;
}

inline void setB(const int64 k, Natural& b) {
  b = k;
  b *= kConstB;
  b += kConstA;
}

inline void setC(const int64 k, Natural& c) {
  c = 6 * k + 5;
  c *= 6 * k + 1;
  c *= 2 * k + 1;
}

};

int64 Ramanujan::numberOfTerms(int64 n_dec) {
  return n_dec / kNumDecPerTerm + 1;
}

void Ramanujan::setValues(const int64 n, Natural& a0, Natural& b0, Natural& c0) {
  // Set A, B, C for n and n+1, and merge them.
  // This hack allows us not to use negative integers.
  setA(n, a0);
  setB(n, b0);
  setC(n, c0);
  Natural a1;
  Natural b1;
  Natural c1;
  setA(n + 1, a1);
  setB(n + 1, b1);
  setC(n + 1, c1);
  std::cout << n << " " << a0 << " " << b0 << " " << c0 << "\n";
  std::cout << (n + 1) << " " << a1 << " " << b1 << " " << c1 << "\n";
  
  b1 *= c0;
  b0 *= a1;
  b0 -= b1;
  a0 *= a1;
  c0 *= c1;
}

void Ramanujan::postCompute(int64 n_dec, Natural&& a, Natural&& b, Float& pi) {
  const int64 length = n_dec / std::log10(16) / 16 + 2;

  a *= 4270934400;
  Float fa(std::move(a));
  Float fb(std::move(b));
  fa.resize(length);
  fb.resize(length);

  pi = Float::iSqrt(10005, length);
  fa *= Inverse(std::move(fb), length);
  pi *= fa;
}

}  // namespace ppi
