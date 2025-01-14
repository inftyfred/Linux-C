#include "Gun.h"
#include <iostream>

void Gun::reload(int bullet)
{
    this->_bullet = bullet;
}

bool Gun::shoot()
{
    if(this->_bullet > 0)
    {
        this->_bullet--;
        printf("shoot success\n");
        return true;
    }
    else
    {
        std::cout << "bullet empety" << std::endl;
        return false;
    }
}