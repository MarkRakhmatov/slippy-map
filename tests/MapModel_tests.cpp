#include "tiles/MapModel.hpp"
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <ostream>
#include <print>
#include <set>
#include <span>

std::ostream &operator<<(std::ostream &os, const geo::Tile &tile) {
  return os << "{ x: " << tile.x << " y: " << tile.y << " z: " << tile.z << " }"
            << std::endl;
}

TEST_CASE("Bounds normal case", "[MapModel]") {
  geo::MapModel model(1200, 800, 3);
  model.setCenterCoords(47, 35);
  auto bounds = model.getBounds();
  REQUIRE(bounds.z == 3);
  REQUIRE(bounds.minx == 2);
  REQUIRE(bounds.xsize == 5);
  REQUIRE(bounds.miny == 1);
  REQUIRE(bounds.ysize == 4);
}
TEST_CASE("Bounds left", "[MapModel]") {
  geo::MapModel model(1200, 800, 3);
  model.setCenterCoords(0.f, -66.f);
  auto bounds = model.getBounds();
  REQUIRE(bounds.z == 3);
  REQUIRE(bounds.minx == 0);
  REQUIRE(bounds.xsize == 5);
  REQUIRE(bounds.miny == 2);
  REQUIRE(bounds.ysize == 4);
}
TEST_CASE("Bounds left beyond edge", "[MapModel]") {
  geo::MapModel model(1200, 800, 1);
  model.setCenterCoords(0.f, -75.f);
  auto bounds = model.getBounds();
  REQUIRE(bounds.z == 1);
  REQUIRE(bounds.minx == -1);
  REQUIRE(bounds.xsize == 5);
  REQUIRE(bounds.miny == 0);
  REQUIRE(bounds.ysize == 4);
}
TEST_CASE("iterate unique tiles", "[MapModel]") {
  geo::MapModel model(1200, 800, 1);
  model.setCenterCoords(0.f, -75.f);
  auto bounds = model.getBounds();
  REQUIRE(bounds.z == 1);
  REQUIRE(bounds.minx == -1);
  REQUIRE(bounds.xsize == 5);
  REQUIRE(bounds.miny == 0);
  REQUIRE(bounds.ysize == 4);
  auto [minx, maxx] = bounds.getMinMaxX();
  REQUIRE(minx == 0);
  REQUIRE(maxx == 1);
  auto [miny, maxy] = bounds.getMinMaxY();
  REQUIRE(miny == 0);
  REQUIRE(maxy == 1);
  std::vector<geo::Tile> tiles;
  bounds.iterateUniqueTiles(
      [&tiles](const geo::Tile &t) { tiles.push_back(t); });
  std::vector<geo::Tile> expectedUniqueTiles{
      {1, 0, 1}, {0, 1, 1}, {0, 0, 1}, {1, 1, 1}};
  std::sort(expectedUniqueTiles.begin(), expectedUniqueTiles.end());
  REQUIRE(tiles.size() == expectedUniqueTiles.size());
  std::sort(tiles.begin(), tiles.end());
  REQUIRE(tiles == expectedUniqueTiles);
}
TEST_CASE("iterate all tiles", "[MapModel]") {
  geo::MapModel model(1200, 800, 1);
  model.setCenterCoords(0.f, -75.f);
  auto bounds = model.getBounds();
  REQUIRE(bounds.z == 1);
  REQUIRE(bounds.minx == -1);
  REQUIRE(bounds.xsize == 5);
  REQUIRE(bounds.miny == 0);
  REQUIRE(bounds.ysize == 4);
  std::vector<geo::Tile> tiles;
  bounds.iterateAllTiles([&tiles](const geo::Tile &t) { tiles.push_back(t); });
  REQUIRE(tiles[4] == geo::Tile{-1, 4, 1});
  REQUIRE(tiles ==
          std::vector<geo::Tile>{
              {-1, 0, 1}, {-1, 1, 1}, {-1, 2, 1}, {-1, 3, 1}, {-1, 4, 1},
              {0, 0, 1},  {0, 1, 1},  {0, 2, 1},  {0, 3, 1},  {0, 4, 1},
              {1, 0, 1},  {1, 1, 1},  {1, 2, 1},  {1, 3, 1},  {1, 4, 1},
              {2, 0, 1},  {2, 1, 1},  {2, 2, 1},  {2, 3, 1},  {2, 4, 1},
              {3, 0, 1},  {3, 1, 1},  {3, 2, 1},  {3, 3, 1},  {3, 4, 1},
              {4, 0, 1},  {4, 1, 1},  {4, 2, 1},  {4, 3, 1},  {4, 4, 1},
          });
}

TEST_CASE("bounds move left", "[Bounds]") {
  geo::TilesBounds bounds1{0, 1, 5, 4, 3};
  std::vector<geo::Tile> tiles1;
  bounds1.iterateAllTiles(
      [&tiles1](const geo::Tile &t) { tiles1.push_back(t); });
  REQUIRE(tiles1 == std::vector<geo::Tile>{
                        {0, 1, 3}, {0, 2, 3}, {0, 3, 3}, {0, 4, 3}, {0, 5, 3},
                        {1, 1, 3}, {1, 2, 3}, {1, 3, 3}, {1, 4, 3}, {1, 5, 3},
                        {2, 1, 3}, {2, 2, 3}, {2, 3, 3}, {2, 4, 3}, {2, 5, 3},
                        {3, 1, 3}, {3, 2, 3}, {3, 3, 3}, {3, 4, 3}, {3, 5, 3},
                        {4, 1, 3}, {4, 2, 3}, {4, 3, 3}, {4, 4, 3}, {4, 5, 3},
                        {5, 1, 3}, {5, 2, 3}, {5, 3, 3}, {5, 4, 3}, {5, 5, 3}});
  geo::TilesBounds bounds2{7, 1, 5, 4, 3};
  std::vector<geo::Tile> tiles2;
  bounds2.iterateAllTiles(
      [&tiles2](const geo::Tile &t) { tiles2.push_back(t); });
  REQUIRE(tiles2[0] == geo::Tile{7, 1, 3});
  REQUIRE(tiles2 ==
          std::vector<geo::Tile>{
              {7, 1, 3},  {7, 2, 3},  {7, 3, 3},  {7, 4, 3},  {7, 5, 3},
              {8, 1, 3},  {8, 2, 3},  {8, 3, 3},  {8, 4, 3},  {8, 5, 3},
              {9, 1, 3},  {9, 2, 3},  {9, 3, 3},  {9, 4, 3},  {9, 5, 3},
              {10, 1, 3}, {10, 2, 3}, {10, 3, 3}, {10, 4, 3}, {10, 5, 3},
              {11, 1, 3}, {11, 2, 3}, {11, 3, 3}, {11, 4, 3}, {11, 5, 3},
              {12, 1, 3}, {12, 2, 3}, {12, 3, 3}, {12, 4, 3}, {12, 5, 3}});
  std::vector<geo::Tile> uniqueTiles1;
  bounds1.iterateUniqueTiles(
      [&uniqueTiles1](const geo::Tile &t) { uniqueTiles1.push_back(t); });
  REQUIRE(bounds1.getMinMaxX() == std::tuple<int32_t, int32_t>(0, 5));
  auto [miny, maxy] = bounds1.getMinMaxY();
  REQUIRE(miny == 1);
  REQUIRE(maxy == 5);
  REQUIRE(uniqueTiles1 ==
          std::vector<geo::Tile>{
              {0, 1, 3}, {0, 2, 3}, {0, 3, 3}, {0, 4, 3}, {0, 5, 3},
              {1, 1, 3}, {1, 2, 3}, {1, 3, 3}, {1, 4, 3}, {1, 5, 3},
              {2, 1, 3}, {2, 2, 3}, {2, 3, 3}, {2, 4, 3}, {2, 5, 3},
              {3, 1, 3}, {3, 2, 3}, {3, 3, 3}, {3, 4, 3}, {3, 5, 3},
              {4, 1, 3}, {4, 2, 3}, {4, 3, 3}, {4, 4, 3}, {4, 5, 3},
              {5, 1, 3}, {5, 2, 3}, {5, 3, 3}, {5, 4, 3}, {5, 5, 3}});
  std::vector<geo::Tile> uniqueTiles2;
  bounds2.iterateUniqueTiles(
      [&uniqueTiles2](const geo::Tile &t) { uniqueTiles2.push_back(t); });
  auto [minx, maxx] = bounds2.getMinMaxX();
  REQUIRE(minx == 0);
  REQUIRE(maxx == 7);
  REQUIRE(bounds2.getMinMaxX() == std::tuple<int32_t, int32_t>(0, 7));
  auto [bounds2Miny, bounds2Maxy] = bounds2.getMinMaxY();
  REQUIRE(bounds2Miny == 1);
  REQUIRE(bounds2Maxy == 5);
  REQUIRE(bounds2.getMinMaxY() == std::tuple<int32_t, int32_t>(1, 5));
  std::vector<geo::Tile> expectedUniqueTiles{
      {0, 1, 3}, {0, 2, 3}, {0, 3, 3}, {0, 4, 3}, {0, 5, 3}, {1, 1, 3},
      {1, 2, 3}, {1, 3, 3}, {1, 4, 3}, {1, 5, 3}, {2, 1, 3}, {2, 2, 3},
      {2, 3, 3}, {2, 4, 3}, {2, 5, 3}, {3, 1, 3}, {3, 2, 3}, {3, 3, 3},
      {3, 4, 3}, {3, 5, 3}, {4, 1, 3}, {4, 2, 3}, {4, 3, 3}, {4, 4, 3},
      {4, 5, 3}, {5, 1, 3}, {5, 2, 3}, {5, 3, 3}, {5, 4, 3}, {5, 5, 3},
      {6, 1, 3}, {6, 2, 3}, {6, 3, 3}, {6, 4, 3}, {6, 5, 3}, {7, 1, 3},
      {7, 2, 3}, {7, 3, 3}, {7, 4, 3}, {7, 5, 3},
  };
  std::sort(expectedUniqueTiles.begin(), expectedUniqueTiles.end());
  std::sort(uniqueTiles2.begin(), uniqueTiles2.end());
  REQUIRE(uniqueTiles2 == expectedUniqueTiles);
  REQUIRE(uniqueTiles2.size() == expectedUniqueTiles.size());
  for (const auto &tile : std::span{uniqueTiles2}.subspan(0, 5 * 5)) {
    std::print("{} {}\n", tile.x, tile.y);
    REQUIRE(bounds1.contains(tile));
  }
}
