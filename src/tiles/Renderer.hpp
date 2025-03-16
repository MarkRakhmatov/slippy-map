#pragma once
#include <SDL3/SDL.h>
#include <memory>

#include "Tile.hpp"

using SDLSurfaceGuard =
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;

SDLSurfaceGuard makeSDLSurfaceGuard(SDL_Surface *surface);

struct SDLTile {
  geo::Tile tile;
  SDLSurfaceGuard surface;
};

void renderTile(SDL_Surface *screen, const geo::Tile &tile,
                SDL_Surface *tileSurface, int offsetx, int offsety,
                int tilesize);
