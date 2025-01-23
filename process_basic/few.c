#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/*
打印时戳 date +%s
*/

int main()
{
    pid_t pid;
    puts("begin!");
    fflush(NULL);//刷新所有缓冲区，保证写入文件时刷新

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0) //child
    {
        execl("/usr/bin/date","date","+%s",NULL);//将执行制定文件
        perror("execl()");//如果能执行到这里，说明一定出错
        exit(1);
    }

    wait(NULL);//父进程释放子进程

    puts("end!");//程序正常运行，就不会运行到这里
    exit(0);
}