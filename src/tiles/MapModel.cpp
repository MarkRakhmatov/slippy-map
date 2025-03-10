#include "MapModel.hpp"
#include <tuple>
#include <vector>
#include <cmath>

#include "Utils.hpp"

constexpr int g_tilesize = 256;

std::tuple<int32_t, int32_t> LatLongToPxOffset(float lat, float lng, int zoom, int width, int height, int tilesize) {
    auto [xtile, ytile] = geo::LatLongToTileNumF(lat, lng, zoom);
	int32_t offsetx = static_cast<int32_t>(xtile * tilesize - width / 2);
	int32_t offsety = static_cast<int32_t>(ytile * tilesize - height / 2);
    return {offsetx, offsety};
}

namespace geo
{
MapModel::MapModel(int width, int height, int zoom)
    : m_width(width),
    m_height(height),
    m_zoom(zoom)
{
	resize(width, height);
}
void MapModel::setCenterCoords(float lat, float lng)
{
    std::tie(m_offsetx, m_offsety) = LatLongToPxOffset(lat, lng, m_zoom, m_width, m_height, g_tilesize);
}
void MapModel::moveBy(int dx, int dy)
{
	m_offsetx += dx;
	m_offsety += dy;
}
void MapModel::zoomAt(int x, int y)
{
	if(m_zoom < 22){
		++m_zoom;
		m_offsetx = m_offsetx * 2 + x;
		m_offsety = m_offsety * 2 + y;
	}
}
void MapModel::zoomFrom(int x, int y)
{
	if(m_zoom > 2){
		--m_zoom;
		m_offsetx = (m_offsetx - m_width / 2) / 2 - x;
		m_offsety = (m_offsety - m_height / 2) / 2 - y;
	}
}
void MapModel::zoomIn()
{
	zoomAt(m_width / 2, m_height / 2);
}
void MapModel::zoomOut()
{
	if(m_zoom > 1){
		m_zoom--;
		m_offsetx = (m_offsetx - m_width / 2) / 2;
		m_offsety = (m_offsety - m_height / 2) / 2;
	}
}
void MapModel::resize(int width, int height)
{
	m_offsetx += (m_width - width) / 2;
	m_offsety += (m_height - height) / 2;

	m_width = width;
	m_height = height;
}
TilesBounds MapModel::getBounds() const noexcept
{
	auto xsize = m_width / g_tilesize  + 1;
	auto ysize = m_height / g_tilesize  + 1;
    return geo::TilesBounds{
        m_offsetx / g_tilesize,
        m_offsety / g_tilesize,
        xsize,
        ysize,
		m_zoom
    };
}
std::tuple<int, int> MapModel::getNormalizedOffset() const noexcept
{
	auto offsetx = m_offsetx;
	/* Make our x offset loop back around*/
	offsetx = geo::mod(offsetx, g_tilesize * (1 << m_zoom));
	auto offsety = m_offsety;
	/* Make our y offset loop back around*/
	offsety = geo::mod(offsety, g_tilesize * (1 << m_zoom));
	return {offsetx, offsety};
}
}