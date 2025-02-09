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
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;

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
        {   //通知法
            pthread_cond_wait(&cond_num, &mut_num);
        }
        num = i;
        pthread_cond_signal(&cond_num);//叫醒一个
        pthread_mutex_unlock(&mut_num);
    }

    pthread_mutex_lock(&mut_num);
    while(num != 0)
    {
        //通知法：等待最后一个数被拿走
        pthread_cond_wait(&cond_num, &mut_num);
    }
    num = -1;//退出状态
    pthread_cond_broadcast(&cond_num);//通知所有
    pthread_mutex_unlock(&mut_num);


    for(i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mut_num);
    pthread_cond_destroy(&cond_num);

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
            //等待num变为非0的通知
            pthread_cond_wait(&cond_num, &mut_num);
        }
        if(num == -1) //无任务，结束循环
        {
            pthread_mutex_unlock(&mut_num);//先解锁再跳出，防止死锁****
            break;
        }
        //临界区：跳转出临界区时应该先解锁再退出
        i = num;
        num = 0;
        pthread_cond_broadcast(&cond_num); //通知所有
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