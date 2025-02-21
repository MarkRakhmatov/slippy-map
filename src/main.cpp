#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "tiles/Utils.hpp"
#include "tiles/Tile.hpp"
#include "tiles/TileDownloader.hpp"
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <thread>
#include <chrono>

using SDLSurfaceGuard = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;

SDLSurfaceGuard makeSDLSurfaceGuard(SDL_Surface* surface) {
    return SDLSurfaceGuard(surface, SDL_DestroySurface);
}

struct SDLTile {
    geo::Tile tile;
    SDLSurfaceGuard surface;
};

inline int mod(int x, int div){
	x %= div;
	if(x < 0)
		x += div;
	return x;
}

std::tuple<int32_t, int32_t> LatLongToPxOffset(float lat, float lng, int zoom, int width, int height, int tilesize=256) {
    auto [xtile, ytile] = geo::LatLongToTileNumF(lat, lng, zoom);
	int32_t offsetx = static_cast<int32_t>(xtile * tilesize - width / 2);
	int32_t offsety = static_cast<int32_t>(ytile * tilesize - height / 2);
    return {offsetx, offsety};
}

void renderTile(SDL_Surface *screen, SDLTile& sdlTile, int offsetx, int offsety, int tilesize) {
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

std::vector<geo::Tile> boundsToTiles(int minx, int maxx, int miny, int maxy, int zoom) {
    std::vector<geo::Tile> tiles;
    tiles.reserve((maxx-minx)*(maxy-miny));
    for(auto x = minx; x < maxx; ++x) {
        for (auto y = miny; y < maxy; ++y) {
            tiles.emplace_back(x, y, zoom);
        }
    }
    return tiles;
}

struct TilesBounds {
    int32_t minx;
    int32_t miny;
    int32_t maxx;
    int32_t maxy;
};

TilesBounds getWindowBounds(int offsetPixX, int offsetPixY, int width, int height, int tilesize, int zoom) {
    int maxOffsetY = (1 << zoom) * tilesize - height;
	if(offsetPixY < 0)
		offsetPixY = 0;
	else if(offsetPixY > maxOffsetY)
        offsetPixY = maxOffsetY;

	/* Make our x offset loop back around*/
	offsetPixX = mod(offsetPixX, tilesize * (1 << zoom));

    return TilesBounds{
        offsetPixX / tilesize,
        offsetPixY / tilesize,
        (offsetPixX + width) / tilesize + 1,
        (offsetPixY + height) / tilesize + 1
    };
}


int main(int, char*[]) {
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	int width = 1200, height = 800;
	SDL_Window* window = SDL_CreateWindow("slippy-map", width, height, SDL_WINDOW_RESIZABLE);

	SDL_Surface *screen = SDL_GetWindowSurface(window);
    int zoom = 12;
    const int tilesize = 256;
    //Upper left corner pixels coordinates
    auto [offsetPixX, offsetPixY] = LatLongToPxOffset(48.4284f, 37.3656f, zoom, width, height);
    auto bounds = getWindowBounds(offsetPixX, offsetPixY, width, height, tilesize, zoom);

    std::vector<geo::Tile> tiles = boundsToTiles(bounds.minx, bounds.maxx, bounds.miny, bounds.maxy, zoom);
    geo::TileDownloader tileDownloader;

    for (const auto& tile: tiles) {
        tileDownloader.Schedule(tile);
    }
    
    auto tileWithBufferCb = [screen, offsetPixX, offsetPixY](TileWithBuffer& tileWithBuffer) {
        auto surface = IMG_Load_IO(
            SDL_IOFromMem(
                static_cast<void*>(tileWithBuffer.data.data()),
                tileWithBuffer.data.size()
            ),
            true
        );
        if (!surface) {
            SDL_Log("Tile::load image is not loaded");
        }
        else {
            auto sdTile = SDLTile{
                tileWithBuffer.tile,
                makeSDLSurfaceGuard(
                    surface
                )
            };
            renderTile(screen, sdTile, offsetPixX, offsetPixY, tilesize);
        }
    };

    int still_running = 1;
    while (still_running) {
        tileDownloader.GetDownloaded(still_running, tileWithBufferCb); 
    }
    SDL_UpdateWindowSurface(window);
	return 0;
}