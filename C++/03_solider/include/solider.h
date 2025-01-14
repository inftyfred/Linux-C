#pragma once
#include <string>
#include <Gun.h>

class solider
{
public:
    solider(std::string name);
    ~solider();
    void addGun(Gun *ptr_gun);
    void reload(int num);
    bool fire();

private:
    std::string _name;
    Gun *_ptr_gun;
};