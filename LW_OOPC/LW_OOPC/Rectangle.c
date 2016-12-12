//
//  Rectangle.c
//  LW_OOPC
//
//  Created by yhp on 2016/12/12.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#include "Rectangle.h"

/**
 计算矩形面积
 @param t 这个对象
 @return 面积
 */
double cal_area_imp(void* t)
{
    Rectangle* cthis = (Rectangle*)t;
    return cthis->lenght * cthis->width;
}

/**
 计算矩形周长
 @param t 这个对象
 @return 周长
 */
double cal_perimeter_imp(void* t)
{
    Rectangle* cthis = (Rectangle*)t;
    return (cthis->lenght + cthis->width) * 2;
}

/**
 初始化话对象属性
 @param t 这个对象
 @param length 长
 @param width 宽
 */
void init_imp(void* t,double length,double width)
{
    Rectangle* cthis = (Rectangle*)t;
    cthis->lenght = length;
    cthis->width = width;
}

CTOR(Rectangle)
    FUNCTION_SETTING(IA.init, init_imp)
    FUNCTION_SETTING(IA.cal_area, cal_area_imp)
    FUNCTION_SETTING(IA.cal_perimeter, cal_perimeter_imp)
END_CTOR






