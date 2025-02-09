#ifndef MYSEM_H__
#define MYSEM_H__

typedef void mysem_t;

/*
使用互斥量+条件变量实现信号量
*/
mysem_t *mysem_init(int initval);//使用资源量上限初始化资源

int mysem_add(mysem_t *, int); //还资源量

int mysem_sub(mysem_t *, int); //给资源量

int mysem_destroy(mysem_t *); //释放资源

#endif