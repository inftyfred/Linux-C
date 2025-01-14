#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFSIZE 1024

int main(int argc, char **argv)
{

    int fd1, fd2;
    char buf[BUFFSIZE];
    int len, ret;

    if(argc < 3)
    {
        fprintf(stderr,"Usage:%s <srcfile> <cpfile>",argv[0]);
        exit(1);
    }

    fd1 = open(argv[1], O_RDONLY);//只读打开
    if(fd1 < 0)
    {
        perror("open:");
        exit(1);
    }
    fd2 = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC,0600);
    if(fd2 < 0)
    {
        perror("open:");
        exit(1);
    }

    while(1)
    {
        len = read(fd1,buf,BUFFSIZE);
        if(len < 0)
        {
            perror("read:");
            break;
        }
        if(len == 0)
            break;

        ret = write(fd2,buf,len);
        if(ret < 0)
        {
            perror("write:");
            break;
        }
    }

    close(fd1);
    close(fd2);

    exit(0);
}
