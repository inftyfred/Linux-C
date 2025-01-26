#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/*
实现一个守护进程
*/
#define FNAME "/tmp/out"

static FILE *fp;

static int daemonize()
{
    pid_t pid;
    int fd;
    pid = fork();
    if(pid < 0)
    {
        return -1;
    }
    if(pid > 0) //parent
    {
        exit(0);
    }
    //child:
    fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
        return -1;
    }
    //重定向 守护进程的标准输入输出
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    if(fd > 2)
    {
        close(fd);
    }
    setsid(); 
    chdir("/");//改变工作目录，根目录相对安全，不会受到其他影响
    umask(0);//设置文件权限掩码，0777，保证具有完整的权限
    return 0; //成功
}

static void daemon_exit(int s)
{
    if(s == SIGINT)
    {
        fprintf(stdout,"SIGINT\n");
    }
    else if(s == SIGTERM)
    {
        fprintf(stdout,"SIGTERM\n");
    }
    fclose(fp);
    closelog();//关闭log
    exit(0);
}

int main()
{
    int i = 0;
    struct sigaction siga;
    
    siga.sa_handler = daemon_exit;
    sigemptyset(&siga.sa_mask);
    sigaddset(&siga.sa_mask,SIGQUIT);
    sigaddset(&siga.sa_mask,SIGTERM);//在响应sigint时阻塞这些信号
    sigaddset(&siga.sa_mask,SIGINT);
    siga.sa_flags = 0;//无特殊要求

    sigaction(SIGINT,&siga,NULL);//注册SIGINT信号
    sigaction(SIGQUIT,&siga,NULL);//注册SIGQUIT信号
    sigaction(SIGTERM,&siga,NULL);//注册SIGTERM信号

    openlog("mydaemon",LOG_PID,LOG_DAEMON);//打开log

    if(daemonize())
    {
        syslog(LOG_ERR,"daemonize() failed!");
        exit(1);
    }
    else
    {
        syslog(LOG_INFO,"daemonize() successed!");
    }

    fp = fopen(FNAME,"w");
    if(fp == NULL)
    {
        syslog(LOG_ERR,"fopen:%s",strerror(errno));
        exit(1);
    }

    syslog(LOG_INFO,"%s was opened.",FNAME);

    while(1)
    {
        fprintf(fp,"%d\n",i);
        fflush(fp);
        syslog(LOG_DEBUG,"%d is printed.",i);
        sleep(1);
        i++;
    }

    exit(0);
}