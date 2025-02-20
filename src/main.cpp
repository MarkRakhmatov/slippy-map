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
#include <curl/curl.h>

#include "tiles/Utils.hpp"
#include "tiles/Tile.hpp"
#include <SDL3_image/SDL_image.h>

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

void renderTiles(SDL_Surface *screen, const std::vector<geo::Tile>& tiles, int offsetx, int offsety, int tilesize, int zoom) {
    for (auto tile: tiles) {
        SDL_Rect dest{tile.x * tilesize - offsetx, tile.y * tilesize - offsety, tilesize, tilesize};
        while(dest.x <= -tilesize){
            dest.x += (1 << zoom) * tilesize;
        }
        SDL_FillSurfaceRect(
            screen,
            &dest,
            SDL_MapRGB(SDL_GetPixelFormatDetails(screen->format),
            SDL_GetSurfacePalette(screen), 255, 255, 255));
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

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
	std::vector<uint8_t>* mem = (std::vector<uint8_t>*)userp;
    mem->resize(size * nmemb);
    memcpy(static_cast<void *>(mem->data()), contents, size * nmemb);
    return mem->size();
}



class CURLTileDownloader {
public:
    CURLTileDownloader(std::string url)
    : mUrl(url), mCurlMultiHandle(curl_multi_init()){

    }
    void Download() {
        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "slippy-map/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        
        mCurlToData[curl] = std::vector<uint8_t>{};
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&mCurlToData[curl]);
        curl_easy_setopt(curl, CURLOPT_URL, mUrl.c_str());
        curl_multi_add_handle(mCurlMultiHandle, curl);
    }

    int work(){
        int msgs_left = 0;
        CURLMsg *msg{};
        int still_running;
        curl_multi_perform(mCurlMultiHandle, &still_running);
        while((msg = curl_multi_info_read(mCurlMultiHandle, &msgs_left))){
            if(msg->msg == CURLMSG_DONE){
                if(msg->easy_handle){
                    if(msg->data.result){
                        SDL_Log("Error loading '%s': %s\n", mUrl.c_str(), curl_easy_strerror(msg->data.result));
                    }else{
                        auto& data = mCurlToData[msg->easy_handle];
                        SDL_Surface *img = IMG_Load_IO(SDL_IOFromMem(static_cast<void*>(data.data()), data.size()), true);
                        if (!img) {
                            SDL_Log("Tile::load image is not loaded");
                        }
                    }
                    curl_multi_remove_handle(mCurlMultiHandle, msg->easy_handle);
                    curl_easy_cleanup(msg->easy_handle);
                    break;
                }
            }
        }
        return still_running;
    }
private:
    std::string mUrl;
    CURLM* mCurlMultiHandle;
    std::unordered_map<CURL*, std::vector<uint8_t>> mCurlToData;
};


int main(int, char*[]) {
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	int width = 1200, height = 800;
	SDL_Window* window = SDL_CreateWindow("slippymap", width, height, SDL_WINDOW_RESIZABLE);

	SDL_Surface *screen = SDL_GetWindowSurface(window);
    int zoom = 12;
    const int tilesize = 256;
    //Upper left corner pixels coordinates
    auto [offsetPixX, offsetPixY] = LatLongToPxOffset(48.4284f, 37.3656f, zoom, width, height);
    auto bounds = getWindowBounds(offsetPixX, offsetPixY, width, height, tilesize, zoom);

    std::vector<geo::Tile> tiles = boundsToTiles(bounds.minx, bounds.maxx, bounds.miny, bounds.maxy, zoom);

    renderTiles(screen, tiles, offsetPixX, offsetPixY, tilesize, zoom);

	SDL_UpdateWindowSurface(window);
	return 0;
}