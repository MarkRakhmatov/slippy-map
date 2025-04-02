#include "MapView.hpp"
#include "Utils.hpp"
#include <tuple>
#include <vector>

inline int mod(int x, int div) {
  x %= div;
  if (x < 0)
    x += div;
  return x;
}

constexpr int g_tilesize = 256;

std::tuple<int32_t, int32_t> LatLongToPxOffset(float lat, float lng, int zoom,
                                               int width, int height,
                                               int tilesize) {
  auto [xtile, ytile] = geo::LatLongToTileNumF(lat, lng, zoom);
  int32_t offsetx = static_cast<int32_t>(xtile * tilesize - width / 2);
  int32_t offsety = static_cast<int32_t>(ytile * tilesize - height / 2);
  return {offsetx, offsety};
}

namespace geo {
MapView::MapView(int width, int height, int zoom)
    : m_width(width), m_height(height), m_zoom(zoom) {
  resize(width, height);
}
void MapView::setCenterCoords(float lat, float lng) {
  std::tie(m_offsetx, m_offsety) =
      LatLongToPxOffset(lat, lng, m_zoom, m_width, m_height, g_tilesize);
}
void MapView::moveBy(int dx, int dy) {
  m_offsetx += dx;
  m_offsety += dy;
}
void MapView::zoomAt(int x, int y) {
  if (m_zoom < 22) {
    ++m_zoom;
    m_offsetx = m_offsetx * 2 + x;
    m_offsety = m_offsety * 2 + y;
  }
}
void MapView::zoomFrom(int x, int y) {
  if (m_zoom > 2) {
    --m_zoom;
    m_offsetx = (m_offsetx - m_width / 2) / 2 - x;
    m_offsety = (m_offsety - m_height / 2) / 2 - y;
  }
}
void MapView::zoomIn() { zoomAt(m_width / 2, m_height / 2); }
void MapView::zoomOut() {
  if (m_zoom > 3) {
    m_zoom--;
    m_offsetx = (m_offsetx - m_width / 2) / 2;
    m_offsety = (m_offsety - m_height / 2) / 2;
  }
}
void MapView::resize(int width, int height) {
  m_offsetx += (m_width - width) / 2;
  m_offsety += (m_height - height) / 2;

  m_width = width;
  m_height = height;
}

void MapView::updateBounds() noexcept {
  int maxOffsetY = (1 << m_zoom) * g_tilesize - m_height;
  if (m_offsety < 0)
    m_offsety = 0;
  else if (m_offsety > maxOffsetY)
    m_offsety = maxOffsetY;

  /* Make our x offset loop back around*/
  m_offsetx = mod(m_offsetx, g_tilesize * (1 << m_zoom));
}

TilesBounds MapView::getBounds() const noexcept {
  return geo::TilesBounds{m_offsetx / g_tilesize, m_offsety / g_tilesize,
                          (m_offsetx + m_width) / g_tilesize + 1,
                          (m_offsety + m_height) / g_tilesize + 1, m_zoom};
}
} // namespace geo