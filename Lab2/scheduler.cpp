//
// Created by Lei Guo on 2017/3/16.
//

#include "scheduler.h"
using namespace std;

// prio should always equals to static-1 if not in PRIO;
Scheduler::Scheduler(char* _infile, char* _randfile, int _quantum, int _vflag)
{
    randfile = _randfile;
    infile = _infile;
    //cout<<"infile "<<infile<<endl;
    //cout<<"randfile:" <<randfile<<endl;
    //cout<<"I'm in sche"<<endl;
    vflag = _vflag;
    rand_count = 0;
    rand_ofs = 0;
    //rand_nums = NULL;
    pre_io = 0;
    no_io = 0;
    quantum = _quantum; // quantum should be INT_MAX when it does not make sense!
    runng_proc = NULL;
    read_randfile();
    //cout<<"reading infile..."<<endl;
    read_infile(); // must read both files at construction
    //cout<<"reading infile succeed"<<endl;
}

Scheduler::~Scheduler() {}

void Scheduler::read_randfile()
{
    ifstream rand_file(randfile);
    if(rand_file.good())
    {
        string str;
        // get from the first line the count
        getline(rand_file, str);
        //cout<<str<<endl;
        rand_count = atoi(str.c_str());
        rand_nums = new int[rand_count];
        int ix = 0;
        while(getline(rand_file, str))
        {
            rand_nums[ix] = atoi(str.c_str());
            ix++;
        }
    }
    rand_file.close();
    //cout<<"read randfile suc"<<endl;
}

int Scheduler::myrandom(int burst)
{
    int rand = 1 + (rand_nums[rand_ofs] % burst);
    rand_ofs++;
    if(rand_ofs >= rand_count) rand_ofs = 0;
    return rand;
}

void Scheduler::read_infile()
{
    ifstream in_file;
    in_file.open(infile);
    //cout<<"ready to read: "<<infile<<endl;
    stringstream parser;
    string str;
    int _AT;
    int _TC;
    int _CB;
    int _IO;
    //if(in_file.is_open())
    //{
        //cout<<"infile good"<<endl;
        int id = 0;
        while(getline(in_file, str))
        {
            //cout<<"getline chenggong"<<endl;
            //cout<<str;
            //cout<<"str :"<<endl;
            // clear parser
            parser.str(string());
            parser.clear();
            //cout<<"parser clear cheng gong"<<endl;
            parser << str;
            parser >> _AT;
            parser >> _TC;
            parser >> _CB;
            parser >> _IO;
            //cout<<"parser first chenggong"<<endl;
            Process* proc = new Process(id, _AT, _TC, _CB, _IO);
            //cout<<"process"<<id<<"created"<<endl;
            proc->STAT_PRIO = myrandom(4);
            //cout<<"stat prio:"<<proc->STAT_PRIO<<endl;
            proc->DYNA_PRIO = proc->STAT_PRIO - 1;
            //cout<<"dyna prio:"<<proc->DYNA_PRIO<<endl;
            proc_list.push_back(proc);
            //cout<<"ready to create event"<<endl;
            Event* event = new Event(proc->AT, 0, proc, 1); // trans is Create_to_Ready
            //cout<<"event create"<<endl;
            add_event(event);
            id++;
            //cout<<"parser first end"<<endl;
        }
    //}
    in_file.close();
    //cout<<"read infile suc"<<endl;
}

// add event to event_que at the right position:
void Scheduler::add_event(Event* event)
{
    // >= not >
    list<Event*>::iterator it = event_que.begin();
    for (; it != event_que.end(); it++)
    {
        if(event->time_stamp < (*it)->time_stamp) break;
    }
    event_que.insert(it, event);
    //cout<<"event add at: "<<event->time_stamp<<endl;
}

void Scheduler::simulate()
{
    //cout<<"event que size: "<<event_que.size()<<endl;
    while(!event_que.empty())
    {
        //cout<<"event_que not empty"<<endl;
        int pre_time = event_que.front()->time_stamp;
        int cur_time = pre_time;
        //cout<<"cur time: "<<cur_time<<endl;
        // note all Ready_to_Run events are created in the dispatcher!
        // so during switch, we only add the process calling add_proc
        // but we did not call add_event() to which is Ready_to_Run!
        while(cur_time == pre_time)
        {
            //cout<<"准备拿event。。。"<<endl;
            Event* cur_event = event_que.front();
            event_que.pop_front();
            //cout<<"deque succeed"<<endl;
            // add to event_list
            event_list.push_back(cur_event);
            Process* cur_proc = cur_event->proc; // not runng_proc!
            int trans = cur_event->trans;
            //cout<<"event trans: "<<cur_event->trans<<endl;
            switch(trans) {
                case 1: // Create_to_Ready
                {
                    //time_stamp = ready_time = AT;
                    //cout<<"ready time1 :"<<cur_time<<endl;
                    cur_proc->ready_time = cur_time;
                    //cout<<"ready time added :"<<cur_time<<endl;
                    add_proc(cur_proc);
                    //cout<<"add proc chenggong"<<endl;
                    break;
                }
                case 2: // Ready_to_Run
                {
                    // cur_proc start running!
                    runng_proc = cur_proc;
                    cur_proc->CW += cur_event->span;
                    int rand_cb = cur_proc->rand_cb;
                    Event *event;
                    // for PRIO and RR:
                    int trans = 0;
                    if (rand_cb > quantum) {
                        // quantum would be expired, so runng_proc would be preempted
                        int time_stamp = cur_time + quantum;
                        /* note the sequence of every event
                         * termination > block > preempt
                         * so the rand_cb could only be quantum
                         * but we should handle termination first!
                         */
                        if (cur_proc->rem_exe_time <= quantum) {
                            trans = 6; // Finish
                        } else {
                            trans = 5;
                        }
                        // DYNA_PRIO-- after a rand_cb or only when rand_cb exceeds quantum?
                        // did not handle when DYNA_PRIO reaches -1
                        event = new Event(time_stamp, quantum, cur_proc, trans); // Run_to_Ready
                    } else {
                        int time_stamp = cur_time + rand_cb;
                        // still we need to consider termination first!
                        if (cur_proc->rem_exe_time <= rand_cb) {
                            trans = 6; // Finish
                        } else {
                            trans = 3;
                            // rand_io should equals to
                            int rand_io = myrandom(cur_proc->IO);
                            cur_proc->rand_io = rand_io;
                        }
                        event = new Event(time_stamp, rand_cb, cur_proc, trans); // Run_to_Block
                    }
                    add_event(event);
                    break;
                }
                case 3: // Run_to_Block
                {
                    // first get the running time for the current process
                    int run_time = cur_event->span;
                    // since there is only one proc running, so runng_proc is actually cur_proc!
                    // runng_proc->rem_exe_time -= run_time; // no need
                    cur_proc->rem_exe_time -= run_time;
                    // cur_event->rem_exe_time = cur_proc->rem_exe_time;
                    int rand_io = cur_proc->rand_io;
                    cur_proc->IT += rand_io; // add rand_io to IO_time;
                    int ready_time = cur_time + rand_io;
                    // next state is ready, so we compute the ready_time for cur_proc
                    // cur_proc->ready_time = ready_time;
                    // the next event is Block_to_Ready
                    Event *event = new Event(ready_time, rand_io, cur_proc, 4); //Block_to_Ready
                    // very important, had bug in here!
                    runng_proc = NULL;
                    add_event(event);

                    // IO_time is when there is at least "one" proc performing IO;
                    if (cur_time > pre_io) {
                        int non_io_span = cur_time - pre_io;
                        no_io += non_io_span;
                    }
                    int cur_io = cur_time + rand_io;
                    if (cur_io > pre_io) {
                        pre_io = cur_io;
                    }
                    break;
                }
                case 4://Block_to_Ready
                {
                    // if return from block, DYNA_PRIO = STAT_PRIO - 1;
                    cur_proc->DYNA_PRIO = cur_proc->STAT_PRIO - 1;
                    cur_proc->ready_time = cur_time;
                    add_proc(cur_proc);
                    break;
                }
                case 5://Run_to_Ready(Preempt)
                {
                    cur_proc->DYNA_PRIO--;
                    cur_proc->preempted = 1;
                    // should handle DYNA==-1
                    // cur_event->DYNA_PRIO = cur_proc->DYNA_PRIO;
                    // runng_proc is actually cur_proc!
                    // we are now sure that it has been preempted
                    // next time when processing it again,
                    // rand_cb should equals to rand_cb - quantum
                    cur_proc->rand_cb = cur_proc->rand_cb - quantum;
                    //cout<<"1"<<endl;
                    cur_proc->rem_exe_time = cur_proc->rem_exe_time - quantum;
                    //cout<<"2"<<endl;
                    cur_proc->ready_time = cur_time;
                    //cout<<"3"<<endl;
                    add_proc(cur_proc);
                    //cout<<"4"<<endl;
                    // still very important
                    runng_proc = NULL;
                    //cout<<"preempt ends"<<endl;
                    break;
                }
                case 6://Finish
                {
                    cur_proc->FT = cur_time;
                    cur_proc->TT = cur_proc->FT - cur_proc->AT;
                    runng_proc = NULL;
                    break;
                }
                default:
                    break;
            }
            if(event_que.empty()) break;
            cur_time = event_que.front()->time_stamp;
        }
        //the CPU can only run one proc at a time!
        if (runng_proc == NULL && !run_que_isEmpty())
        {
            Process* next_proc = get_proc();
            //cout<<"get process chenggong"<<endl;
            //this new runng_proc is the one that has been preempted, or not?
            if (next_proc->preempted == 0)
            {
                int rand_cb = myrandom(next_proc->CB);
                if (rand_cb > next_proc->rem_exe_time)
                {
                    next_proc->rand_cb = next_proc->rem_exe_time;
                }
                else
                {
                    next_proc->rand_cb = rand_cb;
                }
            }
            else
            {
                if (next_proc->rand_cb > next_proc->rem_exe_time)
                {
                    next_proc->rand_cb = next_proc->rem_exe_time;
                }
                next_proc->preempted = 0;
            }
            // if rand_cb != 0, means it might be preempted
            //note we use pre_time
            Event* event = new Event(pre_time, pre_time - next_proc->ready_time, next_proc, 2);
            add_event(event);
            //cout<<"add event chenggong"<<endl;
        }
    }
    report();
    if(vflag==1) print_verbose();
}

// be careful!
void Scheduler::print_verbose()
{
    for (list<Event*>::iterator it = event_list.begin(); it != event_list.end(); it++)
    {
        Event* event = *it;
        event->verbose();
    }
}

void Scheduler::report()
{
    print_type();
    int FT = 0;
    for (list<Process*>::iterator it = proc_list.begin(); it != proc_list.end(); it++) {
        Process * proc = *it;
        proc->TT = proc->FT - proc->AT;
        if (proc->FT > FT) {
            FT = proc->FT;
        }
    }

    int total_CT = 0;
    int total_TT = 0;
    int total_CW = 0;
    // had bug in here
    no_io += FT - pre_io;
    int total_IO = FT - no_io;

    for (list<Process*>::iterator it = proc_list.begin(); it != proc_list.end(); it++)
    {
        Process * proc = *it;
        total_CT += proc->TC;
        total_TT += proc->TT;
        total_CW += proc->CW;

        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", proc->id,
               proc->AT, proc->TC, proc->CB, proc->IO, proc->STAT_PRIO, proc->FT,
               proc->TT, proc->IT, proc->CW);
    }
    double count = proc_list.size();
    double CPU_UTIL = (double) total_CT / FT * 100;
    double IO_UTIL = (double) total_IO / FT * 100;
    double AVER_TT = (double) total_TT / count;
    double AVER_CW = (double) total_CW / count;
    double THR_PUT = count * 100 / FT;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           FT, CPU_UTIL, IO_UTIL, AVER_TT, AVER_CW, THR_PUT);
}