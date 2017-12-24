//
// Created by Lei Guo on 2017/3/16.
//

#include "event.h"
#include <iostream>
using namespace std;

Event::Event(int _time_stamp, int _span, Process* _proc, int _trans)
{
    time_stamp = _time_stamp;
    span = _span;
    proc = _proc;
    rand_cb = _proc->rand_cb;
    rand_io = _proc->rand_io;
    rem_exe_time = _proc->rem_exe_time;
    DYNA_PRIO = _proc->DYNA_PRIO;
    trans = _trans;
}

void Event::verbose()
{
    switch(trans)
    {
    case 1://Create_to_Ready
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": CREATED -> READY"<<endl;
			break;
        }
    case 2://Ready_to_Run
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": READY -> RUNNG ";
            cout<<"cb="<<rand_cb<<" rem="<<rem_exe_time<<" prio="<<DYNA_PRIO<<endl;
            break;
        }
    case 3://Run_to_Block
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": RUNNG -> BLOCK ";
            cout<<"ib="<<rand_io<<" rem="<<rem_exe_time<<endl;
            break;
        }
    case 4://Block_to_Ready
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": BLOCK -> READY"<<endl;
            break;
        }
    case 5://Run_to_Ready(Preempt)
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": RUNNG -> READY ";
            cout<<"cb="<<rand_cb<<" rem="<<rem_exe_time<<" prio="<<DYNA_PRIO<<endl;
            break;
        }
    case 6://Finish
        {
            cout<<time_stamp<<" "<<proc->id<<" "<<span<<": RUNNG -> READY"<<endl;
            break;
        }
    default:
        break;
    }
}
