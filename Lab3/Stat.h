//
// Created by Lei Guo on 2017/4/14.
//

#ifndef OS_LAB3_STAT_H
#define OS_LAB3_STAT_H

#include "PTE.h"
#include <stdio.h>
#include <list>
#include <vector>
// note that I also put global variables in this header file.
class Stat
{
public:
    int num_inst;
    int num_unmap;
    int num_map;
    int num_in;
    int num_out;
    int num_zero;
    Stat() : num_inst(0), num_unmap(0), num_map(0), num_in(0), num_out(0), num_zero(0) {}

    void print_stat()
    {
        unsigned long long total_cost;
        total_cost = (unsigned long long) (num_inst + (num_map + num_unmap) * 400 +
                (num_in + num_out) * 3000 + num_zero * 150);
        printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n", num_inst, num_unmap, num_map, num_in, num_out, num_zero, total_cost);
    }
};
#endif //OS_LAB3_STAT_H
