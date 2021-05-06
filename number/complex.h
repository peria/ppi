#include "base/base.h"

namespace ppi {

struct Complex {
  Complex() = default;
  Complex(double r, double i) : real(r), imag(i) {}
  Complex(const Complex&) = default;
  Complex(Complex&&) = default;
  Complex& operator=(Complex&) = default;
  Complex& operator=(const Complex&) = default;
  Complex& operator=(Complex&&) = default;

  Complex& operator+=(const Complex& rhl) {
    real += rhl.real;
    imag += rhl.imag;
    return *this;
  }
  Complex& operator-=(const Complex& rhl) {
    real -= rhl.real;
    imag -= rhl.imag;
    return *this;
  }
  Complex& operator*=(const Complex& rhl) {
    double r = real;
    double i = imag;
    real = r * rhl.real - i * rhl.imag;
    imag = r * rhl.imag + i * rhl.real;
    return *this;
  }
  Complex& operator*=(double rhl) {
    real *= rhl;
    imag *= rhl;
    return *this;
  }

  Complex i() const { return Complex(-imag, real); }

  Complex conj() const { return Complex(real, -imag); }

  double real = 0;
  double imag = 0;
};

inline Complex operator+(const Complex& lhs, const Complex& rhs) {
  return Complex(lhs.real + rhs.real, lhs.imag + rhs.imag);
}

inline Complex operator-(const Complex& lhs, const Complex& rhs) {
  return Complex(lhs.real - rhs.real, lhs.imag - rhs.imag);
}

inline Complex operator*(const Complex& lhs, const Complex& rhs) {
  return Complex(lhs.real * rhs.real - lhs.imag * rhs.imag,
                 lhs.real * rhs.imag + lhs.imag * rhs.real);
}

}  // namespace ppi
