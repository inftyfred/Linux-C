#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
/*
*使用匿名管道，子进程读管道，父进程写管道
*/
#define BUFFSIZE 1024

int main()
{
    int pd[2];
    pid_t pid;
    char buf[BUFFSIZE];

    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0) //child read
    {
        int fd;
        close(pd[1]); //关闭写端
        dup2(pd[0], 0);//重定向 读端到标准输入
        close(pd[0]); //重定向完毕 关闭读端
        fd = open("/dev/null", O_RDWR);
        dup2(fd, 1);
        dup2(fd, 2);
        execl("/usr/local/bin/mpg123", "mpg123", "-", NULL);

        perror("execl()");
        
        exit(1);
    }
    else    //parent write
    {
        close(pd[0]);   //关闭读端
        //父进程从网上收数据，往管道中写
        write(pd[1], "hello!\n", 7);

        close(pd[1]);   //写完毕 关闭写端
        wait(NULL);     //收尸体
        exit(0);
    }
}