#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

extern char **environ;

/*
打印时戳 date +%s
*/

int main()
{
    puts("begin!");
    fflush(NULL);//刷新所有缓冲区，保证写入文件时刷新

    execl("/usr/bin/date","date","+%s",NULL);//将执行制定文件
    perror("execl()");//如果能执行到这里，说明一定出错
    exit(1);

    puts("end!");//程序正常运行，就不会运行到这里
    exit(0);
}