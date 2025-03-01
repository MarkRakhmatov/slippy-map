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
#include "tiles/MapModel.hpp"
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <thread>
#include <chrono>


class TilesRenderer {
	std::vector<geo::Tile> mScheduledTiles;
	mutable std::vector<SDLTile> mTilesCache;
private:
	int getAvailableTiles(geo::TileDownloader& tileDownloader, std::vector<SDLTile>& availableTiles) {
		auto tileWithBufferCb = [&availableTiles, this](TileWithBuffer& tileWithBuffer) {
			auto surface = IMG_Load_IO(
					SDL_IOFromMem(
					static_cast<void*>(tileWithBuffer.data.data()),
					tileWithBuffer.data.size()
				),
				true
			);
			if (auto it = std::find(mScheduledTiles.cbegin(), mScheduledTiles.cend(), tileWithBuffer.tile); it != mScheduledTiles.cend()) {
				mScheduledTiles.erase(it);
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
		tileDownloader.handleDownloaded(still_running, tileWithBufferCb);
		return still_running;
	}
public:
	bool update(geo::MapModel& view, SDL_Surface* screen, geo::TileDownloader& tileDownloader) {
		auto bounds = view.getBounds();
		auto tileHandler = [&tileDownloader, this](geo::Tile&& tile){
			if (std::find(mScheduledTiles.cbegin(), mScheduledTiles.cend(), tile) != mScheduledTiles.cend()){
				return;
			}
			if (std::find_if(mTilesCache.cbegin(), mTilesCache.cend(),
					[&tile](const auto& sdlTile) {return sdlTile.tile == tile;}) != mTilesCache.cend()){
				return;
			}
			tileDownloader.schedule(tile);
			mScheduledTiles.push_back(tile);
		};
		bounds.iterateUniqueTiles(tileHandler);
		std::vector<SDLTile> availableTiles;
		auto remaining = getAvailableTiles(tileDownloader, availableTiles);
		bool dirty = remaining > 0;
		std::vector<SDLTile> updatedTiles;
		auto boundStr = std::format("Bound: minx {}, xsize {}, miny {}, ysize {}", bounds.minx, bounds.xsize, bounds.miny, bounds.ysize);
		SDL_Log(boundStr.c_str());
		view.normalizeOffset();
		auto [offsetPixX, offsetPixY] = view.getOffset();
		auto offsetStr = std::format("offsetPixX {}, offsetPixY {}", offsetPixX, offsetPixY);
		SDL_Log(offsetStr.c_str());
		
		for(auto& tile: mTilesCache) {
			if (bounds.contains(tile.tile)) {
				updatedTiles.emplace_back(tile.tile, std::move(tile.surface));
			}
		}
		updatedTiles.insert(updatedTiles.end(), std::make_move_iterator(availableTiles.begin()), std::make_move_iterator(availableTiles.end()));
		bounds.iterateAllTiles([&view, &updatedTiles, screen](geo::Tile&& t){
			auto [offsetPixX, offsetPixY] = view.getOffset();
			auto loopX = geo::mod(t.x, 1 << t.z);
			auto loopY = geo::mod(t.y, 1 << t.z);
			geo::Tile normalizedTile{loopX, loopY, t.z};
			auto it = std::find_if(updatedTiles.begin(), updatedTiles.end(), [&normalizedTile](const SDLTile& sdlTile){
				return sdlTile.tile == normalizedTile;
			});
			if (it != updatedTiles.end()) {
				renderTile(screen, t, it->surface.get(), offsetPixX, offsetPixY, 256);
			}
			else {
				SDL_Log("Tile not found!!!");
				renderTile(screen, t, nullptr, offsetPixX, offsetPixY, 256);
			}
		});
		mTilesCache = std::move(updatedTiles);
		return dirty;
	}
};


void eventloop(geo::MapModel &view, SDL_Window* window){
	bool mouseLBDown = false;
	[[maybe_unused]] bool mouseRBDown = false;
	bool dirty = true;
	Uint64 lastLMClick = 0;
	Uint64 lastRMClick = 0;
	float zoomdf = 0;
    
    geo::TileDownloader tileDownloader;
	TilesRenderer tileRenderer;
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
		if(dirty){
			SDL_Surface *screen = SDL_GetWindowSurface(window);
			dirty = tileRenderer.update(view, screen, tileDownloader);
			SDL_UpdateWindowSurface(window);
		}
	}
}

int main(int, char*[]) {
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	int width = 1200, height = 800;
	SDL_Window* window = SDL_CreateWindow("slippy-map", width, height, SDL_WINDOW_RESIZABLE);
    int zoom = 2;
    geo::MapModel mapView(width, height, zoom);
    mapView.setCenterCoords(0.f, -75.f);
    eventloop(mapView, window);
	return 0;
}