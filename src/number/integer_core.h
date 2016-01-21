#pragma once

#include "base/base.h"

namespace ppi {
namespace number {

// IntegerCore class is a namespace to provide computing numbers.
// It does not manage sizes of operands and results.
class IntegerCore {
 public:
  static double Mult(const uint64* a, const int64 na, const uint64* b, const int64 nb, const int64 nc, uint64* c);

 protected:
  static void Split4(const uint64* a, const int64 na, const int64 n, double* ca);
  static double Gather4(double* ca, const int64 n, uint64* a);
};

}  // namespace number
}  // namespace ppi
