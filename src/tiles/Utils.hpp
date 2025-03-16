#pragma once
#include <tuple>

namespace geo {
inline int mod(int x, int div) {
  x %= div;
  if (x < 0)
    x += div;
  return x;
}
std::tuple<int32_t, int32_t> LatLongToTileNum(float lat, float lon, int zoom);
std::tuple<float, float> LatLongToTileNumF(float lat, float lon, int zoom);
} // namespace geo
