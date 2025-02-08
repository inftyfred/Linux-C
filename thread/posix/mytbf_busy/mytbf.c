#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "mytbf.h"

static struct mytbf_st* job[MYTBF_MAX];//加锁才能修改
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_t tid_alrm;//线程标识
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut; 
    //token锁 由于在多个线程中都对token进行了修改 所以需要为其设置一个锁
};

static void *thr_alrm(void *p)
{
    int i;
    while(1)
    {
        pthread_mutex_lock(&mut_job);//job数组的锁
        for(i = 0; i < MYTBF_MAX; i++)
        {
            if(job[i] != NULL)
            {
                pthread_mutex_lock(&job[i]->mut);//token的锁
                job[i]->token += job[i]->cps;
                if(job[i]->token > job[i]->burst)
                {
                    job[i]->token = job[i]->burst;
                }
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);

        sleep(1); //1s中进行一次操作
    }
}

static void module_unload(void)
{
    int i;
    //线程收尸：先cancle后join
    pthread_cancel(tid_alrm);
    pthread_join(tid_alrm, NULL);

    for(i = 0; i < MYTBF_MAX; i++)//销毁多个令牌桶
    {
        if(job[i] != NULL)
        {
            mytbf_destroy(job[i]);
        }
    }
    
    pthread_mutex_destroy(&mut_job);//删除job锁
}


static void module_load(void)
{
    int err;

    err = pthread_create(&tid_alrm, NULL, thr_alrm, NULL);
    if(err)
    {
        fprintf(stderr, "p create:%s\n", strerror(err));
        exit(1);
    }
    
    atexit(module_unload);//钩子函数
}

//unlocked代表该函数在调用前应该先加锁
static int get_free_pos_unlocked(void)
{
    int i;
    for(i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] == NULL)
            return i;
    }
    return -1;
}
static int min(int a, int b)
{
    if(a < b)
        return a;

    return b;
}


mytbf_st *mytbf_init(int cps, int burst)
{
    struct mytbf_st *me;
    int pos;

    pthread_once(&init_once, module_load);//保证module_load只被执行一次

    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    pthread_mutex_init(&me->mut, NULL);
    
    pthread_mutex_lock(&mut_job); 
    //临界区 临界区应该尽可能的短
    pos = get_free_pos_unlocked();
    if(pos < 0) //注意临界区内的跳转语句
    {
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
        

    me->pos = pos;
    
    job[pos] = me;
    //临界区
    pthread_mutex_unlock(&mut_job); 

    return me;
}

int mytbf_fetchtoken(mytbf_st *ptr, int size)
{
    struct mytbf_st *me = ptr;
    int n;

    if(size <= 0)
        return -EINVAL;

    pthread_mutex_lock(&me->mut);
    while (me->token <= 0) //如果没有token，则等待
    {
        pthread_mutex_unlock(&me->mut);
        sched_yield();
        pthread_mutex_lock(&me->mut);
    }
    n = min(me->token,size);
    me->token -= n;
    pthread_mutex_unlock(&me->mut);

    return n;
}

int mytbf_returntoken(mytbf_st *ptr, int size)
{
    struct mytbf_st *me = ptr;

    if(size <= 0)
        return -EINVAL;

    pthread_mutex_lock(&me->mut);
    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;
    pthread_mutex_unlock(&me->mut);

    return size;
}

//销毁一个令牌桶
int mytbf_destroy(mytbf_st *ptr)
{
    struct mytbf_st *me = ptr;

    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);
    pthread_mutex_destroy(&me->mut);
    free(ptr);

    return 0;
}



