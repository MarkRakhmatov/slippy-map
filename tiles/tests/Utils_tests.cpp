#include <catch2/catch_test_macros.hpp>
#include "Utils.hpp"


TEST_CASE( "LatLongToTileNum min lat min long", "[geo]" ) {
    auto [x, y] = geo::LatLongToTileNum(-90.0, -180.0, 1);
    REQUIRE(x == 0 );
    REQUIRE(y == -5 );
}

TEST_CASE( "LatLongToTileNum min lat max long", "[geo]" ) {
    auto [x, y] = geo::LatLongToTileNum(-90.0, 180.0, 1);
    REQUIRE(x == 2 );
    REQUIRE(y == -5 );
}

TEST_CASE( "LatLongToTileNum max lat min long", "[geo]" ) {
    auto [x, y] = geo::LatLongToTileNum(90.0, -180.0, 1);
    REQUIRE(x == 0 );
    REQUIRE(y == 6 );
}

TEST_CASE( "LatLongToTileNum max lat max long", "[geo]" ) {
    auto [x, y] = geo::LatLongToTileNum(90.0, 180.0, 1);
    REQUIRE(x == 2 );
    REQUIRE(y == 6 );
}
