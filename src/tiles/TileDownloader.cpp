#include <format>
#include <cstring>

#include "TileDownloader.hpp"

namespace geo {

constexpr auto g_urlFormat = "http://mts0.google.com/vt/hl=en&src=api&x={}&s=&y={}&z={}";

size_t write_callback(void *contents, size_t contentSize, size_t nmemb, void *userp){
    TileWithBuffer* mem = static_cast<TileWithBuffer*>(userp);
    auto buffSize = mem->data.size();
    mem->data.resize(buffSize + contentSize * nmemb);
    memcpy(static_cast<void *>(mem->data.data() + buffSize), contents, contentSize * nmemb);
    return contentSize * nmemb;
}

CURLMultiGuard makeCURLMultiGuard(CURLM* h) {
    return CURLMultiGuard(h, curl_multi_cleanup);
}

TileDownloader::TileDownloader()
    : mCurlMultiHandle(makeCURLMultiGuard(curl_multi_init())) {

}

void TileDownloader::Schedule(const geo::Tile& t) {
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "slippy-map/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    
    mCurlToData[curl] = TileWithBuffer{t, std::vector<uint8_t>{}};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&mCurlToData[curl]);
    
    auto url = std::format(
        g_urlFormat,
        t.x, t.y, t.z);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(mCurlMultiHandle.get(), curl);
}

void TileDownloader::GetDownloaded(int &still_running, TileHandlerCB tileHandler){
    int msgs_left = 0;
    CURLMsg *msg{};
    curl_multi_perform(mCurlMultiHandle.get(), &still_running);
    while((msg = curl_multi_info_read(mCurlMultiHandle.get(), &msgs_left))){
        if(msg->msg == CURLMSG_DONE) {
            auto& tileWithBuffer = mCurlToData[msg->easy_handle];
            if(msg->easy_handle){
                if(msg->data.result){
                    // SDL_Log("Error loading '%s': %s\n", tileWithBuffer.url.c_str(), curl_easy_strerror(msg->data.result));
                } else {
                    tileHandler(tileWithBuffer);

                    mCurlToData.erase(msg->easy_handle);
                    curl_multi_remove_handle(mCurlMultiHandle.get(), msg->easy_handle);
                    curl_easy_cleanup(msg->easy_handle);
                }
            }
        }
    }
}

}