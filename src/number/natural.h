#pragma once

#include "base/base.h"

namespace ppi {
namespace number {

// Natural class is a namespace to provide computing numbers.
// It does not manage sizes of operands and results.
class Natural {
 public:
  static uint64 Add(const uint64* a, const uint64* b, const int64 n, uint64* c);
  static uint64 Add(const uint64* a, uint64 b, const int64 n, uint64* c);
  static uint64 Subtract(const uint64* a,
                         const uint64* b,
                         const int64 n,
                         uint64* c);
  static uint64 Subtract(const uint64* a, uint64 b, const int64 n, uint64* c);
  static double Mult(const uint64* a,
                     const int64 na,
                     const uint64* b,
                     const int64 nb,
                     const int64 nc,
                     uint64* c);
  static uint64 Mult(const uint64* a, const uint64 b, const int64 n, uint64* c);

  // Computes a[2] / b, assuming a[1] < b.  It means the quotient is storable in
  // uint64.
  // Returns the quotient and stores the remainder in c.
  static uint64 Div(const uint64* a, const uint64 b, uint64* c);
  // Computes a[n] / b, assuming a[n-1] < b.  It means the quotient is storable
  // in uint64.
  // Returns the quotient and stores the remainder in c.
  static uint64 Div(const uint64* a, const uint64 b, const int64 n, uint64* c);
  // Computes (a << (n * 64)) / b, assuming a < b.
  // Stores the quotient into c, and returns the remainder.
  static uint64 Div(const uint64 a, const uint64 b, const int64 n, uint64* c);

 protected:
  static double MultFmt(const uint64* a,
                        const int64 na,
                        const uint64* b,
                        const int64 nb,
                        const int64 nc,
                        uint64* c);
  static void Split4(const uint64* a,
                     const int64 na,
                     const int64 n,
                     double* ca);
  static double Gather4(double* ca, const int64 n, uint64* a);
};

}  // namespace number
}  // namespace ppi
