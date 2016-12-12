//
//  Book.c
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#include <string.h>
#include "Book.h"


static void set(void* t,char* book_no,char* tittle,char* name,char* tel)
{
    Book* cthis = (Book*)t;
    /*该处为做字符串长度检查，存在问题*/
    strcpy(cthis->book_no,book_no);
    strcpy(cthis->tittle,tittle);
    cthis->author = AuthorNew();
    cthis->author->init(cthis->author,name,tel);
}

static void print(void* t)
{
    Book* cthis = (Book*)t;
    Author* pAuthor;
    printf("book_no:%s,tittle:%s\n",cthis->book_no,cthis->tittle);
    pAuthor = cthis->author;
    printf("author_name:%s,author_tel:%s\n",pAuthor->get_name(pAuthor),pAuthor->get_tel_no(pAuthor));
}

CTOR(Book)
FUNCTION_SETTING(set, set)
FUNCTION_SETTING(print, print)
END_CTOR
