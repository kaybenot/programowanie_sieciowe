#include <curl/curl.h>
#include <string>
#include <stdexcept>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpCon
{
private:
    CURL* curl;

public:
    std::string get(std::string website)
    {
        std::string buffer;
        curl_easy_setopt(curl, CURLOPT_URL, website.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return buffer;
    }

    void init()
    {
        curl = curl_easy_init();
        if(!curl)
            throw std::runtime_error("Could not init curl");
    }
};


int main(int argc, char* argv[])
{
    HttpCon http;

    try
    {
        http.init();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::string out = http.get("https://www.google.com");
    std::cout << out << std::endl;
    
    return 0;
}