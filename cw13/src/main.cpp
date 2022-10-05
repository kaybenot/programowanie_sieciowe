#include "JSON.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <stdexcept>
#include <vector>
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>

class Band
{
private:
public:
    std::wstring name;
    std::vector<std::string> members;
};

class Member
{
private:
public:
    int id = -1;
    std::string name;
    std::string artist_url;
    std::vector<std::wstring> bands;
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

    void check_curl_error(CURLcode& res, std::string request)
    {
        if(res != CURLE_OK)
        {
            std::cerr << res << std::endl;
            throw std::runtime_error("An error when performing curl operation!");
        }
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if(http_code == 429)
        {
            std::cout << "Too many requests in last minute, scheduling 61 second pause" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(61));
            request_get(request);
        }
        else if (http_code != 200 || res == CURLE_ABORTED_BY_CALLBACK)
            throw std::runtime_error("Http code indicate fail: " + std::to_string(http_code) + " or curle was aborted by callback");
    }

    void request_get(std::string request)
    {
        bytes.clear();

        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_read_site);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "FooBarApp/3.0");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes);
        res = curl_easy_perform(curl);
        bytes.push_back('\0');
        check_curl_error(res, request);
    }

    std::string ws2s(const std::wstring& wstr)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.to_bytes(wstr);
    }

public:
    void check_members(int artist_num)
    {
        if(curl)
        {
            request_get("https://api.discogs.com/artists/" + std::to_string(artist_num));

            std::vector<Member> members;
            std::wstring band_name;

            JSONValue* json = JSON::Parse(bytes.data());
            JSONObject root = json->AsObject();
            if((root.find(L"name") != root.end()) && root[L"name"]->IsString())
                band_name = root[L"name"]->AsString();
            if((root.find(L"members") != root.end()) && root[L"members"]->IsArray())
            {
                JSONArray arr = root[L"members"]->AsArray();
                for(int i = 0; i < arr.size(); i++)
                {
                    if(!arr[i]->IsObject())
                        continue;
                    JSONObject member = arr[i]->AsObject();
                    Member m;
                    if(member.find(L"id") != member.end() && member[L"id"]->IsNumber())
                        m.id = member[L"id"]->AsNumber();
                    if(member.find(L"name") != member.end() && member[L"name"]->IsString())
                        m.name = ws2s(member[L"name"]->AsString());
                    if(member.find(L"resource_url") != member.end() && member[L"resource_url"]->IsString())
                        m.artist_url = ws2s(member[L"resource_url"]->AsString());
                    members.push_back(m);
                }
            }

            for(Member& member : members)
            {
                request_get(member.artist_url);
                JSONValue* m_json = JSON::Parse(bytes.data());
                if(m_json == nullptr)
                    throw std::runtime_error("An error parsing artist url: " + member.artist_url);
                JSONObject m_root = m_json->AsObject();
                if((m_root.find(L"groups") != m_root.end()) && m_root[L"groups"]->IsArray())
                {
                    JSONArray arr = m_root[L"groups"]->AsArray();
                    for(int i = 0; i < arr.size(); i++)
                    {
                        if(!arr[i]->IsObject())
                            continue;
                        JSONObject m = arr[i]->AsObject();
                        if(m.find(L"name") != m.end() && m[L"name"]->IsString())
                            member.bands.push_back(m[L"name"]->AsString());
                    }
                }
            }

            std::vector<Band> bands;
            for(Member& m : members)
            {
                for(std::wstring bn : m.bands)
                {
                    if(band_name == bn)
                        continue;
                    bool found = false;
                    for(Band& band : bands)
                    {
                        if(band.name == bn)
                        {
                            band.members.push_back(m.name);
                            found = true;
                        }
                    }
                    if(!found)
                    {
                        Band b;
                        b.name = bn;
                        b.members.push_back(m.name);
                        bands.push_back(b);
                    }
                }
            }

            std::sort(bands.begin(), bands.end(), [](const Band& l, const Band& r) -> bool
            {
                return std::locale("pl_PL.UTF8")(l.name, r.name);
            });
            for(Band& b : bands)
            {
                if(b.members.size() < 2)
                    continue;
                std::cout << ws2s(b.name) << ":" << std::endl;
                for(std::string& m_name : b.members)
                    std::cout << "  " << m_name << std::endl;
            }
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

int main(int argc, char* argv[])
{
    std::setlocale(LC_CTYPE, "Polish");
    try
    {
        Discogs_REST rest;
        if(argc < 2)
        {
            std::cout << "Sposób wywoływania: ./program artist_id" << std::endl;
            return 0;
        }
        int artist = std::atoi(argv[1]);
        rest.check_members(artist);
    }
    catch(std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
