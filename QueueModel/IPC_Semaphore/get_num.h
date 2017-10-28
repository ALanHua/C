//
//  get_num.h
//  IPC_Semaphore
//
//  Created by yhp on 2017/10/27.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//

#ifndef get_num_h
#define get_num_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define GN_NONNEG       01
#define GN_GT_0         02

#define GN_ANY_BASE     0100
#define GN_BASE_8       0200
#define GN_BASE_16      0400
int getInt(const char *arg, int flags, const char *name);
long getLong(const char *arg, int flags, const char *name);

#endif /* get_num_h */
