//
// Created by Lei Guo on 2017/4/19.
//

#ifndef OS_LAB3_NRU_H
#define OS_LAB3_NRU_H

#include "Pager.h"
#include "PTE.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

class NRU : public Pager
{
private:
    char* randfile;
    int* rand_nums; // pointer to an int array;
    int rand_count;
    int rand_ofs;
    int num_virt;
    void read_randfile()
    {
        std::ifstream rand_file(randfile);
        if(rand_file.good())
        {
            std::string str;
            // get from the first line the count
            getline(rand_file, str);
            //cout<<str<<endl;
            rand_count = atoi(str.c_str());
            rand_nums = new int[rand_count];
            int ix = 0;
            while(getline(rand_file, str))
            {
                rand_nums[ix] = atoi(str.c_str());
                ix++;
            }
        }
        rand_file.close();
    }

    int myrandom(int burst)
    {
        int rand = rand_nums[rand_ofs] % burst;
        rand_ofs++;
        if(rand_ofs >= rand_count) rand_ofs = 0;
        return rand;
    }

    void renew_classes()
    {
        for(int i=0; i<num_virt; i++)
        {
            if((*page_table)[i].present)
            {
                int R = (int) (*page_table)[i].refered;
                int M = (int) (*page_table)[i].modified;
                if((R==1) && (M==1))
                    class3.push_back(i);
                else if((R==1) && (M==0))
                    class2.push_back(i);
                else if((R==0) && (M==1))
                    class1.push_back(i);
                else
                    class0.push_back(i);
            }
        }
    }
    // when timer reached clock;
    void clear_bit()
    {
        for(int i = 0; i < num_virt; i++)
        {
            if((*page_table)[i].present)
                (*page_table)[i].refered = 0;
        }
    }
    int clock;
    int timer;
    std::vector<int> class0;
    std::vector<int> class1;
    std::vector<int> class2;
    std::vector<int> class3;

public:
    NRU(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table, char* _randfile, int _num_virt) :
            Pager(_type, _page_table, _frame_table)
    {
        randfile = _randfile;
        rand_nums = NULL;
        rand_count = 0;
        rand_ofs = 0;
        read_randfile();
        clock = 10;
        timer = 0;
        num_virt = _num_virt;
    }
    ~NRU() {}
    int get_frame()
    {
        int phys_idx = 0;
        int virt_idx = 0;
        class0.clear();
        class1.clear();
        class2.clear();
        class3.clear();
        renew_classes();
        timer++;
        // when timer reach to clock, clear referenced bits
        if(timer == clock)
        {
            clear_bit();
            timer = 0;
        }

        if(!class0.empty())
        {
            int index = myrandom((int) class0.size());
            virt_idx = class0[index];
        }
        else if(!class1.empty())
        {
            int index = myrandom((int) class1.size());
            virt_idx = class1[index];
        }
        else if(!class2.empty())
        {
            int index = myrandom((int) class2.size());
            virt_idx = class2[index];
        }
        else
        {
            int index = myrandom((int) class3.size());
            virt_idx = class3[index];
        }
        phys_idx = (*page_table)[virt_idx].phys_idx;
        return phys_idx;
    }
};
#endif //OS_LAB3_NRU_H
