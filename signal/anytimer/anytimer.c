#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "anytimer.h"

typedef enum{
    RUNNING,
    CANCELED,
    OVER
}timer_state;
typedef void (*sighandler_t)(int);
static struct anytimer_st* at_job[JOB_MAX];
static int inited = 0;//保证signal只运行一次
static sighandler_t alrm_handler_save;

struct anytimer_st
{
    int sec;
    at_jobfun_t *func;
    void *arg;
    timer_state state;
};

static void alrm_handler(int s)
{
    int i;
    alarm(1);
    for(i = 0; i < JOB_MAX; i++)
    {
        if(at_job[i] != NULL && at_job[i]->state == RUNNING)
        {
            at_job[i]->sec--;
            if(at_job[i]->sec <= 0)
            {
                (at_job[i]->func)(at_job[i]->arg);//执行对应操作
                at_job[i]->state = OVER;
            }
        }
    }
}

static void module_unload(void)
{
    int i;
    signal(SIGALRM,alrm_handler_save);//恢复alrm信号原来的功能
    alarm(0);//关闭闹钟
    for(i = 0; i < JOB_MAX; i++)//销毁多个s
        free(at_job[i]);
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
    for(i = 0; i < JOB_MAX; i++)
    {
        if(at_job[i] == NULL)
            return i;
    }
    return -1;
}

/*
初始化模块
return 0 == 成功
    -1 == 失败
*/
int at_init(void)
{
    if(!inited)
    {
        module_load();
        inited = 1;
        return 0;
    }

    return -1;
}


/*
return value >=0 成功,返回任务ID
        == -EINVAL 失败，参数非法
        == -ENOSPC 失败，数组满
        == -ENOMEM 失败，内存空间不足
*/
int at_addjob(int sec, at_jobfun_t *jobp, void *arg)
{
    struct anytimer_st *at_ptr;
    int pos;
    if(sec <=0)
    {
        return -EINVAL;//数值非法
    }

    at_ptr = malloc(sizeof(*at_ptr));
    if(at_ptr == NULL)
    {
        return -ENOMEM;
    }
    at_ptr->sec = sec;
    at_ptr->func = jobp;
    at_ptr->arg = arg;

    pos = get_free_pos();
    if(pos < 0)
    {
        return -ENOSPC;
    }

    at_job[pos] = at_ptr;//放入队列
    at_ptr->state = RUNNING;

    return pos;//创建成功，返回pos
}

/*
*return value == 0 成功，指定任务已成功取消
*        == -EINVAL 失败，参数非法
*        == -EBUSY  失败，指定任务已完成
*        == -ECANCELED 失败，指定任务重复取消
*/
int at_canceljob(int id)
{
    if(id < 0 || at_job[id] == NULL)
    {
        return -EINVAL;
    }
    if(at_job[id]->state == CANCELED)
    {
        return -ECANCELED;
    }
    if(at_job[id]->state == OVER)
    {
        return -EBUSY;
    }
    at_job[id]->state = CANCELED;
    
    return 0;
}

/*
*return value == 0 成功，指定任务成功释放
*        == -EINVAL 失败，参数非法
*/
int at_waitjob(int id)
{
    if(id < 0 || at_job[id] == NULL)
        return -EINVAL;

    free(at_job[id]);
    at_job[id] = NULL;

    return 0;
}
