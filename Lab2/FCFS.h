//
// Created by Lei Guo on 2017/3/16.
//

#ifndef OS_LAB2_FCFS_H
#define OS_LAB2_FCFS_H
#include <iostream>
#include <fstream>
#include <limits>
#include "scheduler.h"
#include "process.h"
using namespace std;

class FCFS: public Scheduler
{
public:
    FCFS(char* _infile, char* _randfile, int _quantum, int _vflag) :
            Scheduler(_infile, _randfile, _quantum, _vflag) {/*cout<<"I'm in FCFS!"<<endl;*/}

    ~FCFS() {}

protected:
    void add_proc(Process* proc) // add to run queue
    {
        run_que.push_back(proc);
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
        cout<<"FCFS"<<endl;
    }
};
#endif //OS_LAB2_FCFS_H
