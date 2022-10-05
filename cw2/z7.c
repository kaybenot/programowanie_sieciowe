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
        printf("./z5 {IP} {SERVER_PORT}\n");
        return 0;
    }
    const char* ip = argv[1];
    int server_port = atoi(argv[2]);

    char recvbuff[30];

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sd == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(server_port);
    if(inet_pton(AF_INET, ip, &saddr.sin_addr) <= 0)
    {
        perror("Could not translate IP");
        return -1;
    }

    if(connect(sd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
    {
        perror("Could not connect");
        return -1;
    }

    int bytes = read(sd, recvbuff, sizeof(recvbuff));
    if(bytes == -1)
        perror("Error reading from server");
    printf("%s", recvbuff);

    return 0;
}