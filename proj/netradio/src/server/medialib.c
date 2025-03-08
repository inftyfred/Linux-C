#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


#include "medialib.h"
#include "server_conf.h"
#include "mytbf.h"

#define PATHSIZE        1024
#define LINEBUFFSIZE    1024
#define MP3_BITRATE     (320 * 1024)    //采样率

struct channel_context_st
{
    chnid_t chnid;
    char *desc;
    glob_t mp3glob; //解析目录
    int pos;
    int fd;
    off_t offset;
    mytbf_t *tbf;   //流量控制
};

static struct channel_context_st channel[MAXCHNID+1];//0号不用

//将某个目录下的文件转为一个频道
static struct channel_context_st *path2entry(const char *path)
{
    syslog(LOG_INFO, "cur path: %s", path);
    char pathstr[PATHSIZE] = {'\0'};
    char linebuf[LINEBUFFSIZE];
    FILE *fp;
    struct channel_context_st *me;

    static chnid_t curr_id = MINCHNID;

    //检测目录合法性
    strcat(pathstr, path);
    strcat(pathstr, "/desc.txt");
    fp = fopen(pathstr, "r");
    if(fp == NULL)
    {
        syslog(LOG_INFO, "%s can not find desc.txt", path);
        return NULL;
    }
    syslog(LOG_DEBUG, "channel dir:%s", pathstr);
    if(fgets(linebuf, LINEBUFFSIZE, fp) == NULL)
    {
        syslog(LOG_INFO, "%s/desc.txt is null", path);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    me = malloc(sizeof(*me));
    if(me == NULL)
    {
        return NULL;
    }

    //初始化流控
    me->tbf = mytbf_init(MP3_BITRATE / 8, MP3_BITRATE / 8 * 5);
    if(me->tbf == NULL)
    {
        free(me);
        return NULL;
    }

    //初始化频道
    me->desc = strdup(linebuf);
    strncpy(pathstr, path, PATHSIZE);
    strncat(pathstr, "/*.mp3", PATHSIZE-1);
    if(glob(pathstr, 0, NULL, &me->mp3glob) != 0)
    {
        curr_id++;
        syslog(LOG_ERR, "%s is not a channel dir(can not find mp3 files)", path);
        free(me);
        return NULL;
    }
    me->pos = 0;
    me->offset = 0;
    //打开第一个音乐文件
    me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY);
    if(me->fd < 0)
    {
        free(me);
        return NULL;
    }
    me->chnid = curr_id;
    curr_id++;

    return me;
}

int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum)
{
    int i, num = 0;
    char path[PATHSIZE];
    glob_t globres;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;

    for(i = 0; i < MAXCHNID+1; ++i)
    {
        channel[i].chnid = -1;//表示无内容
    }

    //解析目录
    snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);

    if(glob(path, 0, NULL, &globres))
    {
        return -1;
    }

    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if(ptr == NULL)
    {   //globres.gl_pathv[i] -> "/var/media/ch1"
        syslog(LOG_ERR, "malloc() error.");
        exit(1);
    }

    for(i = 0; i < globres.gl_pathc; ++i)
    {
        res = path2entry(globres.gl_pathv[i]);//获取一个频道内容
        if(res != NULL)
        {
            syslog(LOG_ERR, "path2entry() return: %d %s", res->chnid, res->desc);
            memcpy(channel + res->chnid, res, sizeof(*res));
            ptr[num].chnid = res->chnid;
            ptr[num].desc = strdup(res->desc);
            num++;
        }
    }
    *result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
    if(*result == NULL)
    {
        syslog(LOG_ERR, "realloc() failed");
    }
    *resnum = num;

    return 0;
}


int mlib_freechnlist(struct mlib_listentry_st *ptr)
{
    free(ptr);
    return 0;
}

static int open_next(chnid_t chnid)
{
    for(int i = 0; i < channel[chnid].mp3glob.gl_pathc; ++i)
    {
        channel[chnid].pos++;
        if(channel[chnid].pos == channel[chnid].mp3glob.gl_pathc)
        {
            //没有新文件了, 列表循环
            channel[chnid].pos = 0;
            break;
        }
        close(channel[chnid].fd);

        //尝试打开新文件
        channel[chnid].fd = \
            open(channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], O_RDONLY);
        if(channel[chnid].fd < 0)
        {
            syslog(LOG_WARNING, "open %s failed", channel[chnid].mp3glob.gl_pathv[chnid]);
        }
        else
        {
            //打开了新文件
            channel[chnid].offset = 0;
            return 0;
        }
    }
    syslog(LOG_ERR, "None of mp3 in channel %d id acailavle.", chnid);
    return -1;
}


ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size)
{
    int tbfsize;
    int len;
    int next_ret = 0;

    //获取token
    tbfsize = mytbf_fetchtoken(channel[chnid].tbf, size);

    while(1)
    {
        //读取tbfsize数据，从offset处开始的buf
        len = pread(channel[chnid].fd, buf, tbfsize, channel[chnid].offset);
        if(len < 0)
        {
            syslog(LOG_WARNING, "media file %s pread() %s.", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], strerror(errno));
            //读取下一首
            open_next(chnid);
        }
        else if(len == 0)
        {
            syslog(LOG_DEBUG, "media %s file is over", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos]);
            next_ret = open_next(chnid);
            break;
        }
        else    //读取到了数据 len > 0
        {
            channel[chnid].offset += len;
            struct stat sbuf;
            fstat(channel[chnid].fd, &sbuf);
            syslog(LOG_DEBUG, "epoch : %f%%", \
                (channel[chnid].offset) / (1.0*sbuf.st_size)*100);
            break;
        }

        if(tbfsize - len > 0)
        {
            mytbf_returntoken(channel[chnid].tbf, tbfsize - len);
        }
        //printf("cur chnid:%d \n", chnid);

        return len;//返回读取到的长度
    }
    
}
