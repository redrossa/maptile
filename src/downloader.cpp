//
// Created by @redrossa on 2021-06-20.
//

#include "../include/downloader.h"

using namespace maptile;

size_t downloader::write_cb(void* data, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    auto* t = static_cast<transfer*>(userp);
    t->append_data(data, realsize);
    return realsize;
}

downloader::downloader(transfer::builder& builder, size_t maxconn)
{
    const transfer::iterator end_transfer_it = builder.end();
    max_transfers = *end_transfer_it;
    transfers.reserve(max_transfers);

    max_parallel = max_transfers < maxconn ? max_transfers : maxconn;
    handlers.reserve(max_parallel);

    cm = curl_multi_init();
    if (!cm) throw std::bad_alloc();
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, max_parallel);
    curl_multi_setopt(cm, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    for (transfer::iterator i = builder.begin(); i != end_transfer_it; i++)
    {
        transfer* t;
        try
        {
            t = builder(*i);
            transfers.push_back(t);

            if (*i < max_parallel)
            {
                CURL* h = curl_easy_init();
                if (!h) throw std::bad_alloc();
                curl_easy_setopt(h, CURLOPT_URL, t->get_uri().c_str());
                curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_cb);
                curl_easy_setopt(h, CURLOPT_WRITEDATA, t);
                curl_easy_setopt(h, CURLOPT_PRIVATE, t);
                curl_easy_setopt(h, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
#if (CURLPIPE_MULTIPLEX > 0)
                curl_easy_setopt(h, CURLOPT_PIPEWAIT, 1L); /* wait for pipe connection to confirm */
#endif
                handlers.push_back(h);
            }
        }
        catch (std::bad_alloc& e)
        {
            downloader::~downloader();
            throw e;
        }
    }
}

downloader::~downloader()
{
    for (auto& t : transfers)
        delete t;
    for (auto& h : handlers)
        curl_easy_cleanup(h);
    curl_multi_cleanup(cm);
}

int downloader::download(const transfer::yieldfn& yield)
{
    int successes = 0;

    CURLMsg* msg;
    int msgs_left;
    int still_alive;
    index_t curr_tcount = max_parallel;

    for (auto& h : handlers)
        curl_multi_add_handle(cm, h);

    do
    {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left)))
        {
            if (msg->msg != CURLMSG_DONE)
                continue;

            CURLcode code = msg->data.result;
            CURL* h = msg->easy_handle;
            maptile::transfer* priv;
            curl_multi_remove_handle(cm, h);
            curl_easy_getinfo(h, CURLINFO_PRIVATE, &priv);

            /* Error transfer so try again */
            if (code != CURLE_OK)
            {
                priv->clear();
                curl_multi_add_handle(cm, h); /* Read handle */
                continue;
            }

            /* What to do with the completed transfer */
            try
            {
                yield(priv);
                successes++;
            }
            catch (std::exception& e)
            {
                /* ignored */
            }

            /* All transfers not yet done */;
            if (curr_tcount < max_transfers)
            {
                /* Reuse easy handler for the next transfer */
                transfer* t = transfers[curr_tcount++];
                curl_easy_setopt(h, CURLOPT_URL, t->get_uri().c_str());
                curl_easy_setopt(h, CURLOPT_WRITEDATA, t);
                curl_easy_setopt(h, CURLOPT_PRIVATE, t);
                curl_multi_add_handle(cm, h);
            }

            priv->clear();
        }

        if (still_alive) curl_multi_wait(cm, nullptr, 0, 100, nullptr);
    }
    while (still_alive);

    return successes;
}