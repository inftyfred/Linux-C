#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
打印时间到制定文件中，1s打印一行
*/

#define FILENAME "/tmp/out"
#define BUFFSIZE 1024

int main(int argc ,char  **argv)
{

    time_t mytime;
    struct tm *my_tm;
    FILE *fp;
    int count = 0;
    char buf[BUFFSIZE];

    fp = fopen(FILENAME,"a+");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }

    while (fgets(buf,BUFFSIZE,fp) != NULL)
        count++;//记录行数
        
    while (1)
    {
        time(&mytime);
        my_tm = localtime(&mytime);
        fprintf(fp,"%-4d%d-%d-%d %d:%d:%d\n",++count,\
        my_tm->tm_year+1900,my_tm->tm_mon+1,my_tm->tm_mday,\
        my_tm->tm_hour,my_tm->tm_min,my_tm->tm_sec);
        //除了标准输出，所有的都是全缓冲模式 需要使用fflush刷新缓冲区
        fflush(fp);

        sleep(1);
    }
    
	fclose(fp);
	exit(0);
}
