#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

/*
-y: 打印年
-m: 打印月
-d: 打印当前日期
-H：hour
-M: minute
-S: sec
*/
#define TIMESTRSIZE 1024
#define FMTSTRSIZE 1024

int main(int argc ,char  **argv)
{

    time_t mytime;
    struct tm *my_tm;
    char timestr[TIMESTRSIZE];
    int c;
    char fmtstr[FMTSTRSIZE];
    fmtstr[0] = '\0';
    FILE *fp = stdout;

    //获取时间戳
    mytime = time(NULL);
    my_tm = localtime(&mytime);

    //命令行参数分析
    while(1)
    {
        c = getopt(argc,argv,"-H:MSy:md");
        if(c<0)
            break;
        switch (c)
        {
            case 1://处理非选项的传参
                fp = fopen(argv[optind-1],"w");//optind指向argv位置
                if(fp == NULL)
                {
                    perror("fopen()");
                    fp = stdout;//打印到标准输出
                }
                break;
            case 'H'://带参数的选项 optarg指向该参数后选项
                if(strcmp(optarg,"12") == 0)
                    strncat(fmtstr,"%I(%P) ",FMTSTRSIZE);
                else if(strcmp(optarg,"24") == 0)
                    strncat(fmtstr,"%H ",FMTSTRSIZE);
                else
                    fprintf(stderr,"Invalid argument");
                break;
            case 'M':
                strncat(fmtstr,"%M ",FMTSTRSIZE);
                break;
            case 'S':
                strncat(fmtstr,"%S ",FMTSTRSIZE);
                break;
            case 'y'://带参数的选项
                if(strcmp(optarg,"2") == 0)
                    strncat(fmtstr,"%y ",FMTSTRSIZE);
                else if (strcmp(optarg,"4") == 0)
                {
                    strncat(fmtstr,"%Y ",FMTSTRSIZE);
                }
                else
                    fprintf(stderr,"Invalid argument:-y2 or 4");
                break;
            case 'm':
                strncat(fmtstr,"%m ",FMTSTRSIZE);
                break;
            case 'd':
                strncat(fmtstr,"%d ",FMTSTRSIZE);
                break;
            default:
                break;
        }
    }

    strncat(fmtstr,"\n",FMTSTRSIZE);
    strftime(timestr,TIMESTRSIZE,fmtstr,my_tm);
    fputs(timestr,fp);

    //如果不是stdout则释放资源
    if(fp != stdout)
        fclose(fp);

	exit(0);
}
