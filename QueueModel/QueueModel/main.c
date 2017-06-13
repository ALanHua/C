//
//  main.c
//  QueueModel
//
//  Created by yhp on 2017/6/1.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#pragma mark - QUEUE_NODE
typedef struct dspQueueNodeTag DSP_QUEUE_NODE_S;
struct dspQueueNodeTag
{
    int32_t         length;       /* 队列深度*/
    uint64_t*        queue;        /* 数据地址*/
    void (*dealloc)(DSP_QUEUE_NODE_S* pDspQueueNode);
};

void deallocNode(DSP_QUEUE_NODE_S* pDspQueueNode)
{
    if (pDspQueueNode) {
        free(pDspQueueNode->queue);
        free(pDspQueueNode);
    }
}

DSP_QUEUE_NODE_S* initallocNode(int32_t nodeNum)
{
    DSP_QUEUE_NODE_S*  pDspQueueNode    = NULL;
    if (nodeNum > 0) {
        pDspQueueNode = (DSP_QUEUE_NODE_S*)malloc(sizeof(DSP_QUEUE_NODE_S));
        if (pDspQueueNode) {
            // 成员属性
            pDspQueueNode->length = nodeNum;
            pDspQueueNode->queue = (uint64_t*)malloc(sizeof(uint64_t)*nodeNum);
            if (!pDspQueueNode->queue) {
                free(pDspQueueNode);
                return NULL;
            }
            // 功能函数
            pDspQueueNode->dealloc = deallocNode;
        }
    }
    return pDspQueueNode;
}


#pragma mark - QUEUE
typedef struct dspQueueHandleTag DSP_QUEUE_HANDLE_S;
struct dspQueueHandleTag
{
    int32_t            rdPos;      /* 读指正*/
    int32_t            wrPos;     /* 写指正*/
    int32_t            count;        /* 节点个数*/
    DSP_QUEUE_NODE_S*  pNode;
    pthread_mutex_t    lock;         /* 互斥锁*/
    void*              priv;
//    pthread_cond_t     condRd;
//    pthread_cond_t     condWr;
//  puublic
    // 函数
    void (*dealloc)(DSP_QUEUE_HANDLE_S* pStDspQueueHandle);
    int32_t (*push)(DSP_QUEUE_HANDLE_S* pStDspQueueHandle,uint64_t dataAddr);
    uint64_t (*pop)(DSP_QUEUE_HANDLE_S* pStDspQueueHandle);
    bool (*isEmpty)(DSP_QUEUE_HANDLE_S* pStDspQueueHandle);
    bool(*isFull)(DSP_QUEUE_HANDLE_S* pStDspQueueHandle);
};


void deallocQueue(DSP_QUEUE_HANDLE_S* pDspQueueHandle)
{
    if (pDspQueueHandle) {
        pDspQueueHandle->pNode->dealloc(pDspQueueHandle->pNode);
        free(pDspQueueHandle);
    }
}

bool isQueueEmpty(DSP_QUEUE_HANDLE_S* pStDspQueueHandle)
{
    if (pStDspQueueHandle->count == 0) {
        return true;
    }
    return false;
}

bool isQueueFull(DSP_QUEUE_HANDLE_S* pStDspQueueHandle)
{
    if (pStDspQueueHandle->count == pStDspQueueHandle->pNode->length) {
        return true;
    }
    return false;
}

int32_t queuePush(DSP_QUEUE_HANDLE_S* pStDspQueueHandle,uint64_t dataAddr)
{
    int32_t  wrPos = pStDspQueueHandle->wrPos;
    int32_t queueDeep = pStDspQueueHandle->pNode->length;
    
    if (!pStDspQueueHandle->isFull(pStDspQueueHandle)) {
        pthread_mutex_lock(&pStDspQueueHandle->lock);
        pStDspQueueHandle->pNode->queue[wrPos] = dataAddr;
        pStDspQueueHandle->wrPos = (wrPos + 1) % queueDeep;
        pStDspQueueHandle->count++;
        pthread_mutex_unlock(&pStDspQueueHandle->lock);
        return true;
    }
    
    return false;
}

uint64_t queuePop(DSP_QUEUE_HANDLE_S* pStDspQueueHandle)
{
    int32_t rdPos = pStDspQueueHandle->rdPos;
    int32_t queueDeep = pStDspQueueHandle->pNode->length;
    uint64_t nodeData;
    
    if (!pStDspQueueHandle->isEmpty(pStDspQueueHandle)) {
        pthread_mutex_lock(&pStDspQueueHandle->lock);
        nodeData = pStDspQueueHandle->pNode->queue[rdPos];
        pStDspQueueHandle->rdPos = (rdPos + 1) % queueDeep;
        pStDspQueueHandle->count--;
        pthread_mutex_unlock(&pStDspQueueHandle->lock);
        return nodeData;
    }
    return false;
}

DSP_QUEUE_HANDLE_S* initallocQueue(int32_t queueDeep)
{
    DSP_QUEUE_HANDLE_S* pDspQueueHandle = NULL;
    
    pDspQueueHandle = (DSP_QUEUE_HANDLE_S*)malloc(sizeof(DSP_QUEUE_HANDLE_S));
    if (pDspQueueHandle) {
        // 成员属性
        pDspQueueHandle->pNode     = initallocNode(queueDeep);
        if (!pDspQueueHandle->pNode) {
            free(pDspQueueHandle);
            return NULL;
        }
        pDspQueueHandle->rdPos   = 0;
        pDspQueueHandle->wrPos   = 0;
        pDspQueueHandle->count   = 0;
        pthread_mutex_init(&pDspQueueHandle->lock, NULL);
        
        //功能函数
        pDspQueueHandle->dealloc = deallocQueue;
        pDspQueueHandle->isEmpty = isQueueEmpty;
        pDspQueueHandle->isFull  = isQueueFull;
        pDspQueueHandle->pop     = queuePop;
        pDspQueueHandle->push    = queuePush;
    }
    return pDspQueueHandle;
}

#define ARRAY_SIZE  10
int main(int argc, const char * argv[]) {

    uint32_t array[ARRAY_SIZE] = {1,2,3,4,5,6,7,8,9,10};
    
    uint32_t* pArray = (uint32_t*)malloc(ARRAY_SIZE);
    
//    memcpy(pArray, array, ARRAY_SIZE);
    for (int32_t i = 0; i < ARRAY_SIZE; i++) {
        pArray[i] = array[i];
    }
    
    DSP_QUEUE_HANDLE_S* pDspQueueHnadle = NULL;
    
    pDspQueueHnadle = initallocQueue(ARRAY_SIZE);
    
    for (int32_t i = 0; i < ARRAY_SIZE; i++) {
        printf("data:%p,%zd\n",&pArray[i],pArray[i]);
        pDspQueueHnadle->push(pDspQueueHnadle,(uint64_t)&pArray[i]);
    }
    printf("\n------%p-------\n",pDspQueueHnadle);
    uint64_t pp;
    for (int32_t i = 0; i < ARRAY_SIZE; i++) {
        pp = pDspQueueHnadle->pop(pDspQueueHnadle);
        printf("data:%d\n",*((uint32_t*)pp));
    }
    pDspQueueHnadle->dealloc(pDspQueueHnadle);
    
    return 0;
}
