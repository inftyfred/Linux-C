#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        close(pd[1]);//关闭写端
        int len = read(pd[0], buf, BUFFSIZE);
        write(1, buf, len);
        close(pd[0]);   //读完毕 关闭读端
        exit(0);
    }
    else    //parent write
    {
        close(pd[0]);   //关闭读端
        write(pd[1], "hello!\n", 7);
        close(pd[1]);   //写完毕 关闭写端
        wait(NULL);     //收尸体
        exit(0);
    }
}