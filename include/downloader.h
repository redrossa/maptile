//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include "transfer.h"

#include <curl/curl.h>

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
        downloader(transfer::builder& builder, size_t maxconn = 128);

        ~downloader();

        int download(const transfer::yieldfn& yield);

        class fwriter
        {
            const map& m;
            std::string dirname;
            std::string ext;

        public:
            fwriter(const map& m, std::string dirname, std::string ext) : m(m), dirname(dirname), ext(ext) {};

            void operator()(transfer* t) const;
        };
    };
}

#endif //MAPTILE_DOWNLOAD_H