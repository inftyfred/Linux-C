#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

static void *func(void *p)
{
    puts("thread is working!");
    //return NULL;
    pthread_exit(NULL);//结束进程
}

int main()
{
    pthread_t tid;
    int err;

    puts("begin!");

    err = pthread_create(&tid, NULL, func, NULL);
    if(err)
    {
        fprintf(stderr, "pthread_create():%s\n", strerror(err));
        exit(1);
    }

    pthread_join(tid, NULL);//收尸
    puts("end!");
    exit(0);
}