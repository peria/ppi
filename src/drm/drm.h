#pragma once

#include "base/base.h"

namespace ppi {

class Float;
class Natural;

// Interface of DRM algirthm.
class Drm {
public:
  void compute(int64 n_dec, Float& pi);

protected:
  Drm() = default;
  virtual int64 numberOfTerms(int64 n_dec) = 0;
  virtual void setValues(const int64 n, Natural& a, Natural& b, Natural& c) = 0;
  virtual void postCompute(int64 n_dec, Natural&& a, Natural&& b, Float& pi) = 0;

private:
  enum UseElementC { kUse, kNoUse };
  void computeCore(int64 n0, int64 n1, Natural& a, Natural& b, Natural& c,
                   UseElementC use_b);
};

}  // namespace ppi
