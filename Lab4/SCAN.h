//
// Created by Lei Guo on 2017/4/26.
//

#ifndef LAB4_SCAN_H
#define LAB4_SCAN_H

#include <list>
#include "Scheduler.h"
class SCAN: public Scheduler
{
private:
    std::list<Event*> dyna_list;
    int direc; //1 means up, 0 means down
public:
    SCAN() {direc = 1;}
    ~SCAN() {}
    void add_event(Event* event)
    {
        dyna_list.push_back(event);
    }

    Event* get_event(int head)
    {
        Event *res;
        int min_move = 0;
        std::list<Event *>::iterator it = dyna_list.begin();
        std::list<Event *>::iterator min_it;
        int found = 0;
        for(; it != dyna_list.end(); it++)
        {
            if(direc==1 && (*it)->track_num >= head)
            {
                // get the first event whose track num is larger or "equal"!!! to head;
                found = 1;
                min_move = (*it)->track_num - head;
                min_it = it;
                break;
            }
            else if(direc==0 && (*it)->track_num <= head)
            {
                // get the first event whose track num is smaller or "equal"!!! to head;
                found = 1;
                min_move = head - (*it)->track_num;
                min_it = it;
                break;
            }
        }
        if(found == 0)
        {
            if(direc==1) direc = 0;
            else direc = 1;
            return get_event(head);
        }
        else
        {
            int tp = 0;
            for(; it != dyna_list.end(); it++)
            {
                if(direc == 1 && (*it)->track_num >= head)
                {
                    // get the first event whose track num is larger or "equal"!!! to head;
                    tp = (*it)->track_num - head;
                    if(tp < min_move)
                    {
                        min_move = tp;
                        min_it = it;
                    }
                }
                else if(direc == 0 && (*it)->track_num <= head)
                {
                    tp = head - (*it)->track_num;
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
#endif //LAB4_SCAN_H
