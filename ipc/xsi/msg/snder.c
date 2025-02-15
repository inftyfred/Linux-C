#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "proto.h"

int main()
{
    key_t key;
    int msgid;
    struct msg_st sbuf;

    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0)
    {
        perror("ftok()");
        exit(1);
    }

    msgid = msgget(key, 0);//已经被其他进程创建，不需要再次创建
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    sbuf.mtype = 3;
    strcpy(sbuf.name, "fred");
    sbuf.math = rand() % 100;
    sbuf.chinese = rand() % 100;
    if(msgsnd(msgid, &sbuf, sizeof(sbuf), 0) < 0) //- sizeof(long)
    {
        perror("msgsnd()");
        exit(1);
    }

    puts("ok!");
    //msgctl();

    exit(0);
}