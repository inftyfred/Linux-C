#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
20个进程同时写 文件锁解决竞争
*/

#define PRONUM 20
#define FNAME "/tmp/out"
#define LINESIZE 1024

static void func_add(void)
{
    FILE *fp;
    int fd;
    char linebuf[LINESIZE];
    fp = fopen(FNAME, "r+");
    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    fd = fileno(fp);
    /* if error */

    lockf(fd, F_LOCK, 0);
    fgets(linebuf, LINESIZE, fp);
    fseek(fp, 0, SEEK_SET);//定位到文件首
    fprintf(fp, "%d\n", atoi(linebuf)+1);
    sleep(1);
    fflush(fp); //刷新流，保证文件写入成功
    lockf(fd, F_ULOCK, 0);
    fclose(fp);//关闭文件

    return;
}

int main()
{
    int i, err;
    pid_t pid;

    for(i = 0; i < PRONUM; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0) //child 
        {
            func_add();

            exit(0);
        }
    }

    for(i = 0; i < PRONUM; i++)
    {
        wait(NULL);
    }

    exit(0);
}