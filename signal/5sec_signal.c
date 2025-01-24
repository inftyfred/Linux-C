#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static volatile int loop = 1;//防止编译器优化，每次读写都去存储空间读取

static void alarm_handler(int s)
{
    loop = 0;
}

int main()
{
    int64_t count = 0;
    alarm(5);
    signal(SIGALRM,alarm_handler);

    while(loop)
        count++;

    printf("count:%lld\n",count);

    exit(0);
}