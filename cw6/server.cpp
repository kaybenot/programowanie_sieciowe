#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <limits>
#include <chrono>
#include <algorithm>

#define PACKET_MAX 1024

class Helpers
{
private:
public:
    static bool ip_equal(sockaddr_in ip1, sockaddr_in ip2)
    {
        if((ip1.sin_family == ip2.sin_family) &&
            (ip1.sin_port == ip2.sin_port) &&
            (ip1.sin_addr.s_addr == ip2.sin_addr.s_addr))
            return true;
        return false;
    }

    static bool is_digit(char c)
    {
        switch(c)
        {
        case '0' ... '9':
            return true;
        default:
            return false;
        }
    }
};

class Connection
{
private:
    // returns bytes read
    ssize_t try_read(char* buffer)
    {
        return read(fd, buffer, PACKET_MAX);
    }

public:
    sockaddr_in ip;
    int fd;
    bool error_in_line = false;
    uint32_t sum;
    bool has_number = false;

    void send_error()
    {
        send_msg("ERROR");
        error_in_line = true;
        has_number = false;
    }

    void send_msg(std::string msg)
    {
        if(!has_number)
            msg = "ERROR";
        has_number = false;
        msg += "\r\n";
        if(send(fd, msg.c_str(), msg.size(), 0) == -1)
            std::cout << "Could not send a packet" << std::endl;
    }

    void add(uint32_t val)
    {
        if(UINT32_MAX - val < sum)
        {
            send_msg("ERROR");
            error_in_line = true;
            sum = 0;
        }
        else
        {
            sum += val;
        }
    }

    void disconnect()
    {
        close(fd);
    }

    void process_packet(char* buffer)
    {
        uint32_t val = 0;
        ssize_t bytes_read = try_read(buffer);
        if(bytes_read <= 0)
            return;
        for(ssize_t i = 0; i < bytes_read; i++)
        {
            char c = buffer[i];
            switch (c)
            {
            case '0' ... '9':
                if(error_in_line)
                    break;
                has_number = true;
                if((uint64_t)val * 10 + c - 48 > UINT32_MAX)
                {
                    send_error();
                    break;
                }
                else
                    val = val * 10 + c - 48;
                break;
            case ' ':
                if(error_in_line)
                    continue;
                if((i == 0) || (i == bytes_read - 1))
                {
                    send_error();
                    continue;
                }
                if(!Helpers::is_digit(buffer[i - 1]) || !Helpers::is_digit(buffer[i + 1]))
                {
                    send_error();
                    continue;
                }
                add(val);
                val = 0;
                break;
            case '\r':
                if(i + 1 < bytes_read)
                {
                    if(buffer[i + 1] != '\n')
                    {
                        if(error_in_line)
                            break;
                        send_error();
                        break;
                    }
                    else
                    {
                        add(val);
                        val = 0;
                        if(!error_in_line)
                            send_msg(std::to_string(sum));
                        sum = 0;
                        error_in_line = false;
                        i++;
                    }
                }
                else
                {
                    if(error_in_line)
                        break;
                    send_error();
                }
                break;
            default:
                if(error_in_line)
                    break;
                send_error();
                break;
            }
        }
    }

    Connection(sockaddr_in ip, int fd)
    {
        this->ip = ip;
        this->fd = fd;
        sum = 0;
    }
};


class Server
{
private:
    std::array<char, PACKET_MAX> buffer;
    uint16_t port;
    int max_connections;
    std::vector<Connection> connections;
    int fd;
    sockaddr_in server_addr;

    void init()
    {
        fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
        if(fd == -1)
            throw std::runtime_error("Could not create a socket");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1)
            throw std::runtime_error("Could not bind the socket");

        if(listen(fd, max_connections) == -1)
            throw std::runtime_error("Could not listen to port");
    }

    void accept_connection(sockaddr_in client_ip, int fd)
    {
        Connection con(client_ip, fd);
        connections.push_back(con);
    }

    Connection* get_connection(sockaddr_in client_ip)
    {
        for(Connection& con : connections)
        {
            if(Helpers::ip_equal(client_ip, con.ip))
                return &con;
        }
        return nullptr;
    }

    void remove_connection(Connection* con)
    {
        for (size_t i = 0; i < connections.size(); i++)
        {
            if(con == &connections.at(i))
                connections.erase(connections.begin() + i);
        }
        
    }

public:
    Server(uint16_t port, int max_connections)
    {
        this->port = port;
        this->max_connections = max_connections;
    }

    void run()
    {
        init();

        sockaddr_in client_ip;
        socklen_t socket_size = sizeof(client_ip);
        while (true)
        {
            int cd = accept(fd, (sockaddr*)&client_ip, &socket_size);
            if(cd != -1)
            {
                Connection* con = get_connection(client_ip);
                if(con != nullptr)
                {
                    con->disconnect();
                    remove_connection(con);
                }
                accept_connection(client_ip, cd);
            }
            for(Connection& conn : connections)
                conn.process_packet(buffer.data());
        }
        
    }
};

int main(int argc, char* argv[])
{
    Server server(2020, 10);

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
