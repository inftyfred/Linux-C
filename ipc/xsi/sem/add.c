#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

/*
20个进程同时写 文件锁解决竞争
*/

#define PRONUM 20
#define FNAME "/tmp/out"
#define LINESIZE 1024

static int semid;

static void P() //取资源量
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -1; //减去一个资源量
    op.sem_flg = 0; //无特殊操作

    while(semop(semid, &op, 1) < 0)
    {
        if(errno != EINTR && errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }
    }
}

static void V() //还资源量
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1;  //增加一个资源量
    op.sem_flg = 0;

    if(semop(semid, &op, 1) < 0)
    {
        if(errno != EINTR && errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }    
    }
}

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

    P();//取一个资源量
    fgets(linebuf, LINESIZE, fp);
    fseek(fp, 0, SEEK_SET);//定位到文件首
    fprintf(fp, "%d\n", atoi(linebuf)+1);
    sleep(1);
    fflush(fp); //刷新流，保证文件写入成功
    V();//增加一个资源量
    
    fclose(fp);//关闭文件

    return;
}

int main()
{
    int i, err;
    pid_t pid;

    //匿名key值:IPC_PRIVATE，用于父子进程通信，
    //此时函数会自动创建sem数组，无需指定IPC CREATE
    semid = semget(IPC_PRIVATE, 1, 0600);   
    
    if(semid < 0)
    {
        perror("semget()");
        exit(1);
    }

    if(semctl(semid, 0, SETVAL, 1) < 0) //设置sem数组下标为0的值为1
    {
        perror("semctl()");
        exit(1);
    }

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

    semctl(semid, 0, IPC_RMID);//删除semid 0

    exit(0);
}