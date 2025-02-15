#ifndef PROTO_H__
#define PROTO_H__
/*
*约定通信协议
*/
//用于产生同一个key值
#define KEYPATH "/etc/services"
#define KEYPROJ 'g'

#define NAMESIZE 32

struct msg_st
{
    long mtype;//数据类型 必须大于0
    char name[NAMESIZE];
    int math;
    int chinese;
};




#endif