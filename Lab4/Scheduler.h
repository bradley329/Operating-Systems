//
// Created by Lei Guo on 2017/4/25.
//

#ifndef LAB4_SCHEDULER_H
#define LAB4_SCHEDULER_H
#include <vector>
#include "Event.h"
class Scheduler
{
public:
    Scheduler() {}
    virtual ~Scheduler() {}
    virtual void add_event(Event* event) = 0;
    virtual Event* get_event(int head) = 0;
    virtual bool isempty() = 0;
};
#endif //LAB4_SCHEDULER_H
