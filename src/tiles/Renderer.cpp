#include "Renderer.hpp"

SDLSurfaceGuard makeSDLSurfaceGuard(SDL_Surface *surface) {
    return SDLSurfaceGuard(surface, SDL_DestroySurface);
}

void renderTile(SDL_Surface *screen, SDLTile &sdlTile, int offsetx, int offsety, int tilesize) {
    const auto& tile = sdlTile.tile;
    SDL_Rect dest{tile.x * tilesize - offsetx, tile.y * tilesize - offsety, tilesize, tilesize};
    while(dest.x <= -tilesize){
        dest.x += (1 << tile.z) * tilesize;
    }
    if (sdlTile.surface) {
        SDL_BlitSurface(sdlTile.surface.get(), NULL, screen, &dest);
    } else {
        SDL_FillSurfaceRect(
            screen,
            &dest,
            SDL_MapRGB(SDL_GetPixelFormatDetails(screen->format),
            SDL_GetSurfacePalette(screen), 255, 255, 255)
        );
    }
}
