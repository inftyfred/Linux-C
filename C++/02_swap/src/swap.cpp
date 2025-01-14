#include "swap.h"


void swap::run()
{
    int temp;
    temp = this->_a;
    this->_a = this->_b;
    this->_b = temp;
}

void swap::printInfo()
{
    std::cout << "a = " << _a << std::endl;
    std::cout << "b = " << _b << std::endl;
}