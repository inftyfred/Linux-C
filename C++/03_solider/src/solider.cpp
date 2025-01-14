#include "solider.h"


solider::solider(std::string name)
{
    this->_name = name;
    this->_ptr_gun = nullptr;
}

void solider::addGun(Gun *ptr_gun)
{
    this->_ptr_gun = ptr_gun;
}

void solider::reload(int num)
{
    this->_ptr_gun->reload(num);
}

bool solider::fire()
{
    return this->_ptr_gun->shoot();
}

solider::~solider()
{
    if(this->_ptr_gun == nullptr)
    {
        return;
    }
    else
    {
        delete this->_ptr_gun;
        this->_ptr_gun = nullptr;
    }
}

