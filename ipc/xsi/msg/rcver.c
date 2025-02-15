#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

int main()
{
    key_t key;
    int msgid;
    struct msg_st rbuf;

    key = ftok(KEYPATH, KEYPROJ);
    if(key < 0)
    {
        perror("ftok()");
        exit(1);
    }

    msgid = msgget(key, IPC_CREAT | 0600);
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    while(1)
    {   //-sizeof(long)
        if(msgrcv(msgid, &rbuf, sizeof(rbuf), 0, 0) < 0)
        {   //柔性数组./re  
            perror("msgrcv");
            exit(1);
        }
        printf("NAME = %s\n", rbuf.name);
        printf("MATH = %d\n", rbuf.math);
        printf("CHINESE = %d\n", rbuf.chinese);
    }

    msgctl(msgid, IPC_RMID, NULL);//删除队列
    /*if error*/

    exit(0);
}