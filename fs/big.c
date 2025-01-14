#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



int main(int argc, char **argv)
{
    int fd;

    if(argc < 2)
    {
        perror("Usage...\n");
        exit(1);
    }

    fd = open(argv[1],O_RDWR|O_CREAT|O_TRUNC,0600);
    if(fd<0)
    {
        perror("open()");
        exit(1);
    }

    lseek(fd,1LL*1024LL*1024LL*1024LL-1LL,SEEK_SET);//数字后要加上LL，防止结果溢出  5G文件
    write(fd,"",1);//将最后一个字符使用系统调用，否则文件不占空间
    close(fd);
 
    exit(0);
}
