#include "fmt/dft.h"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <cstring>

#include "base/allocator.h"
#include "base/base.h"

namespace ppi {
namespace fmt {

namespace {

Complex* g_work = nullptr;

Complex* WorkArea(int64 size) {
  if (base::Allocator::GetSize(g_work) < size * 2) {
    if (g_work)
      base::Allocator::Deallocate(g_work);
    g_work = base::Allocator::Allocate<Complex>(size * 2);
  }
  return g_work;
}

int64 GetExpOf2(const int64 n) {
  int64 log2n = 0;
  for (int64 m = n; (m & 1) == 0; m /= 2) {
    ++log2n;
  }
  return log2n;
}

void Radix2(const int64 height, const Complex* table, Complex* x, Complex* y) {
#define X(A, B) x[(A)*height + (B)]
#define Y(A, B) y[(A)*2 + (B)]
  Complex c0 = X(0, 0);
  Complex c1 = X(1, 0);
  Complex d0 = c0 + c1;
  Complex d1 = c0 - c1;
  Y(0, 1) = d1;
  Y(0, 0) = d0;
  for (int64 j = 1; j < height; ++j) {
    Complex w = table[j];
    Complex c0 = X(0, j);
    Complex c1 = X(1, j);
    Complex d0 = c0 + c1;
    Complex d1 = c0 - c1;
    Y(j, 0) = d0;
    Y(j, 1) = w * d1;
  }
#undef X
#undef Y
}

void Radix4(const int64 width,
            const int64 height,
            const Complex* table,
            Complex* x,
            Complex* y) {
#define X(A, B, C) x[((A)*height + (B)) * width + (C)]
#define Y(A, B, C) y[((A)*4 + (B)) * width + (C)]
  for (int64 i = 0; i < width; ++i) {
    Complex c0 = X(0, 0, i);
    Complex c1 = X(1, 0, i);
    Complex c2 = X(2, 0, i);
    Complex c3 = X(3, 0, i);
    Complex d0 = c0 + c2;
    Complex d1 = c0 - c2;
    Complex d2 = c1 + c3;
    Complex d3 = (c1 - c3).i();
    Y(0, 3, i) = d1 - d3;
    Y(0, 2, i) = d0 - d2;
    Y(0, 1, i) = d1 + d3;
    Y(0, 0, i) = d0 + d2;
  }
  for (int64 j = 1; j < height; ++j) {
    Complex w1 = table[3 * j];
    Complex w2 = table[3 * j + 1];
    Complex w3 = table[3 * j + 2];
    for (int64 i = 0; i < width; ++i) {
      Complex c0 = X(0, j, i);
      Complex c1 = X(1, j, i);
      Complex c2 = X(2, j, i);
      Complex c3 = X(3, j, i);
      Complex d0 = c0 + c2;
      Complex d1 = c0 - c2;
      Complex d2 = c1 + c3;
      Complex d3 = (c1 - c3).i();
      Y(j, 0, i) = d0 + d2;
      Y(j, 1, i) = w1 * (d1 + d3);
      Y(j, 2, i) = w2 * (d0 - d2);
      Y(j, 3, i) = w3 * (d1 - d3);
    }
  }
#undef X
#undef Y
}

void Radix8(const int64 width,
            const int64 height,
            const Complex* table,
            Complex* x,
            Complex* y) {
#define X(A, B, C) x[((A)*height + (B)) * width + (C)]
#define Y(A, B, C) y[((A)*8 + (B)) * width + (C)]
  static constexpr double kC81 = 0.70710678118654752;

  for (int64 i = 0; i < width; ++i) {
    Complex c0 = X(0, 0, i);
    Complex c1 = X(1, 0, i);
    Complex c2 = X(2, 0, i);
    Complex c3 = X(3, 0, i);
    Complex c4 = X(4, 0, i);
    Complex c5 = X(5, 0, i);
    Complex c6 = X(6, 0, i);
    Complex c7 = X(7, 0, i);
    Complex d0 = c0 + c4;
    Complex d1 = c0 - c4;
    Complex d2 = c2 + c6;
    Complex d3 = (c2 - c6).i();
    Complex d4 = c1 + c5;
    Complex d5 = c1 - c5;
    Complex d6 = c3 + c7;
    Complex d7 = c3 - c7;
    Complex e0 = d0 + d2;
    Complex e1 = d0 - d2;
    Complex e2 = d4 + d6;
    Complex e3 = (d4 - d6).i();
    Complex e4 = kC81 * (d5 - d7);
    Complex e5 = kC81 * (d5 + d7).i();
    Complex e6 = d1 + e4;
    Complex e7 = d1 - e4;
    Complex e8 = d3 + e5;
    Complex e9 = d3 - e5;
    Y(0, 0, i) = e0 + e2;
    Y(0, 1, i) = e6 + e8;
    Y(0, 2, i) = e1 + e3;
    Y(0, 3, i) = e7 - e9;
    Y(0, 4, i) = e0 - e2;
    Y(0, 5, i) = e7 + e9;
    Y(0, 6, i) = e1 - e3;
    Y(0, 7, i) = e6 - e8;
  }
  for (int64 j = 1; j < height; ++j) {
    Complex w1 = table[7 * j];
    Complex w2 = table[7 * j + 1];
    Complex w3 = table[7 * j + 2];
    Complex w4 = table[7 * j + 3];
    Complex w5 = table[7 * j + 4];
    Complex w6 = table[7 * j + 5];
    Complex w7 = table[7 * j + 6];
    for (int64 i = 0; i < width; ++i) {
      Complex c0 = X(0, j, i);
      Complex c1 = X(1, j, i);
      Complex c2 = X(2, j, i);
      Complex c3 = X(3, j, i);
      Complex c4 = X(4, j, i);
      Complex c5 = X(5, j, i);
      Complex c6 = X(6, j, i);
      Complex c7 = X(7, j, i);
      Complex d0 = c0 + c4;
      Complex d1 = c0 - c4;
      Complex d2 = c2 + c6;
      Complex d3 = (c2 - c6).i();
      Complex d4 = c1 + c5;
      Complex d5 = c1 - c5;
      Complex d6 = c3 + c7;
      Complex d7 = c3 - c7;
      Complex e0 = d0 + d2;
      Complex e1 = d0 - d2;
      Complex e2 = d4 + d6;
      Complex e3 = (d4 - d6).i();
      Complex e4 = kC81 * (d5 - d7);
      Complex e5 = kC81 * (d5 + d7).i();
      Complex e6 = d1 + e4;
      Complex e7 = d1 - e4;
      Complex e8 = d3 + e5;
      Complex e9 = d3 - e5;
      Y(j, 0, i) = e0 + e2;
      Y(j, 1, i) = w1 * (e6 + e8);
      Y(j, 2, i) = w2 * (e1 + e3);
      Y(j, 3, i) = w3 * (e7 - e9);
      Y(j, 4, i) = w4 * (e0 - e2);
      Y(j, 5, i) = w5 * (e7 + e9);
      Y(j, 6, i) = w6 * (e1 - e3);
      Y(j, 7, i) = w7 * (e6 - e8);
    }
  }
#undef X
#undef Y
}

const int64 kL2CacheSize = 4 * 1024 * 1024;

}  // namespace

Dft::Setting::Setting(int64 n_, const Axis axis)
    : n(n_), log2n(0), log4n(0), log8n(0), table(nullptr) {
  const int64 exp2 = GetExpOf2(n);
  if (axis == Axis::kFirst && kL2CacheSize / static_cast<int64>(sizeof(Complex)) / 3 < n) {
    // Run a six-step FFT.
    log2n = (exp2 + (n % 5 == 0 ? 2 : 0)) / 2;
    n = 1LL << log2n;
  } else {
    // Run a simple FFT.
    log2n = exp2;
  }

  if (log2n > 1) {
    log4n = 2 - (log2n + 2) % 3;
    log8n = (log2n - 2 * log4n) / 3;
  }

  auto setTable = [](const int64 r, const int64 height, Complex* table) {
    const double theta = -2.0 * M_PI / (r * height);
    for (int64 i = 0; i < height; ++i) {
      for (int64 j = 1; j < r; ++j) {
        double t = theta * i * j;
        table[i * (r - 1) + j - 1] = Complex{std::cos(t), std::sin(t)};
      }
    }
  };

  table = base::Allocator::Allocate<Complex>(2 * n);
  Complex* tbl = table;
  int64 height = n;
  for (int64 i = 0; i < log8n; ++i) {
    height /= 8;
    setTable(8, height, tbl);
    tbl += 7 * height;
  }
  for (int64 i = 0; i < log4n; ++i) {
    height /= 4;
    setTable(4, height, tbl);
    tbl += 3 * height;
  }
  if (log2n == 1) {
    height /= 2;
    setTable(2, height, tbl);
  }
}

Dft::Setting::~Setting() {
  base::Allocator::Deallocate(table);
}

Dft::Dft(const int64 n)
    : setting1_(n, Setting::Axis::kFirst),
      setting2_(n / setting1_.n) {
}

void Dft::Transform(const Direction dir, Complex* a) const {
  const int64 n = setting1_.n * setting2_.n;
  if (dir == Direction::Backward) {
    for (int64 i = 0; i < n; ++i) {
      a[i].imag = -a[i].imag;
    }
  }

  if (setting2_.n == 1) {
    // Run a simple FFT.
    Complex* work = WorkArea(n);
    kernel(setting1_, work, a);
  } else {
    // Run a six-step FFT.
    const double theta = -2.0 * M_PI / n;
    Complex* work = WorkArea((setting1_.n + 1) * 2);
    Complex* temp = work + setting1_.n + 1;

    for (int64 i = 0; i < setting2_.n; ++i) {
      for (int64 j = 0; j < setting1_.n; ++j) {
        temp[j] = a[j * setting2_.n + i];
      }
      kernel(setting1_, work, temp);
      const double theta_i = theta * i;
      for (int64 j = 0; j < setting1_.n; ++j) {
        const double t = theta_i * j;
        a[j * setting2_.n + i] = temp[j] * Complex {std::cos(t), std::sin(t)};
      }
    }
    for (int64 i = 0; i < setting1_.n; ++i) {
      kernel(setting2_, work, a + i * setting2_.n);
    }
  }

  if (dir == Direction::Backward) {
    double inverse = 1.0 / n;
    for (int64 i = 0; i < n; ++i) {
      a[i].real *= inverse;
      a[i].imag *= -inverse;
    }
  }
}

// static
void Dft::kernel(const Setting& setting, Complex* work, Complex* a) {
  Complex* x = a;
  Complex* y = work;
  const Complex* table = setting.table;

  bool data_in_x = true;
  int64 width = 1, height = setting.n;
  for (int64 i = 0; i < setting.log8n; ++i) {
    height /= 8;
    if (data_in_x) {
      Radix8(width, height, table, x, (height > 1) ? y : x);
    } else {
      Radix8(width, height, table, y, x);
    }
    data_in_x = !data_in_x;
    width *= 8;
    table += 7 * height;
  }
  for (int64 i = 0; i < setting.log4n; ++i) {
    height /= 4;
    if (data_in_x) {
      Radix4(width, height, table, x, (height > 1) ? y : x);
    } else {
      Radix4(width, height, table, y, x);
    }
    data_in_x = !data_in_x;
    width *= 4;
    table += 3 * height;
  }
  if (setting.log2n == 1) {
    height /= 2;
    Radix2(height, table, x, (height > 1) ? y : x);
    data_in_x = (height == 1);
    width *= 2;
  }

#if 0
  if (setting.n % 5 == 0) {
    height /= 5;
    Radix5(width, height, data_in_x ? x : y, x);
  }
#endif
}

}  // namespace fmt
}  // namespace ppi
