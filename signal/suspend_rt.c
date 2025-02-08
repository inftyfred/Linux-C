#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <bits/types/sigset_t.h>
#include <asm-generic/signal-defs.h>

#define MYRTSIG (SIGRTMIN+6)

static void mysig_handler(int s)
{
    write(1,"!",1);
}

int main()
{
    int i,j;
    sigset_t set,oldset,saveset;

    signal(MYRTSIG,mysig_handler);//ctrl+c后会调用int_handler
    sigemptyset(&set);
    sigaddset(&set,MYRTSIG);
    sigprocmask(SIG_UNBLOCK,&set,&saveset);//目的为了保存原来的状态
    sigprocmask(SIG_BLOCK,&set,&oldset);//设置阻塞
    for(i = 0; i < 1000; i++)
    {
        sigprocmask(SIG_BLOCK,&set,NULL);
        for(j = 0; j < 5; j++)
        {
            write(1,"*",1);
            sleep(1);
        }
        write(1,"\n",1);
        sigsuspend(&oldset);
    }
    sigprocmask(SIG_SETMASK,&saveset,NULL);//恢复原来的状态
    exit(0);
}