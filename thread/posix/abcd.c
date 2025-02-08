#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THRNUM 4

static pthread_mutex_t mut[THRNUM];

static int next(int n)
{
    if(n + 1 == THRNUM)
        return 0;
    return n + 1;
}

static void *thr_func(void *p)
{
    int n = (int)p;
    int c = 'a' + n;

    while (1)
    {   //锁链
        //四个线程，在第一次进入该函数时，都在尝试锁自己
        //由于在创建线程时就将其锁住，所以都处于阻塞状态
        //当我们把第一个锁解开时，才可以正常运行
        pthread_mutex_lock(mut+n); 
        write(1, &c, 1);
        pthread_mutex_unlock(mut+next(n));
    }
    
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[THRNUM];
    int i, err;

    for(i = 0; i < THRNUM; i++)
    {
        pthread_mutex_init(mut+i, NULL);
        pthread_mutex_lock(mut+i); //全部上锁
        err = pthread_create(tid+i, NULL, thr_func, (void *)i);
        if(err)
        {
            exit(1);
        }
    }

    pthread_mutex_unlock(mut);//解开第一个锁

    alarm(5);

    for(i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    exit(0);
}