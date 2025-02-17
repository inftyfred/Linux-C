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

#define FILESTREAM //TCPSTREAM

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    int sd;
    FILE *fp;
    struct sockaddr_in raddr;
    long long stamp;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    //bind();

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, argv[1], &raddr.sin_addr);

    printf("begin connect\n");

    if(connect(sd, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("connect()");
        exit(1);
    }

    printf("connect done\n");

#ifdef  FILESTREAM  //FILE *处理
    fp = fdopen(sd, "r+");//一切皆文件
    if(fp == NULL)
    {
        perror("fdopen()");
        exit(1);
    }

    if(fscanf(fp, FMT_STAMP, &stamp) < 1)
    {
        fprintf(stderr, "bad format!\n");
    }
    else
    {
        fprintf(stdout, "stamp = %lld\n", stamp);
    }

    fclose(fp);
#elif defined(TCPSTREAM)
    //sprintf(stamp, FMT_STAMP);
    if(recv(sd, &stamp, sizeof(stamp), 0) < 0)
    {
        perror("recv");
        exit(1);
    }
    
    fprintf(stdout, "stamp = %lld\n", stamp);
    
    close(sd);
#endif
    exit(0);
}