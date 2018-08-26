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
#include <string.h>
#include <assert.h>
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define    ENOMEM        12    /* Cannot allocate memory */
#define    EINVAL        22    /* Invalid argument */
#define AVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a,b,c) FFMAX(FFMAX(a,b),c)
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMIN3(a,b,c) FFMIN(FFMIN(a,b),c)
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
    f->rptr = f->wptr = f->buffer;
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


/**
 Initialize an AVFifoBuffer.
 @param memb nmemb number of elements
 @param size size of the single element
 @return AVFifoBuffer or NULL in case of memory allocation failure
 */
AVFifoBuffer* av_fifo_alloc_array(size_t memb,size_t size)
{
    void* buffer = malloc(memb * size);
    return fifo_alloc_common(buffer,memb * size);
}

/**
 It's a compound literal (even though void * isn't a compound type). The compiler allocates a constant void * holding NULL, and then (&(void *){ NULL }) is a pointer to that. It's essentially equivalent to:
 static void* const temp = NULL;
 memcpy(arg, &temp, sizeof(val));
 @param arg ptr Pointer to the pointer to the memory block which should be freed
 */
static void av_freep(void* arg)
{
    void*  val;
    memcpy(&val,arg,sizeof(val));
    memcpy(arg, &(void*){NULL}, sizeof(val));
    free(val);
}

void av_fifo_free(AVFifoBuffer* f)
{
    if (f) {
        av_freep(&f->buffer);
        free(f);
    }
}
void av_fifo_freep(AVFifoBuffer** f)
{
    if (f) {
        av_fifo_free(*f);
        *f = NULL;
    }
}

int av_fifo_size(const AVFifoBuffer* f)
{
    return (uint32_t)(f->wndx - f->rndx);
}

int av_fifo_space(const AVFifoBuffer* f)
{
    return (uint32_t)(f->end - f->buffer) - av_fifo_size(f);
}

void av_fifo_drain(AVFifoBuffer* f,int size)
{
    assert(av_fifo_size(f) >= size);
    f->rptr += size;
    if (f->rptr >= f->end) {
        f->rptr -= f->end - f->buffer;
    }
    f->rndx += size;
}

int av_fifo_generic_read(AVFifoBuffer* f,void* dest,int buf_size,
                         void (*func)(void*,void*,int))
{
    //    需要内存臂章，或者加锁
    do{
        int len = FFMIN((int)(f->end - f->rptr),buf_size);
        if (func) {
            func(dest,f->rptr,len);
        }else{
            memcpy(dest, f->rptr, len);
            dest =(uint8_t*)dest + len;
        }
    //    需要内存臂章，或者加锁
        av_fifo_drain(f,len);
        buf_size -= len;
    }while (buf_size > 0);
    return 0;
}

int av_fifo_realloc2(AVFifoBuffer* f,unsigned int new_size)
{
    uint32_t old_size = (uint32_t)(f->end - f->buffer);
    
    if (old_size < new_size) {
        int32_t len = av_fifo_size(f);
        AVFifoBuffer* f2 = av_fifo_alloc(new_size);
        if (!f2) {
            return AVERROR(ENOMEM);
        }
        av_fifo_generic_read(f, f2->buffer, len, NULL);
        f2->wptr += len;
        f2->wndx += len;
        free(f->buffer);
        *f = *f2;
        free(f2);
    }
    return 0;
}

int av_fifo_grow(AVFifoBuffer* f,unsigned int size)
{
    uint32_t old_size = (uint32_t)(f->end -f->buffer);
    if (size + av_fifo_size(f) < size) {
         return AVERROR(EINVAL);
    }
    
    size += av_fifo_size(f);
    if (old_size < size) {
        return av_fifo_realloc2(f, FFMAX(size, 2*old_size));
    }
    return 0;
}

int av_fifo_generic_write(AVFifoBuffer* f,void* src,
                          int size,int(*func)(void*,void* ,int))
{
    int32_t  total = size;
    uint32_t wndx  = f->wndx;
    uint8_t* wptr = f->wptr;
    
    do {
        int32_t len = FFMIN((int32_t)(f->end -wptr),size);
        if (func) {
            len = func(src,wptr,len);
            if (len == 0) {
                break;
            }
        }else{
            memcpy(wptr, src, len);
            src = (uint8_t*)src + len;
        }
        wptr += len;
        if (wptr >= f->end) {
            wptr = f->buffer;
        }
        wndx += len;
        size -= len;
    } while (size > 0);
    f->wndx = wndx;
    f->wptr = wptr;
    
    return total - size;
}

int av_fifo_generic_peek_at(AVFifoBuffer* f,void* dest,int offset,
                        int buf_size,void (*func)(void*, void*, int))
{
    uint8_t* rptr = f->rptr;
    
    assert(offset >=0);
    assert(buf_size + offset <= f->wptr - f->rptr);
    
    if (offset >= f->end - rptr) {
        rptr += offset - (f->end - f->buffer);
    }else{
        rptr += offset;
    }
    /*
     f->rptr 并没有真正移动，只是copy 了数据
     */
    while (buf_size > 0) {
        int len;
        
        if (rptr >= f->end) {
            rptr -= f->end - f->buffer;
        }
        
        len = FFMIN((int32_t)(f->end - f->rptr), buf_size);
        if (func) {
            func(dest,rptr,len);
        }else{
            memcpy(dest, rptr, len);
            dest = (uint8_t*)dest + len;
        }
        
        buf_size -= len;
        rptr += len;
    }
    return 0;
}

int av_fifo_generic_peek(AVFifoBuffer* f,void* dest,int buf_size,
                         void (*func)(void*, void*, int))
{
    uint8_t* rptr = f->rptr;
    
    do {
        int len = FFMIN((uint32_t)(f->end - f->rptr),buf_size);
        if (func) {
            func(dest,rptr,len);
        }else{
            memcpy(dest, rptr, len);
            dest = (uint8_t*)dest + len;
        }
        
        rptr += len;
        if (rptr >= f->end) {
            rptr -= f->end - f->buffer;
        }
        
        buf_size -= len;
        
    } while (buf_size > 0);
    
    return 0;
}

uint8_t *av_fifo_peek2(const AVFifoBuffer *f, int offs)
{
    uint8_t* ptr = f->rptr + offs;
    
    if (ptr >= f->end) {
        ptr = f->buffer + (ptr - f->end);
    }else if (ptr < f->buffer){
        ptr = f->end - (f->buffer - ptr);
    }
    return ptr;
}

int main(int argc, const char * argv[])
{
    AVFifoBuffer* fifo = av_fifo_alloc(13 * sizeof(int));
    int i,j,n,*p;
    
    /* fill date */
    for (i = 0; av_fifo_space(fifo) >= sizeof(int); i++) {
        av_fifo_generic_write(fifo, &i, sizeof(int), NULL);
    }
    /* peek at FIF0 */
    n = av_fifo_size(fifo) / sizeof(int);
    for (i = -n + 1; i < n; i++) {
        int* v = (int*)av_fifo_peek2(fifo,i*sizeof(int));
        printf("peek:%d,%d\n",i,*v);
    }
    printf("TEST-----000--------\n");
    /* generic peek at FIFO */
    n = av_fifo_size(fifo);
    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "failed to allocate memory.\n");
        exit(1);
    }
    av_fifo_generic_peek(fifo, p, n, NULL);
    /* read data at p */
    n /= sizeof(int);
    for (i = 0; i < n; i++) {
        printf("dump :%d,%d\n",i,p[i]);
    }
     /* read data */
    printf("TEST-----111--------\n");
    for (i = 0; av_fifo_size(fifo) >= sizeof(int); i++) {
        av_fifo_generic_read(fifo, &j, sizeof(int), NULL);
        printf("%d ", j);
    }
    printf("\n");
   
    /* test *ndx overflow */
    av_fifo_reset(fifo);
    fifo->rndx = fifo->wndx = ~(uint32_t)0 -5;
    
    /* fill data */
    for (i = 0; av_fifo_space(fifo) >= sizeof(int); i++)
        av_fifo_generic_write(fifo, &i, sizeof(int), NULL);
    /* peek_at at FIFO */
    n = av_fifo_size(fifo) / sizeof(int);
    for (i = 0; i < n; i++) {
        av_fifo_generic_peek_at(fifo, &j, i * sizeof(int), sizeof(j), NULL);
        printf("%d: %d\n", i, j);
    }
    putchar('\n');
    
    av_fifo_free(fifo);
    free(p);
    
    return 0;
}
