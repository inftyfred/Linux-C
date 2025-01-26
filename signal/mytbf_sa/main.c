#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "mytbf.h"

#define CPS 30 //每次打印数量
#define BUFFSIZE 1024
#define BURST 100  //令牌桶上限

static volatile int token = 0;

int main(int argc, char **argv)
{

    int fd1, fd2=1;
    char buf[BUFFSIZE];
    int len, ret;
    int size;
    mytbf_st *tbf;

    if(argc < 2)
    {
        fprintf(stderr,"Usage:%s <srcfile> <cpfile>",argv[0]);
        exit(1);
    }

	tbf = mytbf_init(CPS,BURST);
    if(tbf == NULL)
    {
        fprintf(stderr,"mytbf_init error\n");
        exit(1);
    }

    fd1 = open(argv[1], O_RDONLY);//只读打开
    if(fd1 < 0)
    {
        perror("open:");
        exit(1);
    }

    while(1)
    {
        size = mytbf_fetchtoken(tbf,BUFFSIZE);//能够拿到的token数
        if(size <= 0)
        {
            fprintf(stderr,"mytbf_fetchtoken():%s\n",strerror(-size));
            exit(1);
        }

        while((len = read(fd1,buf,size)) < 0)
        {
			if(errno == EINTR)
				continue;//表示由信号打断，重新执行read操作
            perror("read:（）");
            goto out;
        }
        if(len == 0)
            break;

        if(size - len > 0)
        {
            mytbf_returntoken(tbf,size-len);//归还token
        }

        ret = write(fd2,buf,len);
        if(ret < 0)
        {
            perror("write:");
            break;
        }
    }

out:
    mytbf_destroy(tbf);
    close(fd1);
    exit(0);
}
