#pragma once

#include "base/base.h"
#include "drm/drm.h"
#include "number/real.h"

namespace ppi {
namespace drm {

using number::Integer;
using number::Real;

class Chudnovsky : public Drm {
 public:
  Chudnovsky() = default;
  ~Chudnovsky() = default;

 private:
  double postCompute(Real* a, Real* b, Real* pi) override;

  int64 numTermsForDigits(int64 num_digits) override;
  void setValues(int64 n, Integer* a, Integer* b, Integer* c) override;
};

}  // namespace drm
}  // namespace ppi
