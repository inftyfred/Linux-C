#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define BUFFSIZE 1024

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
    char rbuf[BUFFSIZE];

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    //bind();

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(80);
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

    fprintf(fp, "ping baidu.com\r\n\r\n");//GET /test.jpg
    fflush(fp);

    while(1)
    {
        int len;
        len = fread(rbuf, 1, BUFFSIZE, fp);
        if(len <= 0)
            break;
        fwrite(rbuf, 1, len, stdout);
    }

    fclose(fp);

    exit(0);
}