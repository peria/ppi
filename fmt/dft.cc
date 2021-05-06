#include "fmt/dft.h"

#include "glog/logging.h"
#include "number/complex.h"

namespace ppi {
namespace fmt {

namespace {

const Complex* GetWs(int64 logn) {
  DCHECK_GE(logn, 1);
  DCHECK_LE(logn, 24);
  static Complex* s_w_table = nullptr;
  if (!s_w_table) {
    s_w_table = new Complex[1 << 24];
  }
  DCHECK(s_w_table);

  return s_w_table;
}

}  // namespace

DFT::DFT(int64 logn)
    : n_(1ULL << logn),
      ws_(GetWs(logn)),
      iws_(ws_ + n_ / 2),
      log2n_(logn % 2),
      log4n_(logn / 2) {}

void DFT::transform(Complex* xs, Direction dir) {
  switch (dir) {
    case Direction::kForward:
      forward(xs);
      return;
    case Direction::kBackward:
      backward(xs);
      return;
  }
}

void DFT::forward(Complex* xs) {
  const Complex* pw = ws_;

  int64 l = 1;
  int64 m = n_;
  for (int64 i = 0; i < log4n_; ++i) {
    m /= 4;
    radix4(xs, l, m, pw);
    pw += m * 3;
    l *= 4;
  }
  for (int64 i = 0; i < log2n_; ++i) {
    m /= 2;
    radix2(xs, l, m);
    l *= 2;
  }
}

void DFT::backward(Complex* xs) {
  const Complex* pw = iws_;

  int64 l = n_;
  int64 m = 1;
  for (int64 i = 0; i < log2n_; ++i) {
    l /= 2;
    radix2(xs, l, m);
    m *= 2;
  }
  for (int64 i = 0; i < log4n_; ++i) {
    l /= 4;
    pw -= m * 3;
    iradix4(xs, l, m, pw);
    m *= 4;
  }

  double inv = 1.0 / n_;
  for (int64 i = 0; i < n_; ++i)
    xs[i] *= inv;
}

void DFT::radix2(Complex* x, int64 l, int64 m) {
  for (int64 j = 0; j < l; ++j) {
    int64 k0 = 2 * j;
    int64 k1 = 2 * j + 1;
    Complex x0(x[k0]);
    Complex x1(x[k1]);
    x[k0] = x0 + x1;
    x[k1] = x0 - x1;
  }
}

void DFT::radix4(Complex* x, int64 l, int64 m, const Complex* ws) {
  for (int64 j = 0; j < l; ++j) {
    for (int64 k = 0; k < m; ++k) {
      Complex w1 = ws[k];
      Complex w2 = w1 * w1;
      Complex w3 = w2 * w1;
      int64 k0 = 4 * j * m + k;
      int64 k1 = (4 * j + 1) * m + k;
      int64 k2 = (4 * j + 2) * m + k;
      int64 k3 = (4 * j + 3) * m + k;
      Complex x0(x[k0]);
      Complex x1(x[k1]);
      Complex x2(x[k2]);
      Complex x3(x[k3]);
      Complex b0 = x0 + x2;
      Complex b1 = x1 + x3;
      Complex b2 = x0 - x2;
      Complex b3 = (x3 - x1).i();
      x[k0] = b0 + b1;
      x[k1] = (b0 - b1) * w2;
      x[k2] = (b2 + b3) * w1;
      x[k3] = (b2 - b3) * w3;
    }
  }
}

void DFT::iradix4(Complex* x, int64 l, int64 m, const Complex* ws) {
  for (int64 j = 0; j < l; ++j) {
    for (int64 k = 0; k < m; ++k) {
      Complex w1 = ws[k].conj();
      Complex w2 = w1 * w1;
      Complex w3 = w2 * w1;
      int64 k0 = 4 * j * m + k;
      int64 k1 = (4 * j + 1) * m + k;
      int64 k2 = (4 * j + 2) * m + k;
      int64 k3 = (4 * j + 3) * m + k;
      Complex x0 = x[k0];
      Complex x1 = x[k1] * w2;
      Complex x2 = x[k2] * w1;
      Complex x3 = x[k3] * w3;
      Complex b0 = x0 + x1;
      Complex b1 = x0 - x1;
      Complex b2 = x2 + x3;
      Complex b3 = (x2 - x3).i();
      x[k0] = b0 + b2;
      x[k1] = b1 + b3;
      x[k2] = b0 - b2;
      x[k3] = b1 - b3;
    }
  }
}

}  // namespace fmt
}  // namespace ppi
