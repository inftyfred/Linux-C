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

//输入发送的IP地址

int main(int argc, char **argv)
{
    // if(argc < 2)
    // {
    //     fprintf(stderr, "usage...\n");
    //     exit(1);
    // }

    int sd;
    struct msg_st sbuf;
    struct sockaddr_in raddr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        exit(1);
    }

    //打开broadcast选项
    int val = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    //bind();
    strcpy(sbuf.name, "infty");
    sbuf.chinese = htonl(89);
    sbuf.math = htonl(65);

    //255.255.255.255代表全网广播
    inet_pton(AF_INET, "255.255.255.255", &raddr.sin_addr);
    raddr.sin_port = htons(atoi((RCVPORT)));
    raddr.sin_family = AF_INET;

    if(sendto(sd, &sbuf, sizeof(sbuf), 0, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("sendto()");
        exit(1);
    }

    puts("OK!");

    close(sd);

    exit(0);
}