//
// Created by Lei Guo on 2017/4/14.
//

#ifndef OS_LAB3_PAGER_H
#define OS_LAB3_PAGER_H

#include <list>
#include <vector>

//just for reminding:
/*
enum type
{
    Come = 1, Map, Unmap, In, Out, Zero
};*/

class Pager
{
protected:
    std::vector<PTE>* page_table;
    std::vector<int>* frame_table;
/*
private:
    void read_randfile();
    int myrandom(int burst);
*/
public:
    int type;
    std::vector<int> valid_list; // this is the frame table but ordered in time;
    // which are present physical frames; For FIFO, SecondChance and Clock algorithm
    Pager(int _type, std::vector<PTE>* _page_table, std::vector<int>* _frame_table):
            type(_type), page_table(_page_table), frame_table(_frame_table)
    {}

    virtual ~Pager() {}
    virtual int get_frame() = 0;
};
#endif //OS_LAB3_PAGER_H
