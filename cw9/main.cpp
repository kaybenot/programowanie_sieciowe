#include <curl/curl.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>

typedef struct
{
    int status = -1;
    bool is_html = false;
    std::string title;
} site_out;


static size_t curl_read_site(void* ptr, size_t size, size_t nmemb, void* data)
{
    if(size * nmemb == 0)
        return 0;
    
    site_out* so = (site_out*) data;

    std::string site = std::string((char*) ptr);
    
    // Get website type
    size_t html_pos = site.find("HTTP");
    if(html_pos == 0)
    {
        std::string status;
        std::stringstream ss(site);
        ss >> status;
        ss >> status;
        so->status = std::atoi(status.c_str());
    }

    // Get content type
    size_t content_type = site.find("Content-Type:");
    if(content_type == 0)
    {
        std::string content;
        std::stringstream ss(site);
        ss >> content;
        ss >> content;
        so->is_html = (content == "text/html") ? true : false;
    }

    // Get title
    size_t title_start = site.find("<title>");
    size_t title_end = site.find("</title>");
    if((title_start != std::string::npos) && (title_end != std::string::npos))
    {
        std::string title = site.substr(title_start + 7, title_end - title_start - 7);
        so->title = title;
    }
    
    return size * nmemb;
}

class HTTP_Check
{
private:
    CURL* curl;

    void check_curl_error(CURLcode& res)
    {
        if(res != CURLE_OK)
        {
            std::cout << curl_easy_strerror(res) << std::endl;
            exit(2);
        }
    }

public:
    void check_site()
    {
        if(curl)
        {
            site_out so;

            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, "http://th.if.uj.edu.pl/");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_read_site);
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &so);
            res = curl_easy_perform(curl);
            check_curl_error(res);

            if(!so.is_html || (so.status != 200) || (so.title != "Institute of Theorethical Physics"))
                exit(1);
        }
    }

    HTTP_Check()
    {
        curl = curl_easy_init();
        if(!curl)
        {
            std::cout << "Could not init curl!" << std::endl;
            exit(2);
        }
    }

    ~HTTP_Check()
    {
        if(curl)
            curl_easy_cleanup(curl);
    }
};

int main()
{
    HTTP_Check http;
    http.check_site();
    return 0;
}
