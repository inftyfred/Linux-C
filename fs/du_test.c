#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>

#define PATHSIZE 1024

// 检查路径是否构成循环
int path_noloop(const char *path)
{
    char *pos;
    pos = strrchr(path, '/'); // 找到路径中最后一个 `/` 的位置
    if (pos == NULL)
    {
        return 1; // 没有 `/`，认为是当前目录
    }
    if (strcmp(pos + 1, ".") == 0 || strcmp(pos + 1, "..") == 0)
    {
        return 0; // 如果是 `.` 或 `..`，返回 0（表示循环）
    }
    return 1; // 否则返回 1（表示正常）
}

// 递归计算磁盘使用情况
static int64_t mydu(const char *path)
{
    struct stat statres;
    char nextpath[PATHSIZE];
    glob_t globres;
    int ret, i;
    int64_t sum;

    // 获取文件状态
    ret = stat(path, &statres); // 使用 stat 而不是 lstat
    if (ret < 0)
    {
        perror("stat()");
        exit(1);
    }

    // 如果是非目录文件，直接返回占用的块数
    if (!S_ISDIR(statres.st_mode))
    {
        return (statres.st_blocks * 512) / 1024; // 转换为 KB
    }

    // 如果是目录文件，递归处理
    snprintf(nextpath, PATHSIZE, "%s/*", path); // 匹配非隐藏文件
    ret = glob(nextpath, GLOB_ONLYDIR, NULL, &globres); // 只匹配目录
    if (ret != 0)
    {
        fprintf(stderr, "glob() failed for path: %s\n", nextpath);
        globfree(&globres);
        return (statres.st_blocks * 512) / 1024; // 返回当前目录的大小
    }

    snprintf(nextpath, PATHSIZE, "%s/.*", path); // 匹配隐藏文件
    ret = glob(nextpath, GLOB_ONLYDIR | GLOB_APPEND, NULL, &globres); // 只匹配目录并追加
    if (ret != 0)
    {
        fprintf(stderr, "glob() failed for path: %s\n", nextpath);
        globfree(&globres);
        return (statres.st_blocks * 512) / 1024; // 返回当前目录的大小
    }

    sum = 0;
    for (i = 0; i < globres.gl_pathc; i++)
    {
        if (path_noloop(globres.gl_pathv[i]))
            sum += mydu(globres.gl_pathv[i]); // 递归计算子目录或文件的大小
    }

    sum += (statres.st_blocks * 512) / 1024; // 加上当前目录的大小
    globfree(&globres); // 释放 glob 资源
    return sum; // 返回总大小（单位是 KB）
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(1);
    }

    printf("%lld KB\n", mydu(argv[1])); // 计算并输出指定路径的磁盘使用情况
    exit(0);
}