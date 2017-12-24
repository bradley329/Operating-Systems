//
// Created by Lei Guo on 2017/3/19.
//
#include "PRIO.h"
using namespace std;

PRIO::PRIO(char* _infile, char* _randfile, int _quantum, int _vflag) :
        Scheduler(_infile, _randfile, _quantum, _vflag)
{
    //cout<<"I'm in PRIO!"<<endl;
    active_size = 0;
    expired_size = 0;
    //cout<<"PRIO start construct!"<<endl;
    for(int i=0; i<4; i++)
    {
        list<Process*> empty;
        active_que.push_back(empty);
        expired_que.push_back(empty);
    }
    //cout<<"PRIO construct ends"<<endl;
}

void PRIO::add_proc(Process* proc) // add to active_que or expired_que
{
    // create to ready: proc->STAT_PRIO - 1; Create->Ready takes care of it!
    // if return from IO: proc->STAT_PRIO - 1; IO->Ready trans takes care of it!
    // Preempt and reaches -1, proc->STAT_PRIO - 1, go to expired
    //cout<<"adding proc"<<endl;
    if(proc->DYNA_PRIO <= -1)
    {
        proc->DYNA_PRIO = proc->STAT_PRIO - 1;
        // add to expired:
        expired_que[proc->DYNA_PRIO].push_back(proc);
        expired_size++;
    }
    else
    {
        // proc->DYNA_PRIO = proc->DYNA_PRIO - 1;
        // add to active:
        active_que[proc->DYNA_PRIO].push_back(proc);
        active_size++;
    }
}

Process* PRIO::get_proc()
{
    if (active_size <= 0)
    {
        int temp = active_size;
        active_size = expired_size;
        expired_size = temp;
        active_que.swap(expired_que);
    }
    Process* proc;
    for (int i=3; i>=0; i--)
    {
        //list is a pointer
        list<Process*>* cur_prio = &active_que[i];
        if(cur_prio->empty())
        {
            continue;
        }
        else
        {
            proc = cur_prio->front();
            cur_prio->pop_front();
            break;
        }
    }
    active_size--;
    return proc;
}

bool PRIO::run_que_isEmpty()
{
    return (active_size + expired_size) == 0;
}

void PRIO::print_type()
{
    cout<<"PRIO "<<quantum<<endl;
}