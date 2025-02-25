#pragma once
#include <unordered_map>
#include "Tile.hpp"
#include <functional>

namespace geo
{
struct TilesBounds {
    int32_t minx;
    int32_t miny;
    int32_t maxx;
    int32_t maxy;
    int32_t z;
    bool contains(const geo::Tile& tile) const {
        return tile.z == z && tile.x >= minx && tile.x <= maxx && tile.y >= miny && tile.y <= maxy;
    }
    void iterateTiles(std::function<void(const geo::Tile&)> tileCB) const noexcept{
        for(auto x = minx; x < maxx; ++x) {
            for (auto y = miny; y < maxy; ++y) {
                tileCB({x, y, z});
            }
        }
    }
};
    
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
        void updateBounds() noexcept;
		TilesBounds getBounds() const noexcept;
        std::tuple<int, int> getOffset() const noexcept {
            return  {m_offsetx, m_offsety};
        }
    private:
        int m_width, m_height;
        int m_offsetx{}, m_offsety{};
        int m_zoom;
};
}
