#ifndef PROTO_H__
#define PROTO_H__

#define MTGROUP "224.2.2.2" //多播组 B类地址
#define RCVPORT "1989"  //atoi

#define NAMESIZE 11

struct msg_st
{
    uint8_t name[NAMESIZE];
    uint32_t math;
    uint32_t chinese;
}__attribute__((packed)); //gcc语法 保证编译器不进行对齐

#endif