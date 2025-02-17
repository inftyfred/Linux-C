#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <bits/sigaction.h>
#include <sys/mman.h>
#include <bits/mman-linux.h>
#include <errno.h>
#include <bits/types/sigset_t.h>
#include <asm-generic/signal-defs.h>
//#include <asm-generic/signal.h>

#include "proto.h"

/*
*进程池
*/

#define MINSPARESERVER  5   //最小空闲进程
#define MAXSPARESERVER  10  //最大空闲进程
#define MAXCLIENTS      20  //最多的CLIENT数量
#define SIG_NOTIFY      SIGUSR2 //信号选择
#define LINEBUFFSIZE    128

typedef enum
{
    STATE_IDLE = 0,
    STATE_BUSY
}PROC_STATE;

struct server_st
{
    pid_t pid;  //default -1
    PROC_STATE state;
//  int reuse;  //进程的重复使用次数， 防止一个进程运行时间过长
};

static struct server_st *serverpool;//
static int idle_count = 0, busy_count = 0;
static int sd;

static void usr2_handler(int s)
{
    return;
}

static void server_job(int pos)
{
    pid_t ppid;
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    int client_sd;
    int len;
    long long stamp;
    char linebuf[LINEBUFFSIZE];
    char ipstr[IPSTRSIZE];

    ppid = getppid();

    while (1)
    {
        serverpool[pos].state = STATE_IDLE;
        kill(ppid, SIG_NOTIFY);

        client_sd = accept(sd, (void *)&raddr, &raddr_len);
        if(client_sd < 0)
        {
            if(errno != EINTR || errno != EAGAIN)
            {
                perror("accept()");
                exit(1);
            }
        }

        serverpool[pos].state = STATE_BUSY;
        kill(ppid, SIG_NOTIFY);
        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
        //printf("[%d]client:%s:%d\n", getpid(), ipstr, ntohs(raddr.sin_port));

        stamp = time(NULL);
        len = snprintf(linebuf, LINEBUFFSIZE, FMT_STAMP,stamp);
        send(client_sd, linebuf, len, 0);
        /*  if error  */

        sleep(5);

        close(client_sd);
    }
    
}

static int add_1_server(void)
{
    int slot;
    pid_t pid;

    if(idle_count + busy_count >= MAXCLIENTS)
        return -1;
    for(slot = 0; slot < MAXCLIENTS; slot++)
    {
        if(serverpool[slot].pid == -1)
        {
            break;
        }
    }

    serverpool[slot].state = STATE_IDLE;
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if(pid == 0)    //child
    {
        server_job(slot);
        exit(0);
    }
    else
    {
        serverpool[slot].pid = pid;
        idle_count++;
    }

    return 0;
}

static int del_1_server(void)
{
    int i;

    if(idle_count == 0)
        return -1;
    
    for(i = 0; i < MAXCLIENTS; i++)
    {
        if(serverpool[i].pid != -1 && serverpool[i].state == STATE_IDLE)
        {
            kill(serverpool[i].pid, SIGTERM);
            serverpool[i].pid = -1;
            idle_count--;
            break;
        }
    }

    return 0;
}

static int scan_pool(void)
{
    int i;
    int busy = 0, idle = 0;

    for(i = 0; i < MAXCLIENTS; i++)
    {
        if(serverpool[i].pid == -1)
            continue;
        if(kill(serverpool[i].pid, 0))
        {
            serverpool[i].pid = -1;
            continue;
        }
        if(serverpool[i].state == STATE_IDLE)
        {
            idle++;
        }
        else if(serverpool[i].state == STATE_BUSY)
        {
            busy++;
        }
        else
        {
            fprintf(stderr, "scan_pool():Unknown state.\n");
            abort();
        }
    }

    idle_count = idle;
    busy_count = busy;

    return 0;
}

int main(int argc, char **argv)
{
    int i, val = 1;
    struct sockaddr_in laddr;
    struct sigaction sa, osa;
    sigset_t set, oset;

    //不用对子进程收尸，自行消亡
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa, &osa);

    sa.sa_handler = usr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &osa);

    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &set, &oset);//先忽略信号

    //mmap动态分配地址，相当于malloc
    serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLIENTS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(serverpool == NULL)
    {
        perror("mmap()");
        exit(1);
    }

    for(i = 0; i < MAXCLIENTS; i++)
    {
        serverpool[i].pid = -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsocket()");
        exit(1);
    }

    inet_pton(AF_INET, "0.0.0.0",&laddr.sin_addr);
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    if(listen(sd, 100) < 0)
    {
        perror("listen()");
        exit(1);
    }

    for(i = 0; i < MINSPARESERVER; i++)
    {
        add_1_server();
    }

    while(1)
    {
        //信号驱动函数 
        //以oset状态挂起，等待一个信号：SIG_NOTIFY
        sigsuspend(&oset);

        scan_pool();

        //contrl the pool
        if(idle_count > MAXSPARESERVER)
        {
            for(i = 0; i < idle_count - MAXSPARESERVER; i++)
            {
                del_1_server();
            }
        }
        else if(idle_count < MINSPARESERVER)
        {
            for(i = 0; i < (MINSPARESERVER - idle_count); i++)
            {
                add_1_server();
            }
        }

        for(i = 0; i < MAXCLIENTS; i++)
        {
            if(serverpool[i].pid == -1)
            {
                putchar(' ');
            }
            else if(serverpool[i].state == STATE_IDLE)
            {
                putchar('.');
            }
            else
            {
                putchar('x');
            }    
        }
        putchar('\n');
    }

    sigprocmask(SIG_SETMASK, &oset, NULL);

    exit(0);
}