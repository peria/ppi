#pragma once

#include "bbp/bbp.h"

#include <vector>
#include "base/base.h"

namespace ppi {

class BbpGpu : public Bbp {
public:
  BbpGpu(const Formula& formula);

private:
  void computeIntegralTerm(const Term& term, int64 bit_shift, int64 from,
			   int64 to, uint64* value) const override;

  static constexpr int64 kNumGpuThreads = 100000;
};

}  // namespace ppi
