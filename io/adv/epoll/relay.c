#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/epoll.h>

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
    int epfd;
    struct epoll_event ev;

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

    epfd = epoll_create(10);
    if(epfd < 0)
    {
        perror("epoll_create()");
        exit(1);
    }

    //初始化ev，将fd1、fd2注册到epfd中
    ev.events = 0;
    ev.data.fd = fd1; //epoll的精华所在
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &ev);

    ev.events = 0;
    ev.data.fd = fd2;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &ev);

    while (fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        //布置监视任务
        ev.data.fd = fd1;
        ev.events = 0;
        if(fsm12.state == STATE_R)
        {
            ev.events |= EPOLLIN; //1可读
        }
        if(fsm21.state == STATE_W)
        {
            ev.events |= EPOLLOUT; //1可写
        }
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd1, &ev);

        ev.data.fd = fd2;
        ev.events = 0;
        if(fsm12.state == STATE_W)
        {
            ev.events |= EPOLLOUT; //2可写
        }
        if(fsm21.state == STATE_R)
        {
            ev.events |= EPOLLIN; //2可读
        }
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd2, &ev);
        
        //监视
        if(fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
        {
            while(epoll_wait(epfd, &ev, 1, -1) < 0) //每次处理一个事件
            {
                if(errno == EINTR)
                    continue;
                perror("epoll_wait()");
                exit(1);
            }
        }
        //查看监视结果
        //根据监视结果 推动状态机
        //当大于AUTO状态时，我们也无条件推状态机
        if((ev.data.fd == fd1 && ev.events & EPOLLIN) \
            || (ev.events & EPOLLOUT && ev.data.fd == fd2) \
            || fsm12.state > STATE_AUTO)
            fsm_driver(&fsm12);
        if( (ev.events & EPOLLIN && ev.data.fd == fd2) \
            || (ev.events & EPOLLOUT && ev.data.fd == fd1) \
            || fsm21.state > STATE_AUTO)
            fsm_driver(&fsm21);
    }

    //恢复文件描述符原来的状态
    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);

    close(epfd);
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