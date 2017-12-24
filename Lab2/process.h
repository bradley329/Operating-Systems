//
// Created by Lei Guo on 2017/3/16.
//

#ifndef OS_LAB2_PROCESS_H
#define OS_LAB2_PROCESS_H

class Process
{
public:
    int id; // process id;
    int AT; // arrival time;
    int TC; // total time required;
    int CB; // max CPU burst;
    int rand_cb; //random CB time;
    int IO;//max IO burst;
    int rand_io; //random IO time;
    int STAT_PRIO;
    int DYNA_PRIO; // only has meaning when PRIO;
    int FT; // Finishing time
    int TT; // Turnaround time (finishing time - AT)
    int IT; // I/O Time (time in blocked state)
    int CW; // CPU Waiting time (time in Ready state)
    int rem_exe_time; // remaining execution time;
    // for computing the cpu_wait
    int ready_time;
    int preempted;

    //Constructor:
    Process(int _id, int _AT, int _TC, int _CB, int _IO)
    {
        id = _id;
        AT = _AT;
        TC = _TC;
        CB = _CB;
        IO = _IO;
        rand_cb = 0;
        rand_io = 0;
        STAT_PRIO = 0;
        DYNA_PRIO = 0;
        FT = 0;
        TT = 0;
        IT = 0;
        CW = 0;
        rem_exe_time = _TC;
        ready_time = 0;
        preempted = 0;
    }
};

#endif // OS_LAB2_PROCESS_H
