#ifndef PROTO_H__
#define PROTO_H__

#include <site_type.h>

#define DEFAULT_MGROUP      "224.2.2.2" //组播地址（D类地址）
#define DEFAULT_RCVPROT     "1989"

#define CHNNR               100
#define LISTCHNID           0
#define MINCHNID            1
#define MAXCHNID            (MINCHNID+CHNNR-1)

#define MSG_CHANNEL_MAX     (65536-20-8)    //减去IP包头和UDP包头的长度
#define MAX_DATA            (MSG_CHANNEL_MAX-sizeof(chnid_t))   //最大数据长度

#define MSG_LIST_MAX        (65536-20-8)
#define MAX_ENTRY           (MSG_LIST_MAX-sizeof(chnid_t))

struct msg_channel_st
{
    chnid_t chnid;          /*must be MINCHNID MAXCHNID*/
    uint8_t data[1];//变长
}__attribute__((packed));

struct msg_listentry_st
{
    chnid_t chnid;
    uint16_t len;
    uint8_t desc[1];
}__attribute__((packed));

struct msg_list_st
{
    chnid_t chnid;          /*must be LISTCHNID*/
    struct msg_listentry_st entry[1];
}__attribute__((packed));

#endif
