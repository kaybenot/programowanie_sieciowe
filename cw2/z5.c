#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("./z5 {PORT}\n");
        return 0;
    }
    int listen_port = atoi(argv[1]);

    char sendbuff[30];

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sd == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(listen_port);
    if(inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) <= 0)
    {
        perror("Could not translate IP");
        return -1;
    }

    if(bind(sd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
    {
        perror("Could not bind socket");
        return -1;
    }

    if(listen(sd, 4) == -1)
    {
        perror("Could not listen to port");
        return -1;
    }

    while (true)
    {
        int connd = accept(sd, NULL, NULL);
        if(connd == -1)
        {
            perror("Could not accept connection");
            continue;
        }
        snprintf(sendbuff, sizeof(sendbuff), "Hello, world!\r\n");
        write(connd, sendbuff, strlen(sendbuff));
        close(connd);
    }

    return 0;
}