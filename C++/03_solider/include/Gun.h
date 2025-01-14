#pragma once
#include <string>

class Gun
{
public:
    Gun(std::string type)
    {
        this->_type = type;
        this->_bullet = 0;
    }
    void reload(int bullet);
    bool shoot();

private:
    int _bullet;
    std::string _type;

};