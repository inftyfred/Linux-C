#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define CPS 10 //每次打印数量
#define BUFFSIZE CPS
#define BURST 100  //令牌桶上限

static volatile sig_atomic_t token = 0;//sig_atomic_t:保证token--的操作是一条指令完成的（原子操作）

static void alrm_handler(int s)
{
	alarm(1);//alarm链
	token++;
    if(token > BURST)
        token = BURST;
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

    while(1)
    {
		while(token <= 0)
			pause();//等待信号，防止盲等

		token--;

        while((len = read(fd1,buf,BUFFSIZE)) < 0)
        {
			if(errno == EINTR)
				continue;//表示由信号打断，重新执行read操作
            perror("read:（）");
            goto out;
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

out:
    close(fd1);
    exit(0);
}
