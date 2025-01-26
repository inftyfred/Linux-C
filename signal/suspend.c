#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <bits/types/sigset_t.h>
#include <asm-generic/signal-defs.h>

static void int_handler(int s)
{
    write(1,"!",1);
}

int main()
{
    int i,j;
    sigset_t set,oldset,saveset;

    signal(SIGINT,int_handler);//ctrl+c后会调用int_handler
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
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
        /*
        以下语句相当于suspend，只不过suspend是原子化操作
        sigset_t tmpset;
        sigprocmask(SIG_SETMASK,&oldset,&tmpset);//解除阻塞
        pause();
        sigprocmask(SIG_SETMASK,&tmpset,NULL);
        */
    }
    sigprocmask(SIG_SETMASK,&saveset,NULL);//恢复原来的状态
    exit(0);
}