#include <tuple>
#include <numbers>
#include <cmath>


int long2tilex(double lon, int z) 
{ 
	return (int)(floor((lon + 180.0) / 360.0 * (1 << z))); 
}

int lat2tiley(double lat, int z)
{ 
    double latrad = lat * std::numbers::pi/180.0;
	return (int)(floor((1.0 - asinh(tan(latrad)) / std::numbers::pi) / 2.0 * (1 << z))); 
}

double tilex2long(int x, int z) 
{
	return x / (double)(1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z) 
{
	double n = std::numbers::pi - 2.0 * std::numbers::pi * y / (double)(1 << z);
	return 180.0 / std::numbers::pi * atan(0.5 * (exp(n) - exp(-n)));
}

namespace geo
{
    std::tuple<int32_t, int32_t> LatLongToTileNum(float lat, float lon, int zoom) {
        int n = 1 << zoom;
        int32_t xtile = static_cast<int32_t>(n * ((lon + 180) / 360.0f));
        float lat_rad = lat*std::numbers::pi_v<float>/180.0f;
        int32_t ytile = static_cast<int32_t>(
            floor((1.0 - asinh(tan(lat_rad)) / std::numbers::pi_v<float>) / 2.0f * n)
        );
        return std::tuple<int32_t, int32_t>(xtile, ytile);
    }
}