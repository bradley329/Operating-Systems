//
// Created by Lei Guo on 2017/3/23.
//

#ifndef OS_LAB2_SJF_H
#define OS_LAB2_SJF_H
#include <iostream>
#include <fstream>
#include <limits>
#include <queue>
#include <iomanip>
#include "scheduler.h"
#include "process.h"
using namespace std;
//如果想要用PQ 别忘了各个scheduler需要自己的runque了！！！
/*
 * if the rem_exe_time equals, then PQ would not keep the order of insertions...using position might work!
class CompareRem {
public:
    bool operator()(const  Process* p1, const Process* p2)
    {
        if (p1->rem_exe_time > p2->rem_exe_time) return true;
        return false;
    }
};*/
class SJF: public Scheduler {

public:
    // priority_queue<Process*, vector<Process*>, CompareRem> run_que;
    SJF(char* _infile, char* _randfile, int _quantum, int _vflag) :
            Scheduler(_infile, _randfile, _quantum, _vflag) {}

    ~SJF() {}

protected:
    void add_proc(Process* proc) //add to run_que
    {
        // order of run_que is based on rem_exe_time
        if(!run_que.empty())
        {
            list<Process*>::iterator it = run_que.begin();
            for(;it != run_que.end(); it++)
            {
                if((*it)->rem_exe_time > proc->rem_exe_time) break;
            }
            //note that when ==, it++, then insert
            run_que.insert(it, proc);
        }
        else
        {
            run_que.push_back(proc);
        }
    }
    Process* get_proc()
    {
        Process * proc = run_que.front();
        run_que.pop_front();
        return proc;
    }

    bool run_que_isEmpty()
    {
        return run_que.empty();
    }

    void print_type()
    {
        cout<< "SJF"<<endl;
    }
};
#endif //OS_LAB2_SJF_H
