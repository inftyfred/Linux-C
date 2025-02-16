#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>

#include "proto.h"

#define IPSTRSIZE 128

int main()
{
    int sd;
    struct sockaddr_in laddr, raddr;//local/remote addr
    struct msg_st rbuf;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];

    sd = socket(AF_INET, SOCK_DGRAM, 0);//0代表默认的一个协议：IPPROTO_UDP
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }
    //0.0.0.0可以匹配任何IP地址（any ip）
    //即绑定到所有可用网络接口。
    //当套接字绑定到该地址时，会监听所有本地 IP 接口（如以太网、Wi-Fi、本地环回等）上的连接请求。
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCVPORT));

    if(bind(sd,(void *)&laddr, sizeof(laddr)) < 0) //为socket绑定端口
    {
        perror("bind");
        exit(1);
    }

    /*!!!非常重要，否则第一次接收会错误*/
    raddr_len = sizeof(raddr);

    while(1)
    {
        recvfrom(sd, &rbuf, sizeof(rbuf), 0, (void *)&raddr, &raddr_len);
        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        printf("MESSAGE FROM %s:%d\n", ipstr, ntohs(raddr.sin_port));
        printf("NAME = %s\n", rbuf.name);
        printf("MATH = %d\n", ntohl(rbuf.math));
        printf("CHINESE = %d\n", ntohl(rbuf.chinese));
    }

    close(sd);

    exit(0);
}