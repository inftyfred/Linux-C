#include <stdio.h>
#include <stdlib.h>


//钩子函数
static void f1(void)
{
    puts("f1() is working!");
}

static void  f2(void)
{
    puts("f2() is working");
}

static void f3(void)
{
    puts("f3() is working");
}


int main()
{


    printf("begin!\n");

    //将3个函数挂在钩子上
    atexit(f1);
    atexit(f2);
    atexit(f3);

    printf("end\n");

    exit(0);
}