#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>

#include "proto.h"

//输入发送的IP地址

int main(int argc, char **argv)
{
    int sd;
    struct msg_st sbuf;
    struct sockaddr_in raddr;
    struct ip_mreqn mreq;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        exit(1);
    }

    inet_pton(AF_INET, MTGROUP, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex("eth0");
    //创建多播组
    int val = 1;
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    //bind();
    strcpy(sbuf.name, "infty");
    sbuf.chinese = htonl(23);
    sbuf.math = htonl(99);

    //255.255.255.255代表全网广播
    inet_pton(AF_INET, MTGROUP, &raddr.sin_addr);
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