//
//  main.c
//  memblock
//
//  Created by yhp on 2018/7/28.
//  Copyright © 2018年 YouHuaPei. All rights reserved.
//

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define PACK(a,b,c,d) (uint32_t)( ((a) << 24) | ((b) << 16) | ((c) << 8) | d)
#define FIXED_MEMORYPOOL_SIGNATURE PACK('f','m','p','l')

#define CONFIG_NUMOF_FIXEDMEMORYPOOL_HANDLES (2)
#define NULL_SIGNATURE                      (0x00000000)

// TEST
#define BLOCK_SIZE      (24)
#define BLOCK_COUNT     (4)
#define POOL_SIZE       ((BLOCK_SIZE) * (BLOCK_COUNT))

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
typedef void*  handle_t;

typedef struct block {
    struct blockheader {
        bool using;
        struct block* next_free_blk;
    }header;
    uint8_t data[];
}block_t;
typedef struct blockheader blockheader_t;

typedef struct{
    uint32_t    signature;
    block_t*    free;
    size_t      ava_blks;
    size_t      blksz;
    size_t      blkcnt;
}base_t;

/* ========================================================================== */
/*                          变量声明区                                        */
/* ========================================================================== */
static uint8_t s_pool[POOL_SIZE];
static size_t s_blocksize  = BLOCK_SIZE;
static size_t s_blockcount = BLOCK_COUNT;
static handle_t s_handle   = NULL;
/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
static base_t* get_object(void)
{
    static base_t objs[CONFIG_NUMOF_FIXEDMEMORYPOOL_HANDLES];
    static bool has_inited = 0;
    int32_t    i;
    
    if (!has_inited) {
        memset(objs, 0, sizeof(objs));
        has_inited = 1;
    }
    
    for (i = 0; i < CONFIG_NUMOF_FIXEDMEMORYPOOL_HANDLES; i++) {
        if (objs[i].signature == NULL_SIGNATURE) {
            return &objs[i];
        }
    }
    
    return NULL;
}

/**
 初始化内存链表
 @param base 内存内部对象
 @param memory 起始地址
 @param blksz 块大小
 @param blkcnt 块数量
 */
static void initialize_blocks(base_t* base,void* memory,size_t blksz,size_t blkcnt)
{
    block_t*    blk;
    uint8_t*    p;
    size_t      i,end = blkcnt -1;
    
    base->ava_blks = blkcnt;
    base->blksz    = blksz;
    base->blkcnt   = blkcnt;
    base->free     = memory;
    p = (uint8_t*)base->free;
    
    for (i = 0; i < end; i++) {
        blk = (block_t*)p;
        p += blksz;
        blk->header.using = false;
        blk->header.next_free_blk = (block_t*)p;
    }
//    拿到最后一个节点
    blk = (block_t*)p;
    blk->header.next_free_blk = NULL;
}

static void* allocate_block(base_t* base)
{
    block_t* blk;
    
    if (!base->free) {
        return NULL;
    }
    
    blk = base->free;
    base->free = blk->header.next_free_blk;
    blk->header.using = true;
    blk->header.next_free_blk = NULL;
    base->ava_blks--;
    return (void*)blk->data;
}

static int free_block(base_t* base,void* data)
{
    //    拿到blk的起始地址
    block_t *blk = (block_t*)((uint8_t*)data - sizeof(blockheader_t));
    if (!blk->header.using) {
        return -1;
    }
    blk->header.using = false;
    blk->header.next_free_blk = base->free;
    base->free = blk;
    base->ava_blks++;
    return 0;
}

/**
 创建块内存对象
 @param memory 起始地址
 @param blksz  块大小
 @param blkcnt 块数量
 @return 块对象句柄
 */
handle_t fixedmpool_create(void* memory,size_t blksz,size_t blkcnt)
{
    base_t* base;
    
    if (!memory || !blkcnt ||
        (blksz == sizeof(blockheader_t))) {
        return NULL;
    }
    
    base = get_object();
    if (!base) {
        return NULL;
    }
    base->signature = FIXED_MEMORYPOOL_SIGNATURE;
    initialize_blocks(base, memory, blksz, blkcnt);
    
    return NULL;
}

/**
 销毁块对象
 @param hdl 块对象句柄
 @return 0 成功 -1 失败
 */
int fixedmpool_destroy(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return -1;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return -1;
    }
    
    base->signature = NULL_SIGNATURE;
    
    return 0;
}


/**
 申请块地址
 @param hdl 块对象句柄
 @return 块地址
 */
void* fixedmpool_allocate(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return NULL;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return NULL;
    }
    
    return allocate_block(base);
}


/**
 释放块地址
 @param hdl 块对象句柄
 @param block 块地址
 @return 0 成功 <0 失败
 */
int fixedmpool_free(handle_t hdl, void *block)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return -1;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return -1;
    }
    return free_block(base, block);
}

/**
 获取块数量
 @param hdl 块对象句柄
 @return 实际块数量
 */
size_t fixedmpool_available_blocks(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return 0;
    }
    
    return base->ava_blks;
}

/**
 获取块内存的长度
 @param hdl  块对象句柄
 @return 每块内存的长度
 */
size_t fixedmpool_blockdata_size(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return 0;
    }
    
    return base->blksz - sizeof(blockheader_t);
}

/**
 获取已经使用的内存块
 @param hdl 块对象句柄
 @return 已经使用的内存
 */
size_t fixedmpool_used_blocks(handle_t hdl)
{
    base_t* base = (base_t*)hdl;
    if (!base) {
        return 0;
    }
    
    if (base->signature != FIXED_MEMORYPOOL_SIGNATURE) {
        return 0;
    }
    
    return base->blkcnt - base->ava_blks;
}

int main(int argc, const char * argv[])
{
    uint8_t* p[BLOCK_COUNT];
    uint8_t* q[BLOCK_COUNT];
    size_t i;
    int random_idxs[BLOCK_COUNT] = {3,0,1,2};
    
    s_handle = fixedmpool_create(s_pool, s_blocksize, s_blockcount);
    assert(s_handle == NULL);
    for (i = 0; i < s_blockcount; i++) {
        p[i] = q[i] = fixedmpool_allocate(s_handle);
        assert(p[i] == NULL);
        assert(q[i] == NULL);
    }
    
    assert(fixedmpool_free(s_handle, p[0]) != 0);
    p[0] = fixedmpool_allocate(s_handle);
    assert(p[0] == NULL);
    
    for (i = 0; i < s_blockcount; i++) {
        int idx = random_idxs[i];
        assert(fixedmpool_free(s_handle, p[idx]) != 0);
    }
    
    printf(" simple test ok \n");
    
    return 0;
}
