#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <exception>
#include <string>
#include <limits>
#include <array>
#include <sstream>
#include <algorithm>

#define UDP_MAX 65535

class Server
{
private:
    int fd;
    uint16_t port;
    sockaddr_in serveraddr;
    std::array<char, UDP_MAX> recv_buff;

    void init()
    {
        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(fd == -1)
            throw std::runtime_error("Could not open socket");

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
        if(bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
            throw std::runtime_error("Could not bind socket");
    }

    bool is_digits(const std::string& str)
    {
        return str.find_first_not_of("0123456789") == std::string::npos;
    }
    
    bool has_digits(const std::string& str)
    {
        return std::find_if(str.begin(), str.end(), ::isdigit) != str.end();  
    }

    bool has_spaces(const std::string& str)
    {
        return std::find(str.begin(), str.end(), ' ') != str.end();  
    }

    std::string calc_sum()
    {
        uint32_t sum = 0;

        std::stringstream ss(recv_buff.data());
        if(!has_digits(ss.str()))
            return "ERROR";
        if(!has_spaces(ss.str()))
            return "ERROR";
        std::string item;
        while(std::getline(ss, item, ' '))
        {
            if(!is_digits(item))
                return "ERROR";
            if(item == "")
                continue;
            if(item.at(item.size() - 1) == '\n')
                item.pop_back();
            if(item == "")
                continue;
            if(item.at(item.size() - 1) == '\r')
                item.pop_back();
            if(item == "")
                continue;

            uint32_t val;
            std::stringstream items(item);
            items >> val;
            if(items.fail())
                return "ERROR";

            if(UINT32_MAX - val < sum)
                return "ERROR";
            sum += val;
        }
        return std::to_string(sum);
    }

public:
    Server(uint16_t port)
    {
        this->port = port;
    }

    void run()
    {
        init();
        
        sockaddr_in income;
        socklen_t income_len = sizeof(income);
        while (true)
        {
            ssize_t bytes = recvfrom(fd, recv_buff.data(), recv_buff.size() - 1, 0, (struct sockaddr*)&income, &income_len);
            if(bytes == 0)
            {
                std::string nobytes = "ERROR";
                sendto(fd, nobytes.c_str(), nobytes.size(), 0, (const struct sockaddr*)&income, income_len);
                continue;
            }

            for(size_t i = 0; i < bytes; i++)
            {
                if(recv_buff.at(i) == '\0')
                {
                    std::string nobytes = "ERROR";
                    sendto(fd, nobytes.c_str(), nobytes.size(), 0, (const struct sockaddr*)&income, income_len);
                    continue;
                }
            }

            recv_buff[bytes] = '\0';
            std::cout << "Received packet: " << recv_buff.data();

            std::string sum = calc_sum();
            sendto(fd, sum.c_str(), sum.size(), 0, (const struct sockaddr*)&income, income_len);
        }
        
    }
};


int main(int argc, char* argv[])
{
    Server server(2020);

    try
    {
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
