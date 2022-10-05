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

    char* msg = "Hello world\r\n";
    char recvbuff[2];

    int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(listen_port);
    if(inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr) <= 0)
    {
        perror("Could not translate IP");
        return -1;
    }

    if(bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Could not bind socket");
        return -1;
    }

    struct sockaddr_in clientaddr;
    socklen_t len;
    recvfrom(sd, recvbuff, sizeof(recvbuff), 0, (struct sockaddr*)&clientaddr, &len);
    sendto(sd, msg, strlen(msg), 0, (const struct sockaddr*)&clientaddr, sizeof(clientaddr));
    printf("Sent message to a client with port %d\n", ntohs(clientaddr.sin_port));
    /*while (true)
    {
        socklen_t len;
        recvfrom(sd, recvbuff, sizeof(recvbuff), MSG_WAITALL, (struct sockaddr*)&clientaddr, &len);
        sendto(sd, msg, strlen(msg), 0, (const struct sockaddr*)&clientaddr, sizeof(clientaddr));
        printf("Sent message to a client with port %d\n", ntohs(clientaddr.sin_port));
    }*/

    return 0;
}