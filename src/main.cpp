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
#include "tiles/Renderer.hpp"
#include "tiles/MapView.hpp"
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <thread>
#include <chrono>


int main(int, char*[]) {
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	int width = 1200, height = 800;
	SDL_Window* window = SDL_CreateWindow("slippy-map", width, height, SDL_WINDOW_RESIZABLE);
    int zoom = 12;
    geo::MapView mapView(width, height, zoom);
    mapView.setCenterCoords(48.4284f, 37.3656f);
    mapView.updateBounds();

    std::vector<geo::Tile> tiles = mapView.currentTiles();

    geo::TileDownloader tileDownloader;
    for (const auto& tile: tiles) {
        tileDownloader.Schedule(tile);
    }

	SDL_Surface *screen = SDL_GetWindowSurface(window);
    auto tileWithBufferCb = [screen, &mapView](TileWithBuffer& tileWithBuffer) {
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
            auto [offsetPixX, offsetPixY] = mapView.getOffset();
            renderTile(screen, sdTile, offsetPixX, offsetPixY, 256);
        }
    };

    int still_running = 1;
    while (still_running) {
        tileDownloader.GetDownloaded(still_running, tileWithBufferCb);
    }
    SDL_UpdateWindowSurface(window);
	return 0;
}