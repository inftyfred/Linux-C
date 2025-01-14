#include <iostream>
#include "swap.h"

int main(int argc, char **argv)
{
    int a = 10, b = 30;

    swap mySwap(a, b);

    printf("before :\n a=%d\n b=%d\n", a, b);

    mySwap.run();

    printf("after :\n");
    mySwap.printInfo();

}