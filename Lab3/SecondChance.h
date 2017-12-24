//
// Created by Lei Guo on 2017/4/19.
//

#ifndef OS_LAB3_SECONDCHANCE_H
#define OS_LAB3_SECONDCHANCE_H
#include "Pager.h"
#include "PTE.h"
class SecondChance : public Pager
{
public:
    SecondChance(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table) :
            Pager(_type, _page_table, _frame_table) {}
    ~SecondChance() {}
    int get_frame()
    {
        bool found = false;
        int phys_idx = 0;
        int candidate = 0;
        int virt_idx = 0;
        while(!found)
        {
            candidate = valid_list.front();
            virt_idx = (*frame_table)[candidate];
            if((*page_table)[virt_idx].refered==1)
            {
                (*page_table)[virt_idx].refered = 0;
                valid_list.erase(valid_list.begin());
                valid_list.push_back(candidate);
            }
            else
            {
                valid_list.erase(valid_list.begin());
                phys_idx = candidate;
                found = true;
            }
        }
        return phys_idx;
    }
};
#endif //OS_LAB3_SECONDCHANCE_H
