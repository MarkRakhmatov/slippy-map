#pragma once
#include <tuple>

namespace geo
{
    std::tuple<int32_t, int32_t> LatLongToTileNum(float lat, float lon, int zoom);
    std::tuple<float, float> LatLongToTileNumF(float lat, float lon, int zoom);
}