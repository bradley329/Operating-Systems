//
// Created by Lei Guo on 2017/4/25.
//
#ifndef LAB4_SSTF_H
#define LAB4_SSTF_H
#include <stdlib.h>
#include "Scheduler.h"
#include <list>
class SSTF: public Scheduler
{
private:
    std::list<Event*> dyna_list;
public:
    SSTF() {}
    ~SSTF() {}
    void add_event(Event* event)
    {
        dyna_list.push_back(event);
    }
    Event* get_event(int head)
    {
        std::list<Event*>::iterator it = dyna_list.begin();
        int min_move = abs((*it)->track_num - head);
        std::list<Event*>::iterator min_it = it;
        it++;
        int tp = 0;
        for(; it!=dyna_list.end(); it++)
        {
            tp = abs((*it)->track_num - head);
            if(tp < min_move)
            {
                min_move = tp;
                min_it = it;
            }
        }
        Event* res = (*min_it);
        dyna_list.erase(min_it);
        return res;
    }
    bool isempty()
    {
        return dyna_list.empty();
    }
};
#endif //LAB4_SSTF_H
