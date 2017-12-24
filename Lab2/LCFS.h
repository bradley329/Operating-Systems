//
// Created by Lei Guo on 2017/3/19.
//

#ifndef OS_LAB2_LCFS_H
#define OS_LAB2_LCFS_H
#include <iostream>
#include <fstream>
#include <limits>
#include "scheduler.h"
#include "process.h"
using namespace std;

class LCFS: public Scheduler {

public:
    LCFS(char* _infile, char* _randfile, int _quantum, int _vflag) :
            Scheduler(_infile, _randfile, _quantum, _vflag) {}

    ~LCFS() {}

protected:
    void add_proc(Process* proc) // ready add to run
    {
        run_que.push_back(proc);
    }
    //LCFS so the run_que is just like a stack!
    Process* get_proc()
    {
        // let run que serve as a stack
        Process * proc = run_que.back();
        run_que.pop_back();
        return proc;
    }

    bool run_que_isEmpty()
    {
        return run_que.empty();
    }

    void print_type()
    {
        cout<<"LCFS"<<endl;
    }
};
#endif //OS_LAB2_LCFS_H
