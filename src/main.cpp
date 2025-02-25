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

int getAvailableTiles(geo::TileDownloader& tileDownloader,
        geo::MapView& mapView,
        SDL_Surface* screen, std::vector<SDLTile>& availableTiles, std::vector<geo::Tile>& scheduledTiles) {
    auto tileWithBufferCb = [screen, &mapView, &availableTiles, &scheduledTiles](TileWithBuffer& tileWithBuffer) {
        auto surface = IMG_Load_IO(
                SDL_IOFromMem(
                static_cast<void*>(tileWithBuffer.data.data()),
                tileWithBuffer.data.size()
            ),
            true
        );
        if (auto it = std::find(scheduledTiles.cbegin(), scheduledTiles.cend(), tileWithBuffer.tile); it != scheduledTiles.cend()) {
            scheduledTiles.erase(it);
        }
        if (!surface) {
            SDL_Log("Tile::load image is not loaded");
        }
        else {
            availableTiles.emplace_back(tileWithBuffer.tile,
                makeSDLSurfaceGuard(surface));
        }
    };

    int still_running = 1;
    tileDownloader.GetDownloaded(still_running, tileWithBufferCb);
    return still_running;
}


void eventloop(geo::MapView &view, SDL_Window* window){
	bool mouseLBDown = false;
	bool mouseRBDown = false;
	bool dirty = true;
	Uint64 lastLMClick = 0;
	Uint64 lastRMClick = 0;
	float zoomdf = 0;
    
    geo::TileDownloader tileDownloader;
    std::vector<geo::Tile> scheduledTiles;
    std::vector<SDLTile> tilesCache;
	for(;;){
		SDL_Event event;
		while((dirty ? SDL_PollEvent : SDL_WaitEvent)(&event)){
			switch (event.type) {
				case SDL_EVENT_MOUSE_BUTTON_UP:
					mouseLBDown = false;
					mouseRBDown = false;
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					if(event.button.button == 1){
						mouseLBDown = true;
						auto ticks = SDL_GetTicks();
						if(ticks - lastLMClick < 250){
							lastLMClick = 0;
							view.zoomAt(static_cast<int>(event.button.x), static_cast<int>(event.button.y));
							dirty = true;
						}
						lastLMClick = ticks;
					}
					else if (event.button.button == 3) {
						mouseRBDown = true;
						auto ticks = SDL_GetTicks();
						if(ticks - lastRMClick < 250){
							lastRMClick = 0;
							view.zoomFrom(static_cast<int>(event.button.x), static_cast<int>(event.button.y));
							dirty = true;
						}
						lastRMClick = ticks;
					}
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					zoomdf += event.wheel.y;
					if(zoomdf >= 4.0f){
						zoomdf = 0;
						view.zoomIn();
						dirty = true;
					}else if(zoomdf <= -4.0f){
						zoomdf = 0;
						view.zoomOut();
						dirty = true;
					}
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if(mouseLBDown){
						view.moveBy(static_cast<int>(-event.motion.xrel), static_cast<int>(-event.motion.yrel));
						dirty = true;
					}
					break;
				case SDL_EVENT_KEY_DOWN:
					switch(event.key.key){
						case SDLK_ESCAPE:
							exit(0);
							break;
						case SDLK_PLUS:
						case SDLK_EQUALS:
							view.zoomIn();
							dirty = true;
							break;
						case SDLK_MINUS:
							view.zoomOut();
							dirty = true;
							break;
					}
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					view.resize(event.window.data1, event.window.data2);
					dirty = true;
					break;
				case SDL_EVENT_WINDOW_EXPOSED:
					dirty = true;
					break;
				case SDL_EVENT_QUIT:
					exit(0);
				default:
					break;
			}
		}
        SDL_Surface *screen = SDL_GetWindowSurface(window);
		if(dirty){
            view.updateBounds();
			auto bounds = view.getBounds();
            auto tileHandler = [&tileDownloader, &scheduledTiles, &tilesCache](const geo::Tile& tile){
                if (std::find(scheduledTiles.cbegin(), scheduledTiles.cend(), tile) != scheduledTiles.cend()){
                    return;
                }
                if (std::find_if(tilesCache.cbegin(), tilesCache.cend(),
                        [&tile](const auto& sdlTile) {return sdlTile.tile == tile;}) != tilesCache.cend()){
                    return;
                }
                tileDownloader.Schedule(tile);
                scheduledTiles.push_back(tile);
            };
            bounds.iterateTiles(tileHandler);
            std::vector<SDLTile> availableTiles;
            auto remaining = getAvailableTiles(tileDownloader, view, screen, availableTiles, scheduledTiles);
            dirty = remaining > 0;
            std::vector<SDLTile> updatedTiles;
            for(auto& tile: tilesCache) {
                if (bounds.contains(tile.tile)) {
                    updatedTiles.emplace_back(tile.tile, std::move(tile.surface));
                }
            }
            auto [offsetPixX, offsetPixY] = view.getOffset();
            for(auto& sdTile: availableTiles) {
                renderTile(screen, sdTile, offsetPixX, offsetPixY, 256);
            }
            for(auto& sdTile: updatedTiles) {
                renderTile(screen, sdTile, offsetPixX, offsetPixY, 256);
            }
            updatedTiles.insert(updatedTiles.end(), std::make_move_iterator(availableTiles.begin()), std::make_move_iterator(availableTiles.end()));
            tilesCache = std::move(updatedTiles);
		}
        SDL_UpdateWindowSurface(window);
	}
}

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
    eventloop(mapView, window);
	return 0;
}