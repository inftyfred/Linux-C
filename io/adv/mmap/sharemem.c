#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

/*
通过共享内存实现父子进程之间的通信
*/

#define MEMSIZE 1024

int main()
{
    char *ptr;
    pid_t pid;

    ptr = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }


    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        munmap(ptr, MEMSIZE);
        exit(1);
    }

    if(pid == 0) //child write
    {
        strcpy(ptr, "hello!");
        munmap(ptr, MEMSIZE);
        exit(0);
    }
    else    //parent read
    {
        wait(NULL);
        puts(ptr);
        munmap(ptr, MEMSIZE);
        exit(0);
    }

}