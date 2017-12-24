//
// Created by Lei Guo on 2017/4/26.
//

#ifndef LAB4_FSCAN_H
#define LAB4_FSCAN_H
#include <list>
#include "Scheduler.h"
// like scan, but has two queues
class FSCAN: public Scheduler
{
private:
    std::list<Event*>* now_queue;
    std::list<Event*>* then_queue;
    int direc; //1 means up, 0 means down
public:
    FSCAN()
    {
        now_queue = new std::list<Event*>();
        then_queue = new std::list<Event*>();
        direc = 1;
    }
    ~FSCAN() {}
    void add_event(Event* event)
    {
        then_queue->push_back(event);
    }

    Event* get_event(int head)
    {
        //std::cout<<"如果下句出不来就是now_queue->size非法"<<std::endl;
        if(now_queue->size()==0)
        {
            //std::cout<<"now_queue->size合法"<<std::endl;
            // switch two queues
            std::list<Event*>* tp = now_queue;
            now_queue = then_queue;
            then_queue = tp;
            direc = 1;
        }
        Event *res;
        int min_move = 0;
        std::list<Event *>::iterator min_it;
        std::list<Event *>::iterator it = now_queue->begin();
        int found = 0;
        for(; it != now_queue->end(); it++)
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
                min_move =  head - (*it)->track_num;
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
            for(; it != now_queue->end(); it++)
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
            now_queue->erase(min_it);
            return res;
        }
    }
    bool isempty()
    {
        return now_queue->empty() && then_queue->empty();
    }
};
#endif //LAB4_FSCAN_H
