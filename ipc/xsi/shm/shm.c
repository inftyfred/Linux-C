#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>

/*
通过共享内存实现父子进程之间的通信
子进程写，父进程读
*/

#define MEMSIZE 1024

int main()
{
    char *ptr;
    pid_t pid;
    int shmid;

    shmid = shmget(IPC_PRIVATE, MEMSIZE, 0600);
    if(shmid < 0)
    {
        perror("shmget()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)    //child     write
    {
        //NULL:自动寻找地址
        //0：无特殊要求
        ptr = shmat(shmid, NULL, 0);
        if(ptr == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }
        strcpy(ptr, "hello!");
        shmdt(ptr); //解除映射
        exit(0);
    }
    else            //parent    read
    {
        wait(NULL);
        ptr = shmat(shmid, NULL, 0);//映射子进程中的地址到ptr
        if(ptr == (void *)-1)
        {
            perror("");
            exit(1);
        }
        puts(ptr);
        shmdt(ptr);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }
}