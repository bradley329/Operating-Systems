//
// Created by Lei Guo on 2017/4/20.
//

#ifndef OS_LAB3_AGING_H
#define OS_LAB3_AGING_H

#include <vector>
#include "Pager.h"
#include "PTE.h"
class Aging : public Pager
{
private:
    int num_phys;
    int num_virt;
    std::vector<unsigned int> counter;
public:
    Aging(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table, int _num_phys, int _num_virt) :
            Pager(_type, _page_table, _frame_table)
    {
        num_phys = _num_phys;
        num_virt = _num_virt;
        //initialize all counters to 0;
        if(type==7)
        {
            for(int i=0; i<num_phys; i++)
            {
                counter.push_back(0);
            }
        }
        else
        {
            for(int i=0; i<num_virt; i++)
            {
                counter.push_back(0);
            }
        }

    }
    ~Aging() {}
    int get_frame()
    {
        int phys_idx = 0;
        int virt_idx = 0;
        if(type==7) //phys:
        {
            renew_phys_counter();
            unsigned int min = counter.front();
            for(int i=0; i<num_phys; i++)
            {
                if(counter[i]<min)
                {
                    phys_idx = i;
                    min = counter[i];
                }
                // remember ref only valid in one pass!
                // so zero ref in each pass is necessary
                virt_idx = (*frame_table)[i];
                (*page_table)[virt_idx].refered = 0;
            }
            // this phys_idx would be replaced by a new one, so initialize
            // with zero
            counter[phys_idx] = 0;
        }
        else //virt:
        {
            renew_virt_counter();
            unsigned int min = 0;
            //first find the first possible candidate;
            for(int i=0; i<num_virt; i++)
            {
                if((*page_table)[i].present)
                {
                    virt_idx = i;
                    min = counter[i];
                    (*page_table)[i].refered = 0;
                    break;
                }
            }

            for(int i=virt_idx+1; i<num_virt; i++)
            {
                if((*page_table)[i].present)
                {
                    if(counter[i]<min)
                    {
                        virt_idx = i;
                        min = counter[i];
                    }
                    (*page_table)[i].refered = 0;
                }
            }
            counter[virt_idx] = 0;
            phys_idx = (*page_table)[virt_idx].phys_idx;
        }
        return phys_idx;
    }

    void renew_phys_counter()
    {
        for(int i=0; i<num_phys; i++)
        {
            int phys_idx = (*frame_table)[i];
            unsigned int r = (*page_table)[phys_idx].refered;
            counter[i] = counter[i] >> 1;
            // we want to add r to the leftmost of counter
            r = r << 31;
            counter[i] =  r | counter[i];
        }
    }

    void renew_virt_counter()
    {
        for(int i=0; i<num_virt; i++)
        {
            if((*page_table)[i].present)
            {
                unsigned int r = (*page_table)[i].refered;
                counter[i] = counter[i] >> 1;
                r = r << 31;
                counter[i] =  r | counter[i];
            }
        }
    }
};

#endif //OS_LAB3_AGING_H
