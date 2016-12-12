//
//  Author.c
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#include <string.h>
#include "Author.h"

static void init(void* t,char* name,char* telphone)
{
    Author* cthis = (Author*)t;
    strcpy(cthis->name, name);
    strcpy(cthis->telphone, telphone);
}

static char* get_name(void* t)
{
    Author* cthis = (Author*)t;
    return cthis->name;
}

static char* get_tel_no(void* t)
{
    Author* cthis = (Author*)t;
    return cthis->telphone;
}

CTOR(Author)
FUNCTION_SETTING(init, init)
FUNCTION_SETTING(get_name, get_name)
FUNCTION_SETTING(get_tel_no, get_tel_no)
END_CTOR


