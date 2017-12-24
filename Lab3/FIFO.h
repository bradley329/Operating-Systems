//
// Created by Lei Guo on 2017/4/19.
//

#ifndef OS_LAB3_FIFO_H
#define OS_LAB3_FIFO_H
#include "Pager.h"
#include "PTE.h"
class FIFO : public Pager
{
public:
    FIFO(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table) :
            Pager(_type, _page_table, _frame_table) {}
    ~FIFO() {}
    int get_frame()
    {
        int phys_idx = valid_list.front();
        valid_list.erase(valid_list.begin());
        return phys_idx;
    }
};
#endif //OS_LAB3_FIFO_H
