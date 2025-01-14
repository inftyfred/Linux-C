#include <iostream>
#include "swap.h"

int main(int argc, char **argv)
{
    int a = 10, b = 20;

    swap mySwap(a, b);

    printf("before :a=%d,b=%d\n", a, b);

    mySwap.run();

    mySwap.printInfo();
}