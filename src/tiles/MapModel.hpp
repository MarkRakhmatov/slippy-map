#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "Tile.hpp"

#include "Utils.hpp"

namespace geo {
struct TilesBounds {
  int32_t minx;
  int32_t miny;
  int32_t xsize;
  int32_t ysize;
  int32_t z;
  auto contains(const geo::Tile &tile) const -> bool {
    /*
    auto [minx1, maxx] = getMinMaxX();
    auto [miny1, maxy] = getMinMaxY();
    return tile.z == z && tile.x >= minx1
            && tile.x <= maxx
            && tile.y >= miny1
            && tile.y <= maxy;
    */
    bool res = false;

    iterateUniqueTiles([&tile, &res](const geo::Tile &uniqueTile) {
      if (uniqueTile == tile) {
        res = true;
      }
    });
    return res;
  }

  auto getMinMaxX() const -> std::tuple<int, int> {
    if (z == 0) {
      return {0, 0};
    }
    if (z == 1) {
      return {0, 1};
    }
    if (xsize >= 1 << z) {
      return {0, (1 << z) - 1};
    }
    auto minx1 = mod(minx, 1 << z);
    int32_t maxx = mod(minx1 + xsize, 1 << z);
    if (maxx < minx1) {
      std::swap(maxx, minx1);
    }
    return {minx1, maxx};
  }

  auto getMinMaxY() const -> std::tuple<int, int> {
    if (z == 0) {
      return {0, 0};
    }
    if (z == 1) {
      return {0, 1};
    }
    if (ysize >= 1 << z) {
      return {0, (1 << z) - 1};
    }
    auto miny1 = mod(miny, 1 << z);
    int32_t maxy = mod(miny1 + ysize, 1 << z);
    if (maxy < miny1) {
      std::swap(maxy, miny1);
    }
    return {miny1, maxy};
  }

  void iterateUniqueTiles(
      std::function<void(const geo::Tile &)> tileCB) const noexcept {
    std::vector<geo::Tile> uniqueTiles{};
    auto allTilesFunc = [&uniqueTiles, &tileCB](const geo::Tile &t) {
      auto x = mod(t.x, 1 << t.z);
      auto y = mod(t.y, 1 << t.z);
      auto normTile = geo::Tile{x, y, t.z};
      auto it = std::find(uniqueTiles.begin(), uniqueTiles.end(), normTile);
      if (it == uniqueTiles.end()) {
        uniqueTiles.push_back(normTile);
        tileCB(std::move(normTile));
      }
    };
    iterateAllTiles(allTilesFunc);
  }

  void iterateAllTiles(
      std::function<void(const geo::Tile &)> tileCB) const noexcept {
    for (auto x = minx; x <= minx + xsize; ++x) {
      for (auto y = miny; y <= miny + ysize; ++y) {
        tileCB({x, y, z});
      }
    }
  }
};

class MapModel {
public:
  MapModel(int width, int height, int zoom);
  void setCenterCoords(float lat, float lng);
  void moveBy(int dx, int dy);
  void zoomAt(int x, int y);
  void zoomFrom(int x, int y);
  void zoomIn();
  void zoomOut();
  void resize(int width, int height);
  TilesBounds getBounds() const noexcept;
  std::tuple<int, int> getOffset() const noexcept {
    return getNormalizedOffset();
  }
  void normalizeOffset() noexcept {
    std::tie(m_offsetx, m_offsety) = getNormalizedOffset();
  }
  std::tuple<int, int> getNormalizedOffset() const noexcept;
  std::tuple<int, int> getSize() const noexcept { return {m_width, m_height}; }

private:
  int m_width, m_height;
  int m_offsetx{}, m_offsety{};
  int m_zoom;
};
} // namespace geo
