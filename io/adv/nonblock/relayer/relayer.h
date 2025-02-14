#ifndef RELAYER_H__
#define RELAYER_H__

#define REL_JOBMAX 100000

enum
{
    STATE_RUNNING=1,
    STATE_CANCELD,
    STATE_OVER
};

struct rel_stat_st
{
    int state;
    int fd1;
    int fd2;
    int64_t count12, count21;
    //struct timerval start, end;
};

int rel_addjob(int fd1, int fd2);
/*
* return >=0            成功，返回当前任务ID
*        ==0            失败，参数非法
*        ==-ENOSPC      失败，任务数组满
*        ==-ENOMEM      失败，内存分配有误
**/ 

int rel_canceljob(int id);
/*
*   return  ==  0           成功，指定任务成功取消
*           ==  -EINVAL     失败，参数非法
*           ==  -EBUSY      失败，任务已经被取消
*/

int rel_waitjob(int id, struct rel_stat_st *);
/*
*   return  ==   0          成功
*           == -EINVAL      失败，参数非法
*
*/

int rel_statjob(int id, struct rel_stat_st *);
/*
*   return  ==  0       成功
*           == -EINVAL  失败，参数非法
*
**/


#endif