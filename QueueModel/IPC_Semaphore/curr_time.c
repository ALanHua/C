//
//  curr_time.c
//  IPC_Semaphore
//
//  Created by yhp on 2017/10/27.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//

#include "curr_time.h"

#define BUF_SIZE  1000
#define CHECK_FORMAT(a)     (((a) != NULL) ? (a) : "c")

char* currTime(const char* format)
{
    static char buf[BUF_SIZE];
    time_t      t;
    size_t      s;
    struct tm*  tm;
    
    t   = time(NULL);
    tm  = localtime(&t);
    if (NULL == tm) {
        printf("can not get time\n");
        return NULL;
    }
    
    s = strftime(buf, BUF_SIZE, CHECK_FORMAT(format),tm);
    
    return (s == 0) ? NULL : buf;
}
