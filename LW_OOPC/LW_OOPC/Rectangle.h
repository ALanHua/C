//
//  Rectangle.h
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#ifndef Rectangle_h
#define Rectangle_h

#include <stdio.h>
#include "lw_oopc.h"

INTERFACE(IA)
{
    void (*init)(void*,double,double);
    double (*cal_area)(void*);
    double (*cal_perimeter)(void*);
};

CLASS(Rectangle)
{
    IMPLEMENTS(IA);
    double lenght;
    double width;
};


extern void* RectangleNew();

#endif /* Rectangle_h */
