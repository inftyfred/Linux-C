#include <stdio.h>
#include <stdlib.h>

int main()
{
#pragma omp parallel //这句代表下面的语句会并发实现
{
    puts("hello");
    puts("world");
}
//8核打印8次
    exit(0);
}