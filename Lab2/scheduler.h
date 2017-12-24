//
// Created by Lei Guo on 2017/3/16.
//

#ifndef OS_LAB2_SCHEDULER_H
#define OS_LAB2_SCHEDULER_H
#include <fstream>
#include <iostream>
#include <list>
//for some occasions, queue is not accurate;
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include "event.h"
#include "process.h"

//give each child class an id of its type:
class Scheduler
{
protected:
    int quantum; // for RR or PRIO
    std::list<Process*> run_que;
    char* infile;
    char* randfile;
    virtual void add_proc(Process *) = 0; // virtual function for different scheduler to implement
    //due to deterministic, one single process is returned!
    virtual Process * get_proc() = 0; // virtual function for differnt scheduler to provide the next ready process
    virtual bool run_que_isEmpty() = 0;
    virtual void print_type() = 0;
private:
    int vflag;
    // for reading the rand file:
    int* rand_nums; // pointer to an int array;
    int rand_count;
    int rand_ofs;
    int myrandom(int burst);
	// this two lines are for determining the IO time!
	int pre_io; // time when the pre process finished io;
	int no_io; // total time when there is completely no process performing IO;
    Process* runng_proc;
    std::list<Process*> proc_list;//initially store all processes;
	std::list<Event*> event_que;
	std::list<Event*> event_list; // to store all the events during the simulation
    void read_infile();
    void read_randfile();
	void report();
	void print_verbose();
    void add_event(Event* event);//independent from the specific type of scheduler!
    //Event* get_event();//still independent from any specific type of scheduler!
public:
    Scheduler(char* _infile, char* _randfile, int _quantum, int _vflag);
    virtual ~Scheduler() = 0;
	void simulate();
};
#endif // OS_LAB2_SCHEDULER_H
