#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <proto.h>

#include "thr_list.h"
#include "server_conf.h"

static struct mlib_listentry_st *list_entry; // 频道列表
static int num_list_entry;
static pthread_t tid_list; // 线程列表

static void *thr_list(void *p)
{
    int totalsize, i;
    struct msg_list_st *entrylistptr;
    struct msg_listentry_st *entryptr;
    int ret;
    int size;

    totalsize = sizeof(chnid_t);
    for(i = 0; i < num_list_entry; ++i)
    {
        totalsize += sizeof(struct msg_listentry_st) + strlen(list_entry[i].desc);
    }
    entrylistptr = malloc(totalsize);
    if(entrylistptr == NULL)
    {
        exit(1);
    }
    entrylistptr->chnid = LISTCHNID;

    entryptr = entrylistptr->entry;
    for(i = 0; i < num_list_entry; ++i)
    {
        size = sizeof(struct msg_listentry_st) + strlen(list_entry[i].desc);
        entryptr->chnid = list_entry[i].chnid;
        entryptr->len = htons(size);
        strcpy(entryptr->desc, list_entry[i].desc);
        entryptr = (void *)(((char *)entryptr) + size);//向后移动entptr
    }

    while(1)
    {
        ret = sendto(serversd, entrylistptr, totalsize, 0, (void *)&sndaddr, sizeof(sndaddr));
        if(ret < 0)
        {
            /*error*/
            syslog(LOG_WARNING, "sendto() %s", strerror(errno));
        }
        else
        {
            /*success*/
            syslog(LOG_DEBUG, "sento():success.");
        }

        sleep(1);
    }
}

//创建节目单线程
int thr_list_create(struct mlib_listentry_st *listptr, int num_ent)
{
    int err;
    list_entry = listptr;
    num_list_entry = num_ent;
    err = pthread_create(&tid_list, NULL, thr_list, NULL);
    if(err)
    {
        return -1;
    }

    return 0;
}

int thr_list_destroy(void)
{
    pthread_cancel(tid_list);
    pthread_join(tid_list, NULL);

    return 0;
}