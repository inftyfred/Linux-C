#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define CPS 10 //每次打印数量
#define BUFFSIZE CPS

static volatile int loop = 0;

static void alrm_handler(int s)
{
	alarm(1);//alarm链
	loop = 1;
}


int main(int argc, char **argv)
{

    int fd1, fd2=1;
    char buf[BUFFSIZE];
    int len, ret;

    if(argc < 2)
    {
        fprintf(stderr,"Usage:%s <srcfile> <cpfile>",argv[0]);
        exit(1);
    }

	signal(SIGALRM,alrm_handler);
	alarm(1);

    fd1 = open(argv[1], O_RDONLY);//只读打开
    if(fd1 < 0)
    {
        perror("open:");
        exit(1);
    }
    if(fd2 < 0)
    {
        perror("open:");
        exit(1);
    }

    while(1)
    {
		while(!loop)
			pause();//等待信号，防止盲等

		loop = 0;

        while(len = read(fd1,buf,BUFFSIZE) < 0)
        {
			if(errno == EINTR)
				continue;//表示由信号打断，重新执行read操作
            perror("read:（）");
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

    exit(0);
}
