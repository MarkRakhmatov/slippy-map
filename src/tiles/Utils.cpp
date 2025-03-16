#include <cmath>
#include <numbers>
#include <tuple>

#include "Utils.hpp"

int long2tilex(double lon, int z) {
  return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z) {
  double latrad = lat * std::numbers::pi / 180.0;
  return (int)(floor((1.0 - asinh(tan(latrad)) / std::numbers::pi) / 2.0 *
                     (1 << z)));
}

double tilex2long(int x, int z) { return x / (double)(1 << z) * 360.0 - 180; }

double tiley2lat(int y, int z) {
  double n = std::numbers::pi - 2.0 * std::numbers::pi * y / (double)(1 << z);
  return 180.0 / std::numbers::pi * atan(0.5 * (exp(n) - exp(-n)));
}

namespace geo {
std::tuple<int32_t, int32_t> LatLongToTileNum(float lat, float lon, int zoom) {
  auto [x, y] = LatLongToTileNumF(lat, lon, zoom);

  return {static_cast<int32_t>(x), static_cast<int32_t>(y)};
}
std::tuple<float, float> LatLongToTileNumF(float lat, float lon, int zoom) {
  int n = 1 << zoom;
  float xtile = n * ((lon + 180) / 360.0f);
  float lat_rad = lat * std::numbers::pi_v<float> / 180.0f;
  float ytile =
      (1.0f - asinh(tan(lat_rad)) / std::numbers::pi_v<float>) / 2.0f * n;
  return std::tuple<float, float>(xtile, ytile);
}
} // namespace geo
