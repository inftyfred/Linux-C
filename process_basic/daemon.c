#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

/*
实现一个守护进程
*/
#define FNAME "/tmp/out"

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

int main()
{
    FILE *fp;
    int i = 0;
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
    fclose(fp);
    closelog();//关闭log
    exit(0);
}