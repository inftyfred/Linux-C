#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <proto.h>

#include "thr_channel.h"
#include "medialib.h"
#include "server_conf.h"


struct thr_channel_ent_st
{
    chnid_t chnid;
    pthread_t tid;
};

struct thr_channel_ent_st thr_channel[CHNNR];
static int tid_nextpos = 0;

static void *thr_channel_snder(void *ptr)
{
    struct msg_channel_st *sbufp;
    struct mlib_listentry_st *ent = ptr;
    int len;

    sbufp = malloc(MSG_CHANNEL_MAX);
    if(sbufp == NULL)
    {
        syslog(LOG_ERR, "malloc()%s\n", strerror(errno));
        exit(1);
    }

    sbufp->chnid = ent->chnid;

    while(1)
    {
        len = mlib_readchn(ent->chnid, sbufp->data, 320*1024/8); //MAX_DATA  MP3_BITRATE / 8 //
        if(len < 0)
        {
            break;
        }
        syslog(LOG_DEBUG, "mlib_readchn() len: %d.", len);
        //syslog(LOG_DEBUG, "mlib_readchn() len: %d", );
        if(sendto(serversd, sbufp, len + sizeof(chnid_t), 0, (void *)&sndaddr, sizeof(sndaddr)) < 0)
        {
            syslog(LOG_ERR, "thr_channel(%d) sendto():%s\n", ent->chnid, strerror(errno));
            break;
        }

        sched_yield();//可有可无
    }

    pthread_exit(NULL);
}

int thr_channel_create(struct mlib_listentry_st *ptr)
{
    int err;

    err = pthread_create(&thr_channel[tid_nextpos].tid, NULL, thr_channel_snder, ptr);
    if(err)
    {
        syslog(LOG_WARNING, "pthread_create() failed.");
        return -err;
    }

    thr_channel[tid_nextpos].chnid = ptr->chnid;
    tid_nextpos++;

    return 0;
}

int thr_channel_destroy(struct mlib_listentry_st *ptr)
{
    int i;

    for(i = 0; i < CHNNR; ++i)
    {
        if(thr_channel[i].chnid == ptr->chnid)
        {
            if(pthread_cancel(thr_channel[i].tid) < 0)
            {
                syslog(LOG_ERR, "pthread_cancel() failed");
                return -ESRCH;
            }
            pthread_join(thr_channel[i].tid, NULL);
            thr_channel[i].chnid = -1;

            return 0;
        }
    }

    return -1;
}

int thr_channel_destroyall(void)
{
    int i = 0;
    for(i = 0; i < CHNNR; ++i)
    {
        if(thr_channel[i].chnid > 0)
        {
            if(pthread_cancel(thr_channel[i].tid) < 0)
            {
                syslog(LOG_ERR, "pthread_cancel() faild");
                return -ESRCH;
            }
            pthread_join(thr_channel[i].tid, NULL);
            thr_channel[i].chnid = -1;
        }
    }

    return 0;
}