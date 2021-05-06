#pragma once

#include "base/base.h"
#include "computer/computer.h"
#include "number/integer.h"
#include "number/real.h"

namespace ppi {
namespace drm {

class DRM3 : public Computer {
 public:
  const char* algorithm() const final { return "DRM"; }

 protected:
  DRM3(const Parameter& parameter);

  double runDRM(const int64 terms, Integer& x, Integer& y);
  virtual void SetValues(const int64 n, Integer& x, Integer& y, Integer& z) = 0;

 private:
  double runDRM(const int64 n0,
                const int64 n1,
                Integer& x0,
                Integer& y0,
                Integer& z0);
};

}  // namespace drm
}  // namespace ppi
