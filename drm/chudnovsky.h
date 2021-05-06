#pragma once

#include "base/base.h"
#include "drm/drm3.h"

namespace ppi {

class Real;

namespace drm {

class Chudnovsky : public DRM3 {
 public:
  static std::unique_ptr<Computer> Create(const Parameter& parameter);

  // Computer override
  Real&& Compute() override;
  const char* formula() const override { return "Chudnovsky"; }

  // DRM3 override
  void SetValues(const int64 n, Integer& x, Integer& y, Integer& z) override;

 private:
  Chudnovsky(const Parameter& parameter);
};

}  // namespace drm
}  // namespace ppi
