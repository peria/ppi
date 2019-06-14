#pragma once

#include "base/base.h"
#include "number/real.h"

namespace ppi {
namespace drm {

using number::Integer;
using number::Real;

class Drm {
 public:
  Drm() = default;
  virtual ~Drm() = default;

  // Computes pi using DRM algorithm.
  // Returns the maximum rounding error in multiplications.
  double compute(const int64 num_digits, Real* pi);

 protected:
  virtual double postCompute(Real*, Real*, Real*) { return 0; };

  virtual int64 numTermsForDigits(int64 num_digits) = 0;
  virtual void setValues(int64 n, Integer* a, Integer* b, Integer* c) = 0;

 private:
  double internal(int64 n0, int64 n1, Integer* a0, Integer* b0, Integer* c0);
};

}  // namespace drm
}  // namespace ppi
