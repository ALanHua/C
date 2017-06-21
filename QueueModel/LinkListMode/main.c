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
        if (!pNode->name) {
            free(pNode);
            return NULL;
        }
        strcpy(pNode->name, name);
        pNode->count  = 1;
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
    FILE*        pDumpOutFile;
    // 节点管理
    WORD_NODE_S* (*getWordNode)(char* name);
    void         (*deleteWordNode)(WORD_NODE_S* pNode);
    // 链表管理
    int          (*add)(WORD_LIST_MANAGE* self,char* name);
    WORD_NODE_S* (*search)(WORD_LIST_MANAGE* self,char* name);
    int          (*delete)(WORD_LIST_MANAGE* self,char* name);
    void         (*dump)(WORD_LIST_MANAGE* self);
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
        if (self->pDumpOutFile) {
            fclose(self->pDumpOutFile);
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
    
    if ((result == 0) && self->pWordHeader) {
        pos->count++;
    }else{
        // 添加节点
        pNewWord = self->getWordNode(pWordName);// 暂时不需要判断
        if (pNewWord == NULL) {
            printf("getWordNode error\n");
            return -1;
        }
        if (prev == NULL) {
            pNewWord->pNext    = self->pWordHeader;
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

void dumpWords(WORD_LIST_MANAGE* self)
{
    WORD_NODE_S* pos      = NULL;
    
    if (!self || !self->pDumpOutFile) {
        return;
    }
    for (pos = self->pWordHeader; pos != NULL; pos = pos->pNext) {
        fprintf(self->pDumpOutFile, "%20s:%5d\n",pos->name,pos->count);
        fprintf(stdout, "%s:%d\n",pos->name,pos->count);
    }
}

WORD_LIST_MANAGE* initallocWordListManage(char* pOutFilename)
{
    WORD_LIST_MANAGE* pListManage = NULL;
    
    pListManage = (WORD_LIST_MANAGE*)malloc(sizeof(WORD_LIST_MANAGE));
    if (pListManage) {
        if (pOutFilename) {
            // 文件初始化
            pListManage->pDumpOutFile = fopen(pOutFilename, "wb");
            if (!pListManage->pDumpOutFile) {
                free(pListManage);
                return NULL;
            }
            // 链表初始化
            pListManage->pWordHeader        = NULL;
            pListManage->getWordNode        = initallocWordNode;
            pListManage->deleteWordNode     = deallocWordNode;
            pListManage->add                = addToTail;
            pListManage->search             = searchWord;
            pListManage->delete             = deleteWord;
            pListManage->dump               = dumpWords;
            pListManage->dealloc            = deallocWordListManage;
        }
    }
    
    return pListManage;
}


int main(int argc, const char * argv[]) {
    
    WORD_LIST_MANAGE* pList = NULL;
    
    pList = initallocWordListManage("/Users/smartwater/Downloads/dump.txt");
    if (!pList) {
        printf("malloc list errot\n");
        return -1;
    }
    
    pList->add(pList,"bace");
    pList->add(pList,"bace");
    pList->add(pList,"baav");
    pList->add(pList,"b1223");
    pList->dump(pList);
    
    
    pList->dealloc(pList);
    
    return 0;
}
