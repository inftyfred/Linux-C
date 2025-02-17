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

    if(connect(sd, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("connect()");
        exit(1);
    }

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
    //rcve();
    //close();

    exit(0);
}