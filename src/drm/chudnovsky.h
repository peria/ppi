#pragma once

#include "base/base.h"
#include "number/real.h"
#include "drm/drm.h"

namespace ppi {
namespace drm {

using number::Integer;
using number::Real;

class Chudnovsky : public Drm {
 public:
  Chudnovsky() = default;

 private:
  void SetValues(int64 n, Integer* a, Integer* b, Integer* c) override;
};

}  // namespace pi
}  // namespace ppi
