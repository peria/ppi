#pragma once

#include "drm/drm.h"

namespace ppi {

class Ramanujan : public Drm {
public:
  Ramanujan() = default;

private:
  int64 numberOfTerms(int64 n_dec) override;
  void setValues(const int64 n, Natural& a, Natural& b, Natural& c) override;
  void postCompute(int64 n_dec, Natural&& a, Natural&& b, Float& pi) override;
};

}  // namespace ppi
