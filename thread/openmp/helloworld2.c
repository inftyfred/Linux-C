#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
#pragma omp parallel sections//这句代表下面的语句会并发实现
{
#pragma omp section
    printf("[%d]hello\n", omp_get_thread_num());//num代表打印该语句的核的代号
#pragma omp section
    printf("[%d]world\n", omp_get_thread_num());
}

    exit(0);
}