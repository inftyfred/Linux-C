#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>

#define PATHSIZE 1024

//是否构成循环
static int path_noloop(const char *path)
{
    char *pos;
    pos = strrchr(path,'/');//找到最右边的字符位置
    if(pos == NULL)
    {
        exit(1);
    }
    if(strcmp(pos+1,".") == 0 || strcmp(pos+1,"..") == 0)
    {
        return 0;//说明是循环文件
    }
    return 1;//说明正常
}


static int64_t mydu(const char *path)
{
    static struct stat statres;     //static优化 没有跨越递归可以优化
    static char nextpath[PATHSIZE];
    glob_t globres;
    int ret,i;
    int64_t sum=0;


    ret = lstat(path,&statres);
    if(ret < 0)
    {
        perror("lstat()");
        exit(1);
    }
    if(!S_ISDIR(statres.st_mode))
    {
        //非目录文件
        return statres.st_blocks;
    }

    //目录文件 递归 glob解析: /* /.*
    //再次调用mydu递归
    strncpy(nextpath,path,PATHSIZE);
    strncat(nextpath,"/*",PATHSIZE);//非隐藏文件
    if (glob(nextpath,0,NULL,&globres) < 0) {
        fprintf(stderr,"glob()");
        exit(1);
    }

    strncpy(nextpath,path,PATHSIZE);
    strncat(nextpath,"/.*",PATHSIZE);//隐藏文件
    if (glob(nextpath,GLOB_APPEND,NULL,&globres) < 0) {
        fprintf(stderr,"glob()");
        exit(1);
    }

    sum = statres.st_blocks;//加上目录文件的大小
    for(i=0; i<globres.gl_pathc; i++)
    {
        if(path_noloop(globres.gl_pathv[i]))
            sum += mydu(globres.gl_pathv[i]);
    }

    globfree(&globres);

    return sum;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage ...\n");
        exit(1);
    }

    printf("%lld\n",mydu(argv[1])/2);//转换为KB

    exit(0);
}