#pragma once
#include <curl/curl.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "Tile.hpp"
#include <functional>

using CURLMultiGuard = std::unique_ptr<CURLM, decltype(&curl_multi_cleanup)>;

CURLMultiGuard makeCURLMultiGuard(CURLM* h);

struct TileWithBuffer {
    geo::Tile tile;
    std::vector<uint8_t> data;
};

using TileHandlerCB = std::function<void(TileWithBuffer&)>;

namespace geo {
class TileDownloader {
public:
    TileDownloader();
    void Schedule(const geo::Tile& t);
    void GetDownloaded(int &still_running, TileHandlerCB tileHandler);
private:
    CURLMultiGuard mCurlMultiHandle;
    std::unordered_map<CURL*, TileWithBuffer> mCurlToData;
};    
}