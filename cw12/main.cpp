#include <iostream>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>

enum class JSON_TYPE
{
    STRING,
    UNSPECIFIED
};

static size_t curl_read_site(void* ptr, size_t size, size_t nmemb, void* data)
{
    if(size * nmemb == 0)
        return 0;
    
    std::vector<char>* out = (std::vector<char>*) data;
    for(size_t i = 0; i < size * nmemb; i++)
        out->push_back(((char*)ptr)[i]);
    
    return size * nmemb;
}

class Discogs_REST
{
private:
    CURL* curl;
    std::vector<char> bytes;

    void check_curl_error(CURLcode& res)
    {
        if(res != CURLE_OK)
        {
            std::cerr << res << std::endl;
            throw std::runtime_error("An error when performing curl operation!");
        }
    }

    std::vector<std::string> get_vals(std::string name, std::vector<char>& data, JSON_TYPE jt = JSON_TYPE::UNSPECIFIED)
    {
        std::vector<std::string> outs;
        std::string to_search = "\"" + name + "\": ";
        std::string search_end = ",";
        auto start = data.begin();
        while(start != data.end())
        {
            auto it = std::search(start, data.end(), to_search.begin(), to_search.end());
            if(it == data.end())
                break;
            auto it2 = std::search(it, data.end(), search_end.begin(), search_end.end());
            size_t dist = std::distance(data.begin(), it);
            size_t dist2 = std::distance(data.begin(), it2);
            std::string value;
            for(size_t i = dist + to_search.size(); i < dist2; i++)
                value.push_back(data[i]);
            start = it2;
            if(jt == JSON_TYPE::UNSPECIFIED)
            {
                if(value.size() > 2)
                {
                    value.erase(0, 1);
                    value.pop_back();
                }
            }
            outs.push_back(value);
        }
        return outs;
    }

    std::string strip(const std::string &inpt)
    {
        auto start_it = inpt.begin();
        auto end_it = inpt.rbegin();
        while (std::isspace(*start_it))
            ++start_it;
        while (std::isspace(*end_it))
            ++end_it;
        return std::string(start_it, end_it.base());
    }

    void request_get(unsigned int page, std::string artist)
    {
        bytes.clear();
        std::string site = "https://api.discogs.com/database/search?type=release&artist=" + artist + "&format=album&type=release&key=JOIvwaeEfoeOQDlIIuMj&secret=PnfvyDdvwnNQFVIAajqKCwmBQDcmpUTm&page=" + std::to_string(page);

        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, site.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_read_site);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "FooBarApp/3.0");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes);
        res = curl_easy_perform(curl);
        check_curl_error(res);
    }

public:
    void get_artist_albums(std::string artist)
    {
        if(curl)
        {
            request_get(1, artist);

            auto pages = get_vals("page", bytes);
            auto titles = get_vals("title", bytes);
            int i_pages = std::atoi(pages[0].c_str());
            if(i_pages > 1)
            {
                for(int i = 2; i < i_pages; i++)
                {
                    auto titles2 = get_vals("title", bytes);
                    titles.insert(titles.begin(), titles2.begin(), titles2.end());
                }
            }
            for(auto& title : titles)
                title = strip(title);
            std::sort(titles.begin(), titles.end());
            titles.erase(std::unique(titles.begin(), titles.end()), titles.end());
        }
    }

    Discogs_REST()
    {
        curl = curl_easy_init();
        if(!curl)
            throw std::runtime_error("Could not init curl!");
    }

    ~Discogs_REST()
    {
        if(curl)
            curl_easy_cleanup(curl);
    }
};

int main()
{
    try
    {
        Discogs_REST rest;
        rest.get_artist_albums("Budka-Suflera");
    }
    catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
