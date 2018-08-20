//
//  main.c
//  AVFifoBuffer
//
//  Created by yhp on 2018/8/20.
//  Copyright © 2018年 YouHuaPei. All rights reserved.
//
/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h>

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
typedef struct AVFifoBuffer{
    uint8_t  *buffer;
    uint8_t  *rptr,*wptr,*end;
    uint32_t rndx,wndx;
}AVFifoBuffer;
/* ========================================================================== */
/*                          变量声明区                                        */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/**
 Reset the AVFifoBuffer to the state right after av_fifo_alloc, in particular it is emptied.
 @param f AVFifoBuffer to reset
 */
void av_fifo_reset(AVFifoBuffer *f)
{
    f->wptr = f->wptr = f->buffer;
    f->wndx = f->rndx = 0;
}

/**
 Initialize an AVFifoBuffer.
 @param buffer start addr
 @param size size of FIFO
 @return AVFifoBuffer or NULL in case of memory allocation failure
 */
static AVFifoBuffer* fifo_alloc_common(void* buffer,size_t size)
{
    AVFifoBuffer* f;
    
    if (!buffer) {
        return NULL;
    }
    
    f = malloc(sizeof(*f));
    if (!f) {
        free(buffer);
        return NULL;
    }
    
    f->buffer = buffer;
    f->end = f->buffer + size;
    av_fifo_reset(f);
    
    return f;
}

/**
 Initialize an AVFifoBuffer.
 @param size size of FIFO
 @return AVFifoBuffer or NULL in case of memory allocation failure
 */
AVFifoBuffer *av_fifo_alloc(unsigned int size)
{
    void* buffer = malloc(size);
    return fifo_alloc_common(buffer,size);
}

int main(int argc, const char * argv[])
{
    
    
    return 0;
}
