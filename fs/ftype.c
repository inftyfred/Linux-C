#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int ftype(const char *fname) //使用int类型作为返回值，但显示仍然使用字符类型
{
    struct stat statres;
    if(stat(fname, &statres) < 0)
    {
        perror("stat()");
        exit(1);
    }
    if( S_ISREG(statres.st_mode) )
        return '-';
    else if( S_ISDIR(statres.st_mode))
        return 'd';
    else if(S_ISSOCK(statres.st_mode))
        return 's';
    else
        return '?';
}


int main(int argc, char **argv)
{
    int fd;

    if(argc < 2)
    {
        perror("Usage...\n");
        exit(1);
    }

    printf("%c dsdsd \n", ftype(argv[1]));

 
    exit(0);
}
