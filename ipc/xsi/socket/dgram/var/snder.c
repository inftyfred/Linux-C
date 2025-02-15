#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <string.h>

#include "proto.h"

//输入发送的IP地址 + name

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "usage...\n");
        exit(1);
    }

    int sd;
    struct msg_st *sbufp;
    struct sockaddr_in raddr;
    size_t size;

    if(strlen(argv[2]) > NAMEMAX)
    {
        fprintf(stderr, "name is to long\n");
        exit(1);
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    size = sizeof(struct msg_st) + strlen(argv[2]);//柔性数组
    sbufp = malloc(size);
    if(sbufp == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    //bind();
    strcpy(sbufp->name, argv[2]);
    sbufp->chinese = htonl(89);
    sbufp->math = htonl(65);

    inet_pton(AF_INET, argv[1], &raddr.sin_addr);
    raddr.sin_port = htons(atoi((RCVPORT)));
    raddr.sin_family = AF_INET;

    if(sendto(sd, sbufp, size, 0, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("sendto()");
        exit(1);
    }

    puts("OK!");

    close(sd);

    exit(0);
}