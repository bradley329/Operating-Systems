//
// Created by Lei Guo on 2017/3/16.
//

#ifndef OS_LAB2_EVENT_H
#define OS_LAB2_EVENT_H
#include "process.h"

//just for reminding:
/*
enum Trans
{
    Create_to_Ready = 1, Ready_to_Run, Run_to_Block, Block_to_Ready, Run_to_Ready, Finish
};*/

class Event
{
public:
    int time_stamp;//current(actual) time;
    int span;//how long has been in current state?
    int trans;
    Process* proc;
    /* follow the hint
     * these lines are for verbose output:
     * why we bother copying them given that we had proc?
     * since Process*, but we want to record the event info and keep it static!
    */
    int rand_cb; //if trans to run, how long would the cpu burst be?
    int rand_io; //if trans to block, how long would the io burst be?
    int rem_exe_time;
    int DYNA_PRIO;

    Event(int _time_stamp, int _span, Process* _proc, int _trans);
    void verbose();//the verbose output for debugging.
};

#endif // OS_LAB2_EVENT_H
