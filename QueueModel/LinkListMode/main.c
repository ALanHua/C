//
//  main.c
//  LinkListMode
//
//  Created by yhp on 2017/6/18.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tagWordNode WORD_NODE_S;
struct tagWordNode
{
    char*           name;
    int             count;
    WORD_NODE_S*    pNext;
};

WORD_NODE_S* initallocWordNode(char* name)
{
    WORD_NODE_S* pNode   = NULL;
    size_t       nameLen = 0;
    
    if (!name) {
        return NULL;
    }
    nameLen = strlen(name) + 1;
    pNode = (WORD_NODE_S*)malloc(sizeof(WORD_NODE_S));
    if (pNode != NULL) {
        pNode->name   = (char*)malloc(sizeof(nameLen));
        if (pNode->name) {
            free(pNode);
            return NULL;
        }
        memcpy(pNode->name, name, nameLen);
        pNode->count  = 0;
        pNode->pNext  = NULL;
    }
    return pNode;
}

void deallocWordNode(WORD_NODE_S* pNode)
{
    if (pNode) {
        free(pNode->name);
        free(pNode);
        pNode = NULL;
    }
}

typedef struct tagWordListManage WORD_LIST_MANAGE;
struct tagWordListManage
{
    WORD_NODE_S* pWordHeader;
    
    // 节点管理
    WORD_NODE_S* (*getWordNode)(char* name);
    void         (*deleteWordNode)(WORD_NODE_S* pNode);
    // 链表管理
    int          (*add)(WORD_LIST_MANAGE* self,char* name);
    WORD_NODE_S* (*search)(WORD_LIST_MANAGE* self,char* name);
    int          (*delete)(WORD_LIST_MANAGE* self,char* name);
    
//  析构
    void         (*dealloc)(WORD_LIST_MANAGE* self);
};

/**
 析构WORD_LIST_MANAGE 结构体
 @param self WORD_LIST_MANAGE自己
 */
void deallocWordListManage(WORD_LIST_MANAGE* self)
{
    WORD_NODE_S* temp = NULL;
    
    if (self) {
        if (self->pWordHeader) {
            while (self->pWordHeader) {
                temp = self->pWordHeader;
                self->pWordHeader = temp->pNext;
                self->deleteWordNode(temp);
                temp = NULL;
            }
        }
        free(self);
        self = NULL;
    }
}

/**
 想链表尾添加元素
 @param self  WORD_LIST_MANAGE
 @param pWordName 节点数据
 */
int addToTail(WORD_LIST_MANAGE* self,char* pWordName)
{
    WORD_NODE_S* pos      = NULL;
    WORD_NODE_S* prev     = NULL;
    WORD_NODE_S* pNewWord = NULL;
    int          result   = -1;
    
    if (!self || !pWordName) {
        return result;
    }
    // 遍历链表
    for (pos = self->pWordHeader; pos != NULL; pos = pos->pNext) {
        result = strcmp(pos->name, pWordName);
        if (result >= 0) {
            break;
        }
        prev = pos;
    }
    
    if ((result == 0) && !self->pWordHeader) {
        pos->count++;
    }else{
        // 添加节点
        pNewWord = self->getWordNode(pWordName);// 暂时不需要判断
        if (prev == NULL) {
            pNewWord->pNext    = pos;
            self->pWordHeader  = pNewWord;
        }else{
            prev->pNext        = pNewWord;
            pNewWord->pNext    = pos;
        }
    }
    
    return 0;
}

WORD_NODE_S* searchWord(WORD_LIST_MANAGE* self,char* pWordName)
{
    WORD_NODE_S* pos      = NULL;
    int          result   = -1;
    
    if (!self || !pWordName) {
        return NULL;
    }
    
    for (pos = self->pWordHeader; pos != NULL; pos = pos->pNext) {
        result = strcmp(pos->name, pWordName);
        if (result == 0) {
            break;
        }
    }
    if (result == 0) {
        return pos;
    }
    
    return NULL;
}

int deleteWord(WORD_LIST_MANAGE* self,char* pWordName)
{
   WORD_NODE_S* pos      = NULL;
    
    if (!self || !pWordName) {
        return -1;
    }
    pos = self->search(self,pWordName);
    if (pos) {
        self->deleteWordNode(pos);
         return 0;
    }
    return -1;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
