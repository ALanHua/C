//
//  Author.h
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#ifndef Author_h
#define Author_h

#include <stdio.h>
#include "lw_oopc.h"

CLASS(Author)
{
    void(*init)(void* t,char* name,char* telphone);
    char* (*get_name)(void* t);
    char* (*get_tel_no)(void* t);
    char name[14];
    char telphone[20];
};

extern void*AuthorNew();

#endif /* Author_h */
