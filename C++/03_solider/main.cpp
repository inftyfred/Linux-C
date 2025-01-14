#include "Gun.h"
#include "solider.h"

void test()
{
    solider s("Xu Sanduo");
    s.addGun(new Gun("AK47"));
    s.reload(30);

    s.fire();
}


int main()
{
    printf("start...\n");
    test();
    printf("done end\n");
    return 0;
}