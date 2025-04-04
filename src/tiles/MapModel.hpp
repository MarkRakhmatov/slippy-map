#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Tile.hpp"

#include "Utils.hpp"

namespace geo {
using int_range = std::tuple<int, int>;
using optional_range = std::optional<int_range>;
using optional_ranges =
    std::tuple<std::optional<int_range>, std::optional<int_range>>;

struct TilesBounds {
  int32_t minx;
  int32_t miny;
  int32_t xsize;
  int32_t ysize;
  int32_t z;

  auto contains(const geo::Tile &tile) const -> bool {
    if (tile.z != z) {
      return false;
    }
    auto [xrange1, xrange2] = getXRanges();
    auto [yrange1, yrange2] = getYRanges();
    bool containsX = inRange(tile.x, xrange1.value()) ||
                     (xrange2 && inRange(tile.x, xrange2.value()));
    bool containsY = inRange(tile.y, yrange1.value()) ||
                     (yrange2 && inRange(tile.y, yrange2.value()));
    return containsX && containsY;
  }

  void iterateUniqueTiles(
      std::function<void(const geo::Tile &)> tileCB) const noexcept {
    int32_t maxX = 1 << z;
    int32_t maxY = 1 << z;

    auto minx1 = mod(minx, maxX);
    auto xsizeRounded = std::min(xsize, maxX - 1);
    int32_t maxx = minx1 + xsizeRounded;

    auto miny1 = mod(miny, maxY);
    auto ysizeRounded = std::min(ysize, maxY - 1);
    int32_t maxy = miny1 + ysizeRounded;

    for (int32_t x = minx1; x <= maxx; ++x) {
      for (int32_t y = miny1; y <= maxy; ++y) {
        tileCB(geo::Tile{mod(x, maxX), mod(y, maxY), z});
      }
    }
  }

  void iterateAllTiles(
      std::function<void(const geo::Tile &)> tileCB) const noexcept {
    for (auto x = minx; x <= minx + xsize; ++x) {
      for (auto y = miny; y <= miny + ysize; ++y) {
        tileCB({x, y, z});
      }
    }
  }

private:
  auto inRange(int value, const int_range &r) const -> bool {
    return value >= std::get<0>(r) && value <= std::get<1>(r);
  }

  auto getXRanges() const -> optional_ranges {
    if (z == 0) {
      return optional_ranges{int_range{0, 0}, std::nullopt};
    }
    if (z == 1) {
      return optional_ranges{int_range{0, 1}, std::nullopt};
    }
    const int maxXSize = static_cast<int>(1 << z);
    if (xsize >= maxXSize) {
      return optional_ranges{int_range{0, maxXSize - 1}, std::nullopt};
    }
    int32_t minx1 = mod(minx, maxXSize);
    int32_t maxx = mod(minx1 + xsize, maxXSize);
    if (minx <= maxx) {
      return optional_ranges{int_range{minx1, maxx}, std::nullopt};
    }
    return optional_ranges{int_range{0, maxx}, int_range{minx1, maxXSize - 1}};
  }

  auto getYRanges() const -> optional_ranges {
    if (z == 0) {
      return optional_ranges{int_range{0, 0}, std::nullopt};
    }
    if (z == 1) {
      return optional_ranges{int_range{0, 1}, std::nullopt};
    }
    const int maxYSize = static_cast<int>(1 << z);
    if (ysize >= maxYSize) {
      return optional_ranges{int_range{0, maxYSize - 1}, std::nullopt};
    }
    int32_t miny1 = mod(miny, maxYSize);
    int32_t maxy = mod(miny1 + ysize, maxYSize);
    if (miny1 <= maxy) {
      return optional_ranges{int_range{miny1, maxy}, std::nullopt};
    }
    return optional_ranges{int_range{0, maxy}, int_range{miny1, maxYSize}};
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
