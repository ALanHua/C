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

#define TEST_RING_BUFFER_SIZE    64
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

/**
 从指定内存处读取数据
 @param hdl 句柄
 @param data 数据地址
 @param size 读取速度长度
 @param token 查找字符
 @return 实际读取长度
 */
size_t ringbuf_read_to(handle_t hdl,void* data,size_t size,uint8_t token)
{
    base_t*     base = (base_t*)hdl;
    uint8_t*    p;
    size_t      asize;
    size_t      headlen;
    size_t      tailLen;
    
    if (!base) {
        return 0;
    }
    if (!data) {
        return 0;
    }
    if (!size) {
        return 0;
    }
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    
    asize    = (base->used > size) ? size : base->used;
    tailLen  = base->buffer_end - base->rp;
    if (tailLen > asize) {
        tailLen = asize;
    }
    headlen = asize - tailLen;
    if (tailLen) {
        p = memchr(base->rp, token, tailLen);
        if (p) {
            return rb_read(base, data, (p - base->rp) + 1);
        }
    }
    
    if (headlen) {
        p = memchr(base->buffer_start, token, headlen);
        if (p) {
            return rb_read(base, data, (p - base->buffer_start) + 1);
        }
    }
    
    return rb_read(base,data,size);
}

size_t ringbuf_available(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    return base->capacity - base->used;
}

size_t ringbuf_used(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    return base->used;
}

size_t ringbuf_capacity(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    
    return base->capacity;
}

error_t ringbuf_purge(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != RINGBUFFER_SIGNATURE) {
        return 0;
    }
    
    base->rp   = base->buffer_start;
    base->wp   = base->buffer_start;
    base->used = 0;
    
    return 0;
}

int main(int argc, const char * argv[]) {
    
    handle_t  ringHdl;
    uint8_t*  pMemory;
    char      txdata[TEST_RING_BUFFER_SIZE] = "hello ring buffer";
    char      rxdata[TEST_RING_BUFFER_SIZE * 2] = {0};
    size_t    check_size;
    
    pMemory = (uint8_t*)malloc(TEST_RING_BUFFER_SIZE);
    if (!pMemory) {
        perror("can not malloc memory");
        exit(-1);
    }
    ringHdl = ringbuf_create(pMemory,TEST_RING_BUFFER_SIZE);
    if (!ringHdl) {
        perror("ringbuf_create error");
        exit(-1);
    }
    size_t txdata_len = sizeof(txdata);

//    printf("--start---capacity: %zd----\n",ringbuf_capacity(ringHdl));
//    printf("--start---available: %zd---\n",ringbuf_available(ringHdl));
//    printf("--start---used: %zd---\n",ringbuf_used(ringHdl));
    
    check_size = ringbuf_write(ringHdl, txdata, txdata_len);
    if (check_size != sizeof(txdata)) {
        printf("ringbuf_write error: (%zd)%zd\n",txdata_len,check_size);
        goto ringbuf_writeErr;
    }
    
//    printf("--111---capacity: %zd----\n",ringbuf_capacity(ringHdl));
//    printf("--111---available: %zd---\n",ringbuf_available(ringHdl));
//    printf("--111---used: %zd---\n",ringbuf_used(ringHdl));
    
    check_size = ringbuf_read(ringHdl, rxdata,txdata_len);
    if (check_size != txdata_len) {
        printf("ringbuf_read error: (%zd)%zd\n",txdata_len,check_size);
        goto ringbuf_writeErr;
    }
    puts(rxdata);
//    printf("--222---capacity: %zd----\n",ringbuf_capacity(ringHdl));
//    printf("--222---available: %zd---\n",ringbuf_available(ringHdl));
//    printf("--222---used: %zd---\n",ringbuf_used(ringHdl));
    
    ringbuf_destory(ringHdl);
    free(pMemory);
    pMemory = NULL;
    
//    printf("ringbuf test over\n");
    
    return 0;
    
ringbuf_writeErr:
    ringbuf_destory(ringHdl);
    free(pMemory);
    pMemory = NULL;
    return -1;
}
