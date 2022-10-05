#include <iostream>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>

typedef struct
{
    int status = -1;
    bool is_html = false;
    std::string title;
    int temperature = -1;
} site_out;

class SiteReader
{
private:
    static std::string data_after_space(std::vector<char>& data, std::vector<char>::iterator pos)
    {
        std::string out;
        int space = 0;
        for(std::vector<char>::iterator i = pos; i < data.end(); i++)
        {
            if(*i == ' ')
                space++;
            else if(space == 1)
                out += *i;
        }
        return out;
    }

public:
    static void try_read_status(std::vector<char>& data, int& out)
    {
        std::string searching = "HTTP";
        std::vector<char>::iterator it = std::search(data.begin(), data.end(), searching.begin(), searching.end());
        if(it != data.end())
        {
            std::stringstream ss(data_after_space(data, it));
            ss >> out;
            if(ss.bad())
                throw std::runtime_error("Could not read status of the website!");
        }
    }

    static void try_read_content_type(std::vector<char>& data, std::string& out)
    {
        std::string searching = "Content-Type:";
        std::vector<char>::iterator it = std::search(data.begin(), data.end(), searching.begin(), searching.end());
        if(it != data.end())
            out = data_after_space(data, it);
    }

    static void try_read_title(std::vector<char>& data, std::string& out)
    {
        std::string searching1 = "<title>";
        std::string searching2 = "</title>";
        std::vector<char>::iterator it1 = std::search(data.begin(), data.end(), searching1.begin(), searching1.end());
        std::vector<char>::iterator it2 = std::search(data.begin(), data.end(), searching2.begin(), searching2.end());
        if((it1 != data.end()) && (it2 != data.end()))
        {
            std::string o;
            for(std::vector<char>::iterator i = it1 + searching1.size(); i < it2; i++)
                o += *i;
            out = o;
        }
    }

    static void try_read_temp(std::vector<char>& data, int& out)
    {
        std::string searching = "<pre class=\"metar\">";
        std::vector<char>::iterator it = std::search(data.begin(), data.end(), searching.begin(), searching.end());
        if(it != data.end())
        {
            std::string searching2 = "TEMPERATURA";
            std::vector<char>::iterator it2 = std::search(data.begin(), data.end(), searching2.begin(), searching2.end());
            if(it2 != data.end())
            {
                std::stringstream ss(data_after_space(data, it2));
                ss >> out;
                if(ss.bad())
                    throw std::runtime_error("An error reading temperature!");
            }
        }
    }
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

class Scrapper
{
private:
    CURL* curl;

    void check_curl_error(CURLcode& res)
    {
        if(res != CURLE_OK)
        {
            std::cerr << res << std::endl;
            throw std::runtime_error("An error when performing curl operation!");
        }
    }

public:
    void check_site()
    {
        if(curl)
        {
            site_out so;
            std::vector<char> bytes;

            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, "http://krakow.infometeo.pl/");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_read_site);
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes);
            res = curl_easy_perform(curl);
            check_curl_error(res);

            SiteReader::try_read_status(bytes, so.status);
            std::string content_type;
            SiteReader::try_read_content_type(bytes, content_type);
            if(!content_type.empty())
                so.is_html = (content_type == "text/html;") ? true : false;
            SiteReader::try_read_title(bytes, so.title);
            SiteReader::try_read_temp(bytes, so.temperature);

            if(!so.is_html || (so.status != 200) || (so.title != "Pogoda w Krakowie - InfoMeteo.pl"))
                throw std::runtime_error("Invalid website information!");
            else
                std::cout << "Temperatura w Krakowie: " << so.temperature << std::endl;
        }
    }

    Scrapper()
    {
        curl = curl_easy_init();
        if(!curl)
            throw std::runtime_error("Could not init curl!");
    }

    ~Scrapper()
    {
        if(curl)
            curl_easy_cleanup(curl);
    }
};

int main()
{
    try
    {
        Scrapper sc;
        sc.check_site();
    }
    catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
