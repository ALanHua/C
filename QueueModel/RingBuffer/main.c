//
//  main.c
//  RingBuffer
//
//  Created by yhp on 2017/11/28.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//
/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define PACK(a, b, c, d) (uint32_t)(((a)<<24) | ((b)<<16) | ((c)<<8) | (d))
#define RINGBUFFER_SIGNATURE    PACK('r','b','u','f')
#define NULL_SIGNATURE 0x00000000

#define CONFIG_NUMOF_RINGBUFFER_HANDLES (3)
/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
typedef struct {
    uint32_t    signature;
    uint8_t*    buffer_start;
    uint8_t*    buffer_end;
    uint8_t*    rp;
    uint8_t*    wp;
    size_t      capacity;
    size_t      used;
}base_t;

//  自定义数据类型
typedef void*   handle_t;
typedef int32_t error_t;
/* ========================================================================== */
/*                          变量声明区                                        */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/**
 获取全局对象
 @return base_t*对象
 */
static base_t* get_object(void)
{
    static base_t  objs[CONFIG_NUMOF_RINGBUFFER_HANDLES];
    static int32_t  has_inited = 0;
    int32_t i;
    if (!has_inited) {
        memset(objs, 0, sizeof(objs));
        has_inited = 1;
    }
    
    for (i = 0; i < CONFIG_NUMOF_RINGBUFFER_HANDLES; i++) {
        if (objs[i].signature == NULL_SIGNATURE) {
            return &objs[i];
        }
    }
    return NULL;
}

/**
 创建环形缓存
 @param memorry 内存地址
 @param size 大小
 @return 句柄
 */
handle_t ringbuf_create(void* memorry,size_t size)
{
    base_t*   base;
    if (!memorry) {
        return NULL;
    }
    if (!size) {
       return NULL;
    }
    
    base = get_object();
    if (!base) {
        return NULL;
    }
    base->signature      = RINGBUFFER_SIGNATURE;
    base->buffer_start   = memorry;
    base->buffer_end     = base->buffer_start + size;
    base->rp             = base->buffer_start;
    base->wp             = base->buffer_start;
    base->capacity       = size;
    base->used           = 0;
    return (handle_t)base;
}

/**
 是否资源
 @param hdl 句柄
 @return 成功失败
 */
error_t ringbuf_destory(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return -1;
    }
    
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return -1;
    }
    
    base->signature = NULL_SIGNATURE;
    
    return 0;
}

static size_t rb_write(base_t* base,const void* data,size_t size)
{
    size_t taillen    = base->buffer_end - base->wp;
    const uint8_t* p = (const uint8_t*)data;
    
    if (taillen > size) {
        memcpy(base->wp, p, size);
        base->wp += size;
    }else{
        size_t headlen = size - taillen;
        memcpy(base->wp, p, taillen);
        memcpy(base->buffer_start, &p[headlen], headlen);
        base->wp = base->buffer_start + headlen;
    }
    
    if (size > (base->capacity - base->used)) {
        base->rp = base->wp;
        base->used = base->capacity;
    }else{
        base->used += size;
    }
    return size;
}

/**
 向缓存里写数据
 @param hdl 句柄
 @param data 数据
 @param size 数据长度
 @return 写入字节数
 */
size_t ringbuf_write(handle_t hdl,const void* data,size_t size)
{
    base_t* base = (base_t*)hdl;
    
    if (!base) {
        return 0;
    }
    if (!data) {
        return 0;
    }
    if (!size) {
        return 0;
    }
    if (size > base->capacity) {
        return 0;
    }
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    
    return rb_write(base,data,size);
}

static size_t rb_read(base_t* base,void* data,size_t size)
{
    uint8_t* p = (uint8_t*)data;
    size_t   taillen;
    size_t   asize;
    
    taillen = base->buffer_end - base->rp;
    asize   = (base->used > size) ? size : base->used;
    
    if (base->rp < base->wp) {
        memcpy(p,base->rp, asize);
    }else{
        size_t headlen = asize - taillen;
        memcpy(p, base->rp, taillen);
        memcpy(&p[headlen], base->buffer_start, headlen);
        base->rp = base->buffer_start + headlen;
    }
    base->used -= asize;
    
    return asize;
}

/**
 从缓存中读取数据
 @param hdl  句柄
 @param data 数据地址
 @param size 长度
 @return 读取的数据长度
 */
size_t ringbuf_read(handle_t hdl,void* data,size_t size)
{
    base_t* base = (base_t*)hdl;
    
    if (!base) {
        return 0;
    }
    if (!data) {
        return 0;
    }
    if (!size) {
        return 0;
    }
    if (size > base->capacity) {
        return 0;
    }
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    
    return rb_read(base,data,size);
}

//size_t ringbuf_read_to(handle_t hdl,void* data,size_t size,uint8_t token)
//{
//    
//}


int main(int argc, const char * argv[]) {
    
    
    return 0;
}
