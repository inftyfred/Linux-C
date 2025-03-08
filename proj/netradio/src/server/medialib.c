#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <fcntl.h>

#include "medialib.h"
#include "mytbf.h"


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

int mlib_getchnlist(struct mlib_listentry_st **, int *)
{

}
