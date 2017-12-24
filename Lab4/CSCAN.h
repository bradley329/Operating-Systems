//
// Created by Lei Guo on 2017/4/26.
//

#ifndef LAB4_CSCAN_H
#define LAB4_CSCAN_H
#include <list>
#include "Scheduler.h"
// C means circular, always move to the upper direction
class CSCAN: public Scheduler
{
private:
    std::list<Event*> dyna_list;
    int back_to_start; // whether should back to head 0
    int cur_head;
public:
    CSCAN()
    {
        back_to_start = 0;
        cur_head = 0;
    }
    ~CSCAN() {}
    void add_event(Event* event)
    {
        dyna_list.push_back(event);
    }

    Event* get_event(int head)
    {
        Event *res;
        int min_move = 0;
        std::list<Event *>::iterator min_it;
        std::list<Event *>::iterator it = dyna_list.begin();
        // normally, cur_head = head
        cur_head = head;
        if(back_to_start==1)
            cur_head = 0;

        int found = 0;
        for(; it != dyna_list.end(); it++)
        {
            //always looks up!
            if((*it)->track_num >= cur_head)
            {
                // get the first event whose track num is larger or "equal"!!! to head;
                found = 1;
                min_move = (*it)->track_num - cur_head;
                min_it = it;
                break;
            }
        }
        if(found == 0)
        {
            back_to_start = 1;
            return get_event(head);
        }
        else
        {
            back_to_start = 0;
            int tp = 0;
            for(; it != dyna_list.end(); it++)
            {
                if((*it)->track_num >= cur_head)
                {
                    // get the first event whose track num is larger or "equal"!!! to head;
                    tp = (*it)->track_num - cur_head;
                    if(tp < min_move)
                    {
                        min_move = tp;
                        min_it = it;
                    }
                }
            }
            res = (*min_it);
            dyna_list.erase(min_it);
            return res;
        }
    }
    bool isempty()
    {
        return dyna_list.empty();
    }
};
#endif //LAB4_CSCAN_H
