#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <error.h>


#define PAT "/etc"


int main()
{
    DIR *dp;
    struct dirent *cur;

    dp = opendir(PAT);
    if(NULL == dp)
    {
        perror("opendir error:");
        exit(1);
    }

    while ((cur = readdir(dp)) != NULL)
    {
        puts(cur->d_name);
    }
    
    closedir(dp);

    exit(0);
}