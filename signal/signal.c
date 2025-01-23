#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void int_handler(int s)
{
    write(1,"!",1);
}

int main()
{
    int i;

    //signal(SIGINT,SIG_IGN);//忽略sigint信号，即ctrl+c  
    signal(SIGINT,int_handler);//ctrl+c后会调用int_handler

    for(i = 0; i < 10; i++)
    {
        write(1,"*",1);
        sleep(1);
    }

    exit(0);
}