#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "relayer.h"

#define BUFFSIZE 1024

enum
{
    STATE_R = 1,
    STATE_W,
    STATE_EX,
    STATE_T
};

struct rel_fsm_st
{
    int state;
    int sfd;
    int dfd;
    int len;
    int pos;
    char buf[BUFFSIZE];
    char *err_msg;
    int64_t count;
};

struct rel_job_st
{
    int fd1;
    int fd2;
    int job_state;
    struct rel_fsm_st fsm12, fsm21;
    int fd1_save, fd2_save;
    //struct timerval start end;
};

static struct rel_job_st* rel_job[REL_JOBMAX];
static pthread_mutex_t mut_rel_job = PTHREAD_MUTEX_INITIALIZER;//保证job的多线程访问
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

//状态机驱动函数
static void fsm_driver(struct rel_fsm_st *fsm)
{
    int ret;

    switch (fsm->state)
    {
    case STATE_R:
        fsm->len = read(fsm->sfd, fsm->buf, BUFFSIZE);
        if(fsm->len == 0)
            fsm->state = STATE_T;
        else if(fsm->len < 0)
        {
            if(errno == EAGAIN) //假错
                fsm->state = STATE_R;
            else
            {
                fsm->err_msg = "read()";
                fsm->state = STATE_EX;
            }
        }
        else
        {
            fsm->pos = 0;
            fsm->state = STATE_W;
        }
        break;
    case STATE_W:
        ret = write(fsm->dfd, fsm->buf+fsm->pos, fsm->len);
        if(ret < 0)
        {
            if(errno == EAGAIN)
                fsm->state = STATE_W;
            else
            {
                fsm->err_msg = "write()";
                fsm->state = STATE_EX;
            }
        }
        else
        {
            fsm->pos += ret;
            fsm->len -= ret;
            if(fsm->len == 0)
                fsm->state = STATE_R;
            else
                fsm->state = STATE_W;
        }
        break;
    case STATE_EX:
        perror(fsm->err_msg);
        fsm->state = STATE_T;
        break;
    case STATE_T:
        /*do sth*/
        break;
    default:
        abort();//异常，信号打断，并保存现场
        break;
    }
}


int get_free_pos_unlocked(void)
{
    int i;
    for(i = 0; i < REL_JOBMAX; i++)
    {
        if(rel_job[i] == NULL)
            return i;
    }

    return -1;
}

static void *thr_relayer(void *p)
{
    int i;
    
    while(1)
    {
        pthread_mutex_lock(&mut_rel_job);
        for(i = 0; i < REL_JOBMAX; i++)
        {
            if(rel_job[i] != NULL)
            {
                if(rel_job[i]->job_state == STATE_RUNNING)
                {
                    fsm_driver(&(rel_job[i]->fsm12));
                    fsm_driver(&(rel_job[i]->fsm21));
                    if(rel_job[i]->fsm12.state == STATE_T && \
                        rel_job[i]->fsm21.state == STATE_T)
                    {
                        rel_job[i]->job_state = STATE_OVER;
                    }
                }
            }
        }
        pthread_mutex_unlock(&mut_rel_job);
    }
}

//module_unload

static void module_load(void)
{
    pthread_t tid_relayer;
    int err;

    err = pthread_create(&tid_relayer, NULL, thr_relayer, NULL);
    if(err)
    {
        //fprintf();
        exit(1);
    }
}

int rel_addjob(int fd1, int fd2)
{
    struct rel_job_st *me;

    pthread_once(&init_once, module_load);//单次运行

    int pos;
    me = malloc(sizeof(*me));
    if(me == NULL)
        return -ENOMEM;
    
    me->fd1 = fd1;
    me->fd2 = fd2;
    me->job_state = STATE_RUNNING;
    
    me->fd1_save = fcntl(me->fd1, F_GETFL);
    fcntl(me->fd1, F_SETFL, me->fd1_save | O_NONBLOCK);
    me->fd2_save = fcntl(me->fd2, F_GETFL);
    fcntl(me->fd2, F_SETFL, me->fd2_save | O_NONBLOCK);

    me->fsm12.sfd = me->fd1;
    me->fsm12.dfd = me->fd2;
    me->fsm12.state = STATE_R;

    me->fsm21.sfd = me->fd2;
    me->fsm21.dfd = me->fd1;
    me->fsm21.state = STATE_R;

    pthread_mutex_lock(&mut_rel_job);
    pos = get_free_pos_unlocked();
    if(pos < 0)
    {
        pthread_mutex_unlock(&mut_rel_job);
        fcntl(me->fd1, F_SETFL, me->fd1_save);
        fcntl(me->fd2, F_SETFL, me->fd2_save);
        free(me);
        return -ENOSPC;
    }
    rel_job[pos] = me;
    pthread_mutex_unlock(&mut_rel_job);

    return pos;
}

#if 0
int rel_canceljob(int id)
{

}


int rel_waitjob(int id, struct rel_stat_st *);
/*
*   return  ==   0          成功
*           == -EINVAL      失败，参数非法
*
*/

int rel_statjob(int id, struct rel_stat_st *)
#endif
