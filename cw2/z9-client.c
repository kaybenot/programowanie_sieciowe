#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("./z5 {IP} {PORT}\n");
        return 0;
    }
    char* ip = argv[1];
    int listen_port = atoi(argv[2]);

    char sendbuff[2];
    char recvbuff[20];

    int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(listen_port);
    if(inet_pton(AF_INET, ip, &serveraddr.sin_addr) <= 0)
    {
        perror("Could not translate IP");
        return -1;
    }

    snprintf(sendbuff, sizeof(sendbuff), "\n");

    socklen_t len;
    sendto(sd, sendbuff, strlen(sendbuff), 0, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("Requested server for a message\n");
    ssize_t bytes = recvfrom(sd, recvbuff, sizeof(recvbuff), 0, (struct sockaddr*)&serveraddr, &len);
    recvbuff[bytes] = '\0';
    printf("%s", recvbuff);
    /*while (true)
    {
        socklen_t len;
        sendto(sd, sendbuff, strlen(sendbuff), 0, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
        printf("Requested server for a message\n");
        recvfrom(sd, recvbuff, sizeof(recvbuff), 0, (struct sockaddr*)&serveraddr, &len);
        printf("%s", recvbuff);
    }*/

    return 0;
}