//
//  lw_oopc.h
//  LW_OOPC
//
//  Created by yhp on 2016/12/11.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#ifndef lw_oopc_h
#define lw_oopc_h

#include <stdio.h>
#include <stdlib.h>

/**
 类的宏定义
 */
#define CLASS(type)                                         \
typedef struct type type;                                   \
struct type

/**
 构造器
 */
#define CTOR(type)                                          \
void* type##New()                                           \
{                                                           \
    struct type* t;                                         \
    t = (struct type*)malloc(sizeof(struct type));          
#define CTOR2(type,type2)                                   \
void* type2##New()                                          \
{                                                           \
struct type* t;                                             \
t = (struct type*)malloc(sizeof(struct type));
#define FUNCTION_SETTING(f1,f2)  t->f1 = f2;
#define END_CTOR  return (void*)t;}

/**
 接口
 */
#define INTERFACE(type)                                     \
typedef struct type type;                                   \
struct type

/**
 实现
 */
#define IMPLEMENTS(type)                                    \
struct type type

#endif /* lw_oopc_h */
