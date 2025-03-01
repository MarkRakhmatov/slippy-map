#pragma once

namespace geo
{
    struct Tile
    {
        int x = -1;
        int y = -1;
        int z = -1;
        bool operator==(const Tile& other) const {
            return z == other.z && y == other.y && x == other.x;
        }
        bool operator<(const Tile& other) const {
            if (z != other.z) {
                return z < other.z;
            }
            if (x != other.x) {
                return x <= other.x;
            }
            return y < other.y;
        }
    };
}
