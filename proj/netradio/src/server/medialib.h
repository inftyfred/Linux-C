#ifndef MEDIALIB_H__
#define MEDIALIB_H__

#include <proto.h>
#include <stdio.h>

#define MP3_BITRATE     (320 * 1024)    //采样率

struct mlib_listentry_st
{
    chnid_t chnid;
    char *desc; //本地使用
};

int mlib_getchnlist(struct mlib_listentry_st **, int *);

int mlib_freechnlist(struct mlib_listentry_st *);

ssize_t mlib_readchn(chnid_t, void *, size_t );

#endif