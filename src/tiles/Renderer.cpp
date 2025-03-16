#include "Renderer.hpp"

SDLSurfaceGuard makeSDLSurfaceGuard(SDL_Surface *surface) {
  return SDLSurfaceGuard(surface, SDL_DestroySurface);
}

void renderTile(SDL_Surface *screen, const geo::Tile &tile,
                SDL_Surface *tileSurface, int offsetx, int offsety,
                int tilesize) {
  SDL_Rect dest{tile.x * tilesize - offsetx, tile.y * tilesize - offsety,
                tilesize, tilesize};
  while (dest.x <= -tilesize) {
    dest.x += (1 << tile.z) * tilesize;
  }
  if (tileSurface) {
    SDL_BlitSurface(tileSurface, NULL, screen, &dest);
  } else {
    SDL_FillSurfaceRect(screen, &dest,
                        SDL_MapRGB(SDL_GetPixelFormatDetails(screen->format),
                                   SDL_GetSurfacePalette(screen), 255, 255,
                                   255));
  }
}
