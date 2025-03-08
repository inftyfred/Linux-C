#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <syslog.h>

#include <proto.h>

#include "medialib.h"
#include "server_conf.h"


/*
 *  -M      指定多播组
 *  -P      指定接收端口
 *  -F      前台运行
 *  -D      指定媒体库位置
 *  -I      指定网络设备
 *  -H      显示帮助
 */

 struct server_conf_st server_conf = {
    .rcvport = DEFAULT_RCVPROT, \
    .mgroup = DEFAULT_MGROUP, \
    .media_dir = DEFAULT_MEDIADIR,\
    .runmode = RUN_DAEMON,  \
    .ifname = DEFAULT_IF
 };

 static void printhelp(void)
{
    printf("server help : \n");
    printf("-M        指定多播组\n");
    printf("-P        指定接收端口\n");
    printf("-F        前台运行\n");
    printf("-D        指定媒体库的位置\n");
    printf("-I        指定网络设备\n");
    printf("-H        显示帮助\n");
}

static void daemon_exit(int s)
{
    closelog();

    exit(0);
}

static int daemonize(void)
{
    pid_t pid;
    int fd;

    pid = fork();
    if(pid < 0)
    {
        //perror("fork()");
        syslog(LOG_ERR, "fork() :%s", strerror(errno)); //写系统日志
        return -1;
    }
    if(pid > 0)
    {
        exit(0);
    }

    fd = open("/dev/null", O_RDWR);
    if(fd < 0)
    {
        //perror("open()");
        syslog(LOG_WARNING, "open() :%s", strerror(errno));
        return -2;
    }

    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    if(fd > 2)
        close(fd);

    setsid();

    chdir("/"); //保证一个一定有的环境
    umask(0);   //关闭umask 保证默认权限

    return 0;
}

static void socket_init(void)
{
    int serversd;
    struct ip_mreqn mreq;

    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if(serversd < 0)
    {
        syslog(LOG_ERR, "socket()");
        exit(1);
    }

    inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex(server_conf.ifname);
    if(setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
    {
        syslog(LOG_ERR, "setsocket()");
        exit(1);
    }

}


int main(int argc, char **argv)
{
    int opt;
    struct sigaction sa, osa;

    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    openlog("netradio", LOG_PID | LOG_PERROR, LOG_DAEMON);
    /*命令行分析*/
    while(1)
    {
        opt = getopt(argc, argv, "M:P:FD:I:H");
        if(opt == -1)
            break;
        switch (opt)
        {
        case 'M':
            server_conf.mgroup = optarg;
            break;
        case 'P':
            server_conf.rcvport = optarg;
            break;
        case 'F':
            server_conf.runmode = RUN_FOREGROUND;
            break;
        case 'D':
            server_conf.media_dir = optarg;
            break;
        case 'I':
            server_conf.ifname = optarg;
            break;
        case 'H':
            printhelp();
            exit(0);
            break;
        default:
            abort();
            break;
        }
    }

    /*守护进程的实现*/
    if(server_conf.runmode == RUN_DAEMON)//守护进程模式
    {
        if(daemonize() != 0)
        {
            exit(1);
        }
    }
    else if(server_conf.runmode == RUN_FOREGROUND)
    {
        /*do nothing*/
    }
    else
    {
        /*inval value*/
        syslog(LOG_ERR, "EINVAL: server_conf.runmode.");
        exit(1);
    }

    /*socket初始化, 主动端 可以省略bind*/
    socket_init();
    
    /*获取频道信息*/
    struct mlib_listentry_st *mlib_list;
    int mlib_list_size;
    int err;

    err = mlib_getchnlist(&mlib_list, &mlib_list_size);

    /*创建节目单线程*/
    int thr_list_create(mlib_list, mlib_list_size);
    /*if error*/

    /*创建频道线程*/
    for(int i = 0; i < mlib_list_size; ++i)
    {
        thr_channel_create(mlib_list+i);
        /*if error*/
    }

    syslog(LOG_DEBUG, "%d channel threads created.", i);

    while(1)
    {
        pause();
    }
        
    exit(0);
}