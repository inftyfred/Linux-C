#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "mytbf.h"

typedef void (*sighandler_t)(int);
static struct mytbf_st* job[MYTBF_MAX];
static int inited = 0;//保证signal只运行一次
static sighandler_t alrm_handler_save;

struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
};

static void alrm_handler(int s)
{
    int i;
    alarm(1);
    for(i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL)
        {
            job[i]->token += job[i]->cps;
            if(job[i]->token > job[i]->burst)
            {
                job[i]->token = job[i]->burst;
            }
        }
    }
}

static void module_unload(void)
{
    int i;
    signal(SIGALRM,alrm_handler_save);//恢复alrm信号原来的功能
    alarm(0);//关闭闹钟
    for(i = 0; i < MYTBF_MAX; i++)//销毁多个令牌桶
        free(job[i]);
}


static void module_load(void)
{
    alrm_handler_save = signal(SIGALRM,alrm_handler);
    alarm(1);

    atexit(module_unload);//钩子函数
}

static int get_free_pos(void)
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

    if(!inited)
    {
        module_load();
        inited = 1;
    }

    pos = get_free_pos();
    if(pos < 0)
        return NULL;

    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;
    job[pos] = me;

    return me;
}

int mytbf_fetchtoken(mytbf_st *ptr, int size)
{
    struct mytbf_st *me = ptr;
    int n;

    if(size <= 0)
        return -EINVAL;

    while (me->token <= 0)
    {
        pause();
    }
    n = min(me->token,size);
    me->token -= n;

    return n;
}

int mytbf_returntoken(mytbf_st *ptr, int size)
{
    struct mytbf_st *me = ptr;

    if(size <= 0)
        return -EINVAL;

    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;

    return size;
}
//销毁一个令牌桶
int mytbf_destroy(mytbf_st *ptr)
{
    struct mytbf_st *me = ptr;

    job[me->pos] = NULL;
    free(ptr);

    return 0;
}



