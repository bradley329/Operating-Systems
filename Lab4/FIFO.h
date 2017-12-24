//
// Created by Lei Guo on 2017/4/25.
//

#ifndef LAB4_FIFO_H
#define LAB4_FIFO_H

#include <list>
#include "Scheduler.h"
class FIFO: public Scheduler
{
private:
    std::list<Event*> dyna_list;
public:
    FIFO() {}
    ~FIFO() {}
    void add_event(Event* event)
    {
        dyna_list.push_back(event);
    }
    Event* get_event(int head)
    {
        Event* res = dyna_list.front();
        dyna_list.pop_front();
        return res;
    }
    bool isempty()
    {
        return dyna_list.empty();
    }
};
#endif //LAB4_FIFO_H
