//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include "transfer.h"

#include <curl/curl.h>

#include <sstream>
#include <fstream>

namespace maptile
{
    class downloader
    {
        CURLM* cm;
        size_t max_parallel;
        std::vector<CURL*> handlers;

        size_t max_transfers;
        std::vector<transfer*> transfers;

        static size_t write_cb(void* data, size_t size, size_t nmemb, void* userp);

    public:
        downloader(const transfer::builder& builder, size_t maxconn = 128);

        ~downloader();

        int download(const transfer::yieldfn& yield);
    };
}

#endif //MAPTILE_DOWNLOAD_H