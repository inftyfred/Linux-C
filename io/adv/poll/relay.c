#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define BUFFSIZE 1024

enum
{
    STATE_R = 1,
    STATE_W,    //分割线以上，我们关心其跳转
    STATE_AUTO, //分割线，并不作为状态跳转
    STATE_EX,   //分割线以下，其跳转是不关心的
    STATE_T
};

struct fsm_st
{
    int state;
    int sfd;
    int dfd;
    int len;
    int pos;
    char buf[BUFFSIZE];
    char *err_msg;
};

//状态机驱动函数
static void fsm_driver(struct fsm_st *fsm)
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

static int max(int a, int b)
{
    return (a > b) ? a : b;
}

static void relay(int fd1, int fd2)
{
    int fd1_save, fd2_save;
    struct fsm_st fsm12, fsm21;
    fd_set rset, wset;

    fd1_save = fcntl(fd1,F_GETFL);//获取fd1的access mode
    fcntl(fd1, F_SETFL, fd1_save|O_NONBLOCK);//设置非阻塞模式

    fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd2_save|O_NONBLOCK);

    //有限状态机初始化
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;
    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    while (fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        //布置监视任务
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        if(fsm12.state == STATE_R)
        {
            FD_SET(fsm12.sfd, &rset);
        }
        else if(fsm12.state == STATE_W)
        {
            FD_SET(fsm12.dfd, &wset);
        }

        if(fsm21.state == STATE_R)
        {
            FD_SET(fsm21.sfd, &rset);
        }
        else if(fsm21.state == STATE_W)
        {
            FD_SET(fsm21.dfd, &wset);
        }
        //监视
        if(fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
        {
            if(select(max(fd1, fd2)+1, &rset, &wset, NULL, NULL) < 0)
            {
                if(errno == EINTR)
                    continue;
                perror("select");
                exit(1);
            }
        }
        //查看监视结果
        //根据监视结果 推动状态机
        //当大于AUTO状态时，我们也无条件推状态机
        if(FD_ISSET(fd1, &rset) || FD_ISSET(fd2, &wset) || fsm12.state > STATE_AUTO)
            fsm_driver(&fsm12);
        if(FD_ISSET(fd2, &rset) || FD_ISSET(fd1, &wset) || fsm21.state > STATE_AUTO)
            fsm_driver(&fsm21);
    }

    //恢复文件描述符原来的状态
    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);
}

int main()
{
    int fd1, fd2;
    
    fd1 = open(TTY1, O_RDWR);
    if(fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd1, "TTY1\n", 5);

    fd2 = open(TTY2, O_RDWR);
    if(fd2 < 0)
    {
        perror("open()");
        exit(0);
    }
    write(fd2, "TTY2\n", 5);

    relay(fd1, fd2);

    close(fd2);
    close(fd1);
    exit(0);
}