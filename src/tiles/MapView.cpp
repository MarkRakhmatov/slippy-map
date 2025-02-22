#include "MapView.hpp"
#include <tuple>
#include <vector>
#include "Utils.hpp"

inline int mod(int x, int div){
	x %= div;
	if(x < 0)
		x += div;
	return x;
}

constexpr int g_tilesize = 256;

std::tuple<int32_t, int32_t> LatLongToPxOffset(float lat, float lng, int zoom, int width, int height, int tilesize) {
    auto [xtile, ytile] = geo::LatLongToTileNumF(lat, lng, zoom);
	int32_t offsetx = static_cast<int32_t>(xtile * tilesize - width / 2);
	int32_t offsety = static_cast<int32_t>(ytile * tilesize - height / 2);
    return {offsetx, offsety};
}

std::vector<geo::Tile> boundsToTiles(int minx, int maxx, int miny, int maxy, int zoom) {
    std::vector<geo::Tile> tiles;
    tiles.reserve((maxx-minx)*(maxy-miny));
    for(auto x = minx; x < maxx; ++x) {
        for (auto y = miny; y < maxy; ++y) {
            tiles.emplace_back(x, y, zoom);
        }
    }
    return tiles;
}

struct TilesBounds {
    int32_t minx;
    int32_t miny;
    int32_t maxx;
    int32_t maxy;
};

TilesBounds getWindowBounds(int offsetPixX, int offsetPixY, int width, int height, int tilesize, int zoom) {
    int maxOffsetY = (1 << zoom) * tilesize - height;
	if(offsetPixY < 0)
		offsetPixY = 0;
	else if(offsetPixY > maxOffsetY)
        offsetPixY = maxOffsetY;

	/* Make our x offset loop back around*/
	offsetPixX = mod(offsetPixX, tilesize * (1 << zoom));

    return TilesBounds{
        offsetPixX / tilesize,
        offsetPixY / tilesize,
        (offsetPixX + width) / tilesize + 1,
        (offsetPixY + height) / tilesize + 1
    };
}

namespace geo
{
MapView::MapView(int width, int height, int zoom)
    : m_width(width),
    m_height(height),
    m_zoom(zoom)
{
	resize(width, height);
	updateBounds();
}
void MapView::setCenterCoords(float lat, float lng)
{
    std::tie(m_offsetx, m_offsety) = LatLongToPxOffset(lat, lng, m_zoom, m_width, m_height, g_tilesize);
}
void MapView::moveBy(int dx, int dy)
{
	m_offsetx += dx;
	m_offsety += dy;
}
void MapView::zoomAt(int x, int y)
{
	if(m_zoom < 22){
		++m_zoom;
		m_offsetx = m_offsetx * 2 + x;
		m_offsety = m_offsety * 2 + y;
	}
}
void MapView::zoomFrom(int x, int y)
{
	if(m_zoom > 2){
		--m_zoom;
		m_offsetx = (m_offsetx - m_width / 2) / 2 - x;
		m_offsety = (m_offsety - m_height / 2) / 2 - y;
	}
}
void MapView::zoomIn()
{
	zoomAt(m_width / 2, m_height / 2);
}
void MapView::zoomOut()
{
	if(m_zoom > 3){
		m_zoom--;
		m_offsetx = (m_offsetx - m_width / 2) / 2;
		m_offsety = (m_offsety - m_height / 2) / 2;
	}
}
void MapView::resize(int width, int height)
{
	m_offsetx += (m_width - width) / 2;
	m_offsety += (m_height - height) / 2;

	m_width = width;
	m_height = height;
}
void MapView::updateBounds()
{
    auto bounds = getWindowBounds(m_offsetx, m_offsety, m_width, m_height, g_tilesize, m_zoom);
    m_zoomToTiles[m_zoom] = boundsToTiles(bounds.minx, bounds.maxx, bounds.miny, bounds.maxy, m_zoom);
}
}