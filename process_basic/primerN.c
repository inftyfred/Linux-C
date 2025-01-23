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
#define N 3 //进程数

int main()
{
    int i,j,mark,n;
    pid_t pid;

    for(n = 0; n < N; n++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            //wait(NULL); 应该释放创建的子进程
            exit(1); 
        }
        if(pid == 0)
        {
            for(i = LEFT+n; i <= RIGHT; i+=N)//交叉分配
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
                    printf("[p:%d]%d is a primer\n",n,i);
                }
            }
            exit(0);//child exit
        }
    }

    for(n = 0; n <= N; n++)
    {
        wait(NULL);//释放所有子进程 不关心返回的状态
    }

    exit(0);
}