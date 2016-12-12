//
//  main.c
//  LW_OOPC
//
//  Created by yhp on 2016/12/11.
//  Copyright © 2016年 YouHuaPei. All rights reserved.
//

#include <stdio.h>
#include "Rectangle.h"

int main(int argc, const char * argv[]) {
    double v;
    struct IA* pr;
    
    pr = (IA*)RectangleNew();
    pr ->init(pr,10.5,20.5);
    v = pr->cal_area(pr);
    printf("area = %7.3f\n",v);
    v = pr->cal_perimeter(pr);
    printf("perimeter = %7.3f\n",v);
    return 0;
}
