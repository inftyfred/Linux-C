#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
/*
筛选质数 添加wait
*/
#define LEFT  30000000
#define RIGHT 30000200

int main()
{
    int i,j,mark;
    pid_t pid;
    for(i = LEFT; i <= RIGHT; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1); 
        }
        if(pid == 0) //child
        {
            mark = 1;
            for(j = 2; j < i/2; j++)
            {
                if(i % j == 0)
                {
                    mark = 0;
                    break;
                }
            }
            if(mark)
            {
                printf("%d is a primer\n",i);
            }
            exit(0);//child exit
        }
    }

    for(i = LEFT; i <= RIGHT; i++)
    {
        wait(NULL);//释放所有子进程 不关心返回的状态
    }


    exit(0);
}