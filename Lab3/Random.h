//
// Created by Lei Guo on 2017/4/19.
//

#ifndef OS_LAB3_RANDOM_H
#define OS_LAB3_RANDOM_H
#include "Pager.h"
#include "PTE.h"
#include <fstream>
#include <iostream>

class Random : public Pager
{
private:
    char* randfile;
    int* rand_nums; // pointer to an int array;
    int rand_count;
    int rand_ofs;
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
    int num_phys;

public:
    Random(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table, int _num_phys, char* _randfile) :
            Pager(_type, _page_table, _frame_table)
    {
        randfile = _randfile;
        rand_nums = NULL;
        rand_count = 0;
        rand_ofs = 0;
        num_phys = _num_phys;
        read_randfile();
    }
    ~Random() {}
    int get_frame()
    {
        return myrandom(num_phys);
    }
};
#endif //OS_LAB3_RANDOM_H
