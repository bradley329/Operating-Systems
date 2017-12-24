//
// Created by Lei Guo on 2017/4/14.
//

#ifndef OS_LAB3_PTE_H
#define OS_LAB3_PTE_H

class PTE
{
public:
    unsigned int present :1;
    unsigned int modified :1;
    unsigned int refered :1;
    unsigned int pageout :1;
    unsigned int phys_idx :28;

    PTE() : present(0), modified(0), refered(0), pageout(0), phys_idx(0) {}
};
#endif //OS_LAB3_PTE_H
