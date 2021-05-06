#include "drm/drm3.h"

#include "number/integer.h"

namespace ppi {
namespace drm {

DRM3::DRM3(const Parameter& parameter) : Computer(parameter) {}

double DRM3::runDRM(const int64 terms, Integer& x, Integer& y) {
  Integer z;
  return runDRM(0, terms, x, y, z);
}

double DRM3::runDRM(const int64 n0,
                    const int64 n1,
                    Integer& x0,
                    Integer& y0,
                    Integer& z0) {
  if (n0 + 1 == n1) {
    SetValues(n0, x0, y0, z0);
    return 0;
  }

  const int64 m = (n0 + n1) / 2;
  Integer x1;
  Integer y1;
  Integer z1;
  runDRM(n0, m, x0, y0, z0);
  runDRM(m, n1, x1, y1, z1);

  // // y0 = x1 * y0 + y1 * z0;
  // y0 *= x1;
  // y1 *= z0;
  // y0 += y1;

  // x0 *= x1;
  // z0 *= z1;

  return 0;
}

}  // namespace drm
}  // namespace ppi
