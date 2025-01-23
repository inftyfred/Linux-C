#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
打印100天以后的时间
*/
#define TIMESTRSIZE 1024

int main(int argc ,char  **argv)
{

    time_t mytime;
    struct tm *my_tm;
    char timestr[TIMESTRSIZE];

    mytime = time(NULL);
    my_tm = localtime(&mytime);
    strftime(timestr,TIMESTRSIZE,"Now:%Y-%m-%d",my_tm);
    puts(timestr);

    //100*60*60*24 100天的秒数
    my_tm->tm_mday += 100;
    (void)mktime(my_tm);//mktime会自动修改时间格式
    strftime(timestr,TIMESTRSIZE,"100days after:%Y-%m-%d",my_tm);
    puts(timestr);

	exit(0);
}
