#include<stdio.h>
#include<stdlib.h>

extern char **environ;

#define MY_ENV_NAME "MY_ENV"

int main()
{
    int ret;
    //puts(getenv("PATH"));

    ret = setenv(MY_ENV_NAME,"my_value",0);
    if(ret != 0)
    {
        fprintf(stderr,"setenv()\n");
        exit(1);
    }
    
    puts(getenv(MY_ENV_NAME));


    exit(0);
}
