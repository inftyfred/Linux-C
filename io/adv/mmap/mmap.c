#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*
*   通过命令行传入一个文件，统计该文件中字母a的个数
*
*/

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    int fd, i, count = 0;
    struct stat fstres;
    char *str;

    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        fprintf(stderr, "open()\n");
        exit(0);
    }

    if(fstat(fd, &fstres) < 0)
    {
        perror("fstat");
        exit(1);
    }

    str = mmap(NULL, fstres.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(str == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }
    close(fd); //已经完成映射，可以直接关闭文件

    for(i = 0; i < fstres.st_size; i++)
    {
        if(str[i] == 'a')
            count++;
    }

    printf("%s[file size: %ld]: a count: %d\n", argv[1], fstres.st_size, count);

    munmap(str, fstres.st_size);//解除空间映射

    exit(0);
}