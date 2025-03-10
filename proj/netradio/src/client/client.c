#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <proto.h>

#include "client.h"

#define BUFSIZE (320*1024/8*3)

/*
* -M    --mgroup    指定多播组
* -P    --port      指定接收端口
* -p    --player    指定播放器
* -H    --help      显示帮助
* */

struct client_conf_st client_conf = { \
    .rcvport = DEFAULT_RCVPROT, \
    .mgroup = DEFAULT_MGROUP, \
    .player_cmd = DEFAULT_PLAYERCMD
};

static void printhelp(void)
{
    printf("client help : \n");
    printf("-M    --mgroup    指定多播组\n");
    printf("-P    --port      指定接收端口\n");
    printf("-p    --player    指定播放器\n");
    printf("-H    --help      显示帮助\n");
}

//写入len个字节
static ssize_t writen(int fd, const char *buf, size_t len)
{
    int ret, pos = 0;
    while(1)
    {
        ret = write(fd, buf+pos, len);
        if(ret < 0)
        {
            if(errno == EINTR)
                continue;
            perror("write()");
            return -1;
        }
        len -= ret;
        pos += ret;
        if(len <= 0)
            break;
    }

    return pos;
}


int main(int argc, char **argv)
{
    /*
    *   初始化
    *   级别：默认值、配置文件、环境变量、命令行参数
    * */
    int opt, index = 0;
    int sd;
    int val = 1;
    int len;
    int ret;
    int pd[2];
    int chosenid;
    pid_t pid;
    struct ip_mreqn mreq;
    struct sockaddr_in laddr, serveraddr, raddr;
    socklen_t serveraddr_len, raddrlen;
    struct option long_options[] = {
        {"mgrop",    1,     0,  'M' },
        {"port",     1,     0,  'P' },
        {"player",   1,     0,  'p' },
        {"help",     0,     0,  'h' },
        {0,          0,     0,   0  }
    };

    //opt init
    while(1)
    {
        opt = getopt_long(argc, argv, "M:P:p:H", long_options, &index);//分析命令行
        if(opt == -1)
            break;
        switch (opt)
        {
        case 'P':
            client_conf.rcvport = optarg;
            break;
        case 'M':
            client_conf.mgroup = optarg;
            break;
        case 'p':
            client_conf.player_cmd = optarg;
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

    //socket init
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
    /*if error*/
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex("eth0");
    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    //可以省略 
    //控制是否可以回送数据
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    //bind
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.rcvport));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    //匿名管道 实现父子进程间通信
    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    //子进程：调用解码器
    if(pid == 0)
    {
        close(sd);
        close(pd[1]);//只读不写
        dup2(pd[0], 0);//重定向：将从管道中接收到的内容重定向到了子进程的标准输入
        if(pd[0] > 0)
            close(pd[0]);
        
        //直接使用shell解析
        execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL);
        perror("execl()");
        exit(1);
    }

    //父进程：从网络上收包，发送给子进程
    if(pid > 0)
    {
        close(pd[0]);//只写不读
        //收节目单
        struct msg_list_st *msg_list;

        msg_list = malloc(MSG_LIST_MAX);
        if(msg_list == NULL)
        {
            perror("malloc()");
            exit(1);
        }

        //接收节目单信息
        serveraddr_len = sizeof(serveraddr);
        while(1)
        {
            len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&serveraddr, &serveraddr_len);
            if(len < sizeof(struct msg_list_st))
            {
                fprintf(stderr, "msg is to small\n");
                continue;
            }
            if(msg_list->chnid != LISTCHNID)
            {
                fprintf(stderr, "chnnid is not match\n");
                continue;
            }
            else
            {
                break;
            }
        }
        //打印节目单并选择频道
        struct msg_listentry_st *pos;
        for(pos = msg_list->entry; (char *)pos < (((char *)msg_list) + len); pos = (void *)(((char *)pos) + ntohs(pos->len)))
        {
            printf("channel %d : %s\n", pos->chnid, pos->desc);
        }

        free(msg_list);

        while(ret < 1)
        {
            printf("请选择频道： ");
            ret = scanf("%d", &chosenid);
            if(ret != 1)
            {
                exit(1);
            }
        }

        //收频道包，发送给子进程
        struct msg_channel_st *msg_channel;
        msg_channel = malloc(MSG_CHANNEL_MAX);
        if(msg_channel == NULL)
        {
            perror("malloc()");
            exit(1);
        }

        raddrlen = sizeof(raddr);
        char rcvbuf[BUFSIZE];
        uint32_t offset = 0;
        memset(rcvbuf, 0, BUFSIZE);
        int bufct = 0;
        while(1)
        {
            len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &raddrlen);
            if(raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || raddr.sin_port != serveraddr.sin_port)
            {
                fprintf(stderr, "ignore: addr not match\n");
                continue;
            }
            if(len < sizeof(struct msg_channel_st))
            {
                fprintf(stderr, "ignore:msg too small\n");
                continue;
            }
            if(msg_channel->chnid == chosenid)
            {
                fprintf(stdout, "accept msg : %d received \n", msg_channel->chnid);
                if(writen(pd[1], msg_channel->data, len-sizeof(chnid_t)) < 0)
                {
                    exit(1);
                }
                // memcpy(rcvbuf + offset, msg_channel->data, len - sizeof(chnid_t));
                // offset += len - sizeof(chnid_t);

                // if (bufct++ % 2 == 0) {
                //     if (writen(pd[1], rcvbuf, offset) < 0) {
                //         exit(1);
                //     }
                //     offset = 0;
                // }
            }
        }

        free(msg_channel);
        close(sd);

        exit(0);
    }
}
