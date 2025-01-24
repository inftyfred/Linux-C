#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    alarm(5);
    alarm(10);
    alarm(1);//程序会在1秒后结束，alarm无法实现多个时钟

    while(1)
        pause();//等待信号到来
    exit(0);
}