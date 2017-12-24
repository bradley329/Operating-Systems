#include <iostream>
#include <list>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "Scheduler.h"
#include "FIFO.h"
#include "SSTF.h"
#include "Event.h"
#include "SCAN.h"
#include "CSCAN.h"
#include "FSCAN.h"
using namespace std;

// Global variable:
list<Event*> static_list;  //all the operation list, for output.

//=========read input file========
void read_infile(char* infile_name)
{
    ifstream infile;
    string str;
    stringstream ss;
    int id = 0; //instruction id;
    int arr_time = 0;
    int track_num = 0;
    infile.open(infile_name);
    if(infile.good())
    {
        while(getline(infile, str))
        {
            if(str.at(0) == '#')
                continue;
            ss.str(str);
            ss>>arr_time>>track_num;
            Event* event = new Event(id, arr_time, track_num, arr_time, arr_time);
            // add the operation to our global static_list
            static_list.push_back(event);
            id++;
            ss.clear();
            ss.str(string());
        }
    }
    infile.close();
}

int main(int argc, char** argv)
{
    char* svalue;
    char* sourcefile_name;
    // cmd line:
    int c;
    opterr = 0;
    Scheduler* scheduler;
    int head = 0;
    //int idle = 0;
    int Total_time = 0; //simulation time or real time
    int Tot_movement = 0;
    //int event_count = 0;
    //int Tot_turnaround = 0;
    //int Tot_waittime = 0;
    double Avg_turnAroundTime = 0.0;
    double Avg_waitTime = 0.0;
    int Max_waittime = 0;
    // ./iosched –s<schedalgo> <inputfile>
    while((c = getopt (argc, argv, "s:"))!=-1)
    {
        switch(c)
        {
            case 's':
            {
                svalue = optarg;
                break;
            }

            default:
                abort();
        }
    }
    sourcefile_name = argv[optind];
    read_infile(sourcefile_name);

    //=========pick a scheduler========
    switch(svalue[0])
    {
        case 'i':
        {
            scheduler = new FIFO();
            break;
        }

        case 'j':
        {
            scheduler = new SSTF();
            break;
        }
        case 's':
        {
            scheduler = new SCAN();
            break;
        }

        case 'c':
        {
            scheduler = new CSCAN();
            break;
        }

        case 'f':
        {
            scheduler = new FSCAN();
            break;
        }

        default:
            break;
    }

    //=========now process the operations========
    list<Event*>::iterator it = static_list.begin();
    //note that it points to the event come after the last one finished
    while(it!=static_list.end())
    {
        Total_time = (*it)->arr_time;
        scheduler->add_event((*it));
        it++;
        while(!scheduler->isempty())
        {
            Event* event = scheduler->get_event(head);
            //cout<<"issue: "<<event->track_num<<endl;
            event->st_time = Total_time;
            int move = abs(event->track_num - head);
            Tot_movement += move;
            Total_time += move;
            event->end_time = Total_time;
            head = event->track_num;
            //we must make sure that all the events in the dyna_list is the schedulable!(has already come)
            for(; it!=static_list.end(); it++)
            {
                if((*it)->arr_time<=Total_time)
                    scheduler->add_event((*it));
                else
                    break;
            }
        }
    }

    //=======get turn around time and wait time======
    double Tot_turnaround = 0;
    double Tot_waittime = 0;
    for(it=static_list.begin(); it!=static_list.end(); it++)
    {
        int wait_time = (*it)->st_time - (*it)->arr_time;
        if(wait_time > Max_waittime)
            Max_waittime = wait_time;
        Tot_waittime += wait_time;
        Tot_turnaround += (*it)->end_time - (*it)->arr_time;
    }
    int event_count = (int) static_list.size();

    //=======print summary======
    Avg_turnAroundTime =  Tot_turnaround/event_count;
    Avg_waitTime = Tot_waittime/event_count;
    printf("SUM: %d %d %.2lf %.2lf %d\n", Total_time, Tot_movement, Avg_turnAroundTime, Avg_waitTime, Max_waittime);

    return 0;
}
