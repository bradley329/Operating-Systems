//
// Created by Lei Guo on 2017/3/19.
//

#ifndef OS_LAB2_PRIO_H
#define OS_LAB2_PRIO_H
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <list>
#include "scheduler.h"
#include "process.h"

class PRIO: public Scheduler
{
private:
    // PRIO has it's own
    // since there are totally 4 dynamic priorities
    std::vector< std::list<Process*> > active_que;
    std::vector< std::list<Process*> > expired_que;
    int active_size;
    int expired_size;
    // what if the DYNA_PRIO reaches -1?
public:
    PRIO(char* _infile, char* _randfile, int _quantum, int _vflag);
    ~PRIO() {}
protected:
    void add_proc(Process *proc);
    // if get_proc is invoked, it means that it never empty
    Process* get_proc();
    bool run_que_isEmpty();
    void print_type();
};
#endif //OS_LAB2_PRIO_H
