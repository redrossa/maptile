//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include "maptile.h"

#include <curl/curl.h>

#include <sstream>
#include <fstream>
#include <vector>
#include <functional>

namespace maptile
{
    class header
    {
    protected:
        std::string address;

    public:
        header(std::string address) : address(address) {};

        virtual ~header() = default;

        std::string get_address() const
        {
            return address;
        }
    };

    template<class T>
    struct transfer
    {
        T header;
        std::vector<byte_t> data;

        explicit transfer(T& h) : header(h) {};

        void update_header(T& h)
        {
            header = h;
        }

        void append_data(void* chunk, size_t chunk_size)
        {
            auto* bytes = static_cast<byte_t*>(chunk);
            data.insert(data.end(), bytes, bytes + chunk_size);
        }

        void clear()
        {
            data.clear();
        }
    };

    template<class T>
    class downloader
    {
        CURLM* cm;
        std::vector<CURL*> handlers;
        std::vector<T> headers;

        static size_t write_cb(void* data, size_t size, size_t nmemb, void* userp)
        {
            size_t realsize = size * nmemb;
            auto* t = static_cast<transfer<T>*>(userp);
            t->append_data(data, realsize);
            return realsize;
        }

    public:
        downloader(const std::vector<T>& headers, size_t maxconn = 128) : headers(headers)
        {
            size_t transfers_size = headers.size();
            size_t max_parallel = transfers_size < maxconn ? transfers_size : maxconn;
            handlers.reserve(max_parallel);

            cm = curl_multi_init();
            if (!cm) throw std::bad_alloc();
            curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, max_parallel);
            curl_multi_setopt(cm, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

            for (index_t i = 0; i < max_parallel; i++)
            {
                T h = headers[i];
                CURL* eh;
                transfer<T>* priv;
                try
                {
                    eh = curl_easy_init();
                    if (!eh) throw std::bad_alloc();
                    priv = new transfer<T>(h);
                }
                catch (std::bad_alloc& e)
                {
                    if (eh) curl_easy_cleanup(eh);
                    this->~downloader();
                    throw e;
                }
                curl_easy_setopt(eh, CURLOPT_URL, h.get_address().c_str());
                curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
                curl_easy_setopt(eh, CURLOPT_WRITEDATA, priv);
                curl_easy_setopt(eh, CURLOPT_PRIVATE, priv);
                curl_easy_setopt(eh, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
#if (CURLPIPE_MULTIPLEX > 0)
                curl_easy_setopt(eh, CURLOPT_PIPEWAIT, 1L); /* wait for pipe connection to confirm */
#endif
                handlers.push_back(eh);
            }
        }

        ~downloader()
        {
            for (auto& eh : handlers)
            {
                transfer<T>* priv = nullptr;
                curl_easy_getinfo(eh, CURLINFO_PRIVATE, &priv);
                delete priv;
                curl_easy_cleanup(eh);
            }
            curl_multi_cleanup(cm);
        }

        int download(const std::function<void(const T&, const std::vector<byte_t>&)>& yield)
        {
            int successes = 0;

            CURLMsg* msg;
            int msgs_left;
            int still_alive;
            index_t curr_tcount = handlers.size();

            for (auto& eh : handlers)
                curl_multi_add_handle(cm, eh);

            do
            {
                curl_multi_perform(cm, &still_alive);

                while ((msg = curl_multi_info_read(cm, &msgs_left)))
                {
                    if (msg->msg != CURLMSG_DONE)
                        continue;

                    CURLcode code = msg->data.result;
                    CURL* eh = msg->easy_handle;
                    transfer<T>* priv;
                    curl_multi_remove_handle(cm, eh);
                    curl_easy_getinfo(eh, CURLINFO_PRIVATE, &priv);

                    /* Error transfer so try again */
                    if (code != CURLE_OK)
                    {
                        priv->clear();
                        curl_multi_add_handle(cm, eh); /* Re-add handle */
                        continue;
                    }

                    /* What to do with the completed transfer */
                    try
                    {
                        yield(priv->header, priv->data);
                        successes++;
                    }
                    catch (std::exception& e)
                    {
                        /* ignored */
                    }

                    /* All headers not yet done */;
                    if (curr_tcount < headers.size())
                    {
                        /* Reuse easy handler for the next transfer */
                        T h = headers[curr_tcount++];
                        priv->update_header(h);
                        priv->clear();
                        curl_easy_setopt(eh, CURLOPT_URL, h.get_address().c_str());
                        curl_multi_add_handle(cm, eh);
                    }
                }

                if (still_alive)
                    curl_multi_wait(cm, nullptr, 0, 100, nullptr);
            }
            while (still_alive);

            return successes;
        }
    };
}

#endif //MAPTILE_DOWNLOAD_H