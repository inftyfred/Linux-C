#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>

extern char **environ;

#define DELIMS " \t\n"

//后续可更改
struct cmd_st
{
    glob_t globres;//使用glob函数，类似argc、argv的存储结构
};


static void prompt(void)
{
    printf("myshell-0.1$ ");
}

static void parse(char *line, struct cmd_st *res)
{
    char *tok;
    int i = 0;
    while (1)
    {
        tok = strsep(&line,DELIMS);
        if(tok == NULL)
        {
            break;
        }
        if(tok[0] == '\0')//处理空字符串
        {
            continue;
        }
        //第一次不追加，因为此时globres中是未知的
        glob(tok,GLOB_NOCHECK|GLOB_APPEND*i,NULL,&res->globres);
        i = 1;
    }
}


int main()
{
    //getline参数初始化必须要完成的步骤
    char *linebuf = NULL;
    size_t linebuf_size = 0;
    pid_t pid;
    struct cmd_st cmd;

    while (1)
    {
        prompt();//打印提示符

        if((getline(&linebuf,&linebuf_size,stdin)) < 0)//获取用户输入
        {
            break;
        }

        parse(linebuf,&cmd); //解析命令
        if(0)//内部命令) //shell解释器内部实现
        {
            //do somthing
        }
        else //外部命令  few实现
        {
            pid = fork();
            if(pid < 0)
            {
                perror("fork()");
                exit(1);
            }
            if(pid == 0)
            {
                execvp(cmd.globres.gl_pathv[0], cmd.globres.gl_pathv);
                perror("execvp()");
                exit(1);
            }
            else //parent
            {
                wait(NULL); //父进程收尸子进程
            }
        }
    }

    exit(0);
}