#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

/*
筛选质数
*/
#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM 4 //线程数

//任务状态 0:无任务 -1：任务结束 >0:有任务
static int num = 0;
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

static void *thr_prime(void *p);

int main()  
{
    int i;
    int err;
    pthread_t tid[THRNUM];

    for(i = 0; i < THRNUM; i++)
    {
        err = pthread_create((tid + i), NULL, thr_prime, (void *)i);
        if(err)
        {
            fprintf(stderr, "p creat%s\n", strerror(err));
            exit(1);
        }
    }

    //main线程 下发任务
    for(i = LEFT; i < RIGHT; i++)
    {
        pthread_mutex_lock(&mut_num);
        while(num != 0) //说明此时没有线程拿到数字
        {   //等待法
            pthread_mutex_unlock(&mut_num);//解锁，让其它线程可以拿到
            sched_yield();//出让调度器给其它线程，相当于一个小的sleep，等待其它线程拿数字
            pthread_mutex_lock(&mut_num);
        }
        num = i;
        pthread_mutex_unlock(&mut_num);
    }

    pthread_mutex_lock(&mut_num);
    while(num != 0)
    {
        pthread_mutex_unlock(&mut_num);
        sched_yield();
        pthread_mutex_lock(&mut_num);
    }
    num = -1;//退出状态
    pthread_mutex_unlock(&mut_num);


    for(i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mut_num);
    exit(0);
}

static void *thr_prime(void *p)
{
    int i, j, mark;

    while(1)
    {
        pthread_mutex_lock(&mut_num);
        //临界区：注意跳转状态
        while(num == 0)
        {
            pthread_mutex_unlock(&mut_num);
            sched_yield();
            pthread_mutex_lock(&mut_num);
        }
        if(num == -1) //无任务，结束循环
        {
            pthread_mutex_unlock(&mut_num);//先解锁再跳出，防止死锁****
            break;
        }
        //临界区：跳转出临界区时应该先解锁再退出
        i = num;
        num = 0;
        pthread_mutex_unlock(&mut_num);

        mark = 1;
        for(j = 2; j < i/2; j++)
        {
            if(i % j == 0)
            {
                mark = 0;
                break;
            }
        }
        if(mark)
        {
            printf("%d is a primer by %d thread\n",i, (int)p);
        }
    }
    pthread_exit(NULL);
}