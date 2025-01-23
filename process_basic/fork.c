#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
    pid_t pid;

    printf("[%d]Begin!\n",getpid());

    fflush(NULL);//在fork前 刷新当前所有的流，非常重要！！！

    pid = fork();//产生了子进程

    if(pid < 0)
    {
        perror("fork():");
        exit(1);
    }
    
    if(pid == 0) //child
    {
        printf("[%d],child is working!\n",getpid());
    }
    else  //parent
    {
        printf("[%d],parent is working!\n",getpid());
    }

    printf("[%d]:End!\n",getpid());
    exit(0);
}
