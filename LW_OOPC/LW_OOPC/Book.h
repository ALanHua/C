//
//  Book.h
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#ifndef Book_h
#define Book_h

#include <stdio.h>
#include "lw_oopc.h"
#include "Author.h"

CLASS(Book)
{
    void(*set)(void* t,char* book_no,char* tittle,char* name,char* tel);
    void(*print)(void* t);
    char book_no[14];
    char tittle[20];
    Author* author;
};

extern void*BookNew();

#endif /* Book_h */
