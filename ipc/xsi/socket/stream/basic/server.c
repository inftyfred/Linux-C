#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include "proto.h"

#define BUFFSIZE    1024

static void server_job(int sd)
{
    char buf[BUFFSIZE];
    int len;

    len = sprintf(buf, FMT_STAMP, (long long)time(NULL));
    if(send(sd, buf, len, 0) < 0)
    {
        perror("send()");
        exit(1);
    }
}

int main()
{
    int sd, newsd;
    struct sockaddr_in laddr;
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];

    sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);//流式传输,默认协议
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    //SO_REUSEADDR:如果地址被重复绑定，则重新绑定而不会报错
    int val = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsocket()");
        exit(1);
    }

    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));

    if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    if(listen(sd, 200) < 0)//200:代表能够监听的最多的C端个数
    {
        perror("listen()");
        exit(1);
    }

    raddr_len = sizeof(raddr);
    
    while(1)
    {
        newsd = accept(sd, (void *)&raddr, &raddr_len);
        if(newsd < 0)
        {
            perror("accept()");
            exit(1);
        }

        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        printf("Client:%s:%d\n", ipstr, ntohs(raddr.sin_port));

        server_job(newsd);

        close(newsd);
    }

    close(sd);

    exit(0);
}