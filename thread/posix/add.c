#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define THRNUM 20
#define FNAME "/tmp/out"
#define LINESIZE 1024

//静态初始化
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void *thr_add(void *p)
{
    FILE *fp;
    char linebuf[LINESIZE];
    fp = fopen(FNAME, "r+");
    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    pthread_mutex_lock(&mut);//加锁
    fgets(linebuf, LINESIZE, fp);
    fseek(fp, 0, SEEK_SET);//定位到文件首
    fprintf(fp, "%d\n", atoi(linebuf)+1);
    //sleep(1);
    fclose(fp);
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);
}

int main()
{
    int i, err;
    pthread_t tid;

    for(i = 0; i < THRNUM; i++)
    {
        err = pthread_create(&tid, NULL, thr_add, NULL);
        if(err)
        {
            fprintf(stderr,"create:%s\n", strerror(err));
            exit(1);
        }
    }

    for(i = 0; i < THRNUM; i++)
    {
        pthread_join(tid, NULL);
    }
    pthread_mutex_destroy(&mut);
    exit(0);
}