#pragma once
#include <unordered_map>
#include "Tile.hpp"

namespace geo
{
class MapView {
	public:
		MapView(int width, int height, int zoom);
		void setCenterCoords(float lat, float lng);
		void moveBy(int dx, int dy);
		void zoomAt(int x, int y);
		void zoomFrom(int x, int y);
		void zoomIn();
		void zoomOut();
		void resize(int width, int height);
		void updateBounds();
        const std::vector<geo::Tile>& currentTiles() {
            return m_zoomToTiles[m_zoom];
        }
        std::tuple<int, int> getOffset() const noexcept{
            return  {m_offsetx, m_offsety};
        }
    private:
        int m_width, m_height;
        int m_offsetx{}, m_offsety{};
        int m_zoom;
        std::unordered_map<int, std::vector<geo::Tile>> m_zoomToTiles;
};
}
