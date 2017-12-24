//
// Created by Lei Guo on 2017/4/19.
//

#ifndef OS_LAB3_PHYS_CLOCK_H
#define OS_LAB3_PHYS_CLOCK_H
#include "Pager.h"
#include "PTE.h"
class Clock : public Pager
{
private:
    int num_phys;
    int num_virt;
    int hand;
public:
    Clock(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table, int _num_phys, int _num_virt) :
            Pager(_type, _page_table, _frame_table)
    {
        hand = 0;
        num_phys = _num_phys;
        num_virt = _num_virt;
    }
    ~Clock() {}

    int get_frame()
    {
        if(type==5)
        {
            return get_phys_frame();
        }
        else
        {
            return get_virt_frame();
        }
    }

    int get_phys_frame()
    {
        int phys_idx = 0;
        bool found = false;
        while(!found)
        {
            int candidate = valid_list.front(); //phys_idx
            int virt_idx = (*frame_table)[candidate];
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

    int get_virt_frame()
    {
        int phys_idx = 0;
        bool found = false;
        while(!found)
        {
            if((*page_table)[hand].present==1)
            {
                if((*page_table)[hand].refered==1)
                {
                    (*page_table)[hand].refered = 0;
                }
                else
                {
                    phys_idx = (*page_table)[hand].phys_idx;
                    found = true;
                }
            }
            hand++;
            if(hand == num_virt)
                hand = 0;
        }
        return phys_idx;
    }
};
#endif //OS_LAB3_PHYS_CLOCK_H
