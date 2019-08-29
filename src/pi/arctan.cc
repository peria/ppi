#include "pi/arctan.h"

#include <gflags/gflags.h>

#include "base/base.h"
#include "number/natural.h"

namespace ppi {

namespace {

const ArcTan::Formula kFormulae[] = {
  {"Machin", {{16, 5}, {-4, 239}}},
};

}  // namespace

ArcTan::ArcTan(int formula_id)
  : formula_(kFormulae[formula_id]) {}

void ArcTan::compute(int64 num_dec_digits, Natural& pi) {
}

}  // namespace ppi
