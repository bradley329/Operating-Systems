//
// Created by Lei Guo on 2017/3/16.
//

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include "scheduler.h"
#include "FCFS.h"
#include "LCFS.h"
#include "SJF.h"
#include "RR.h"
#include "PRIO.h"
using namespace std;

int main(int argc, char** argv)
{
    int vflag = 0;
    char* cvalue = NULL;
    // which scheduler to run?
    Scheduler* scheduler = NULL;
    int c;
    char* sourcefile_name;
    char* randfile_name;

    opterr = 0;
    // <program> [-v] [-s<schedspec>] inputfile randfile
    while ((c = getopt (argc, argv, "vs:")) != -1)
    {
        switch (c) {
            case 'v':
                vflag = 1;
                break;
            case 's':
                cvalue = optarg;
                break;
            case '?':
                //cout << "option can not be recognized" << endl;
                break;
            default:
                abort();
        }
    }
    switch (cvalue[0]) //cvalue is after -s:
    {
        case 'F':
        {
            sourcefile_name = argv[optind];
            //cout << "input: " << sourcefile_name << endl;
            randfile_name = argv[optind + 1];
            //cout << "rfile: " << randfile_name << endl;
            //ifstream sourcefile;
            //sourcefile.open(sourcefile_name);
            //ifstream randfile;
            //randfile.open(randfile_name);
            //cout << "opened" << endl;
            scheduler = new FCFS(sourcefile_name, randfile_name, numeric_limits<int>::max(), vflag);
            //cout << "FCFS sc" << endl;
            break;
        }
        case 'L':
        {
            sourcefile_name = argv[optind];
            randfile_name = argv[optind+1];
            scheduler = new LCFS(sourcefile_name, randfile_name, numeric_limits<int>::max(), vflag);
            break;
        }
        case 'S':
        {
            sourcefile_name = argv[optind];
            randfile_name = argv[optind+1];
            scheduler = new SJF(sourcefile_name, randfile_name, numeric_limits<int>::max(), vflag);
            break;
        }
        case 'R':
        {
            sourcefile_name = argv[optind];
            randfile_name = argv[optind+1];
            string ss(cvalue);
            ss = ss.substr(1);
            int quantum = atoi(ss.c_str());
            scheduler = new RR(sourcefile_name, randfile_name, quantum, vflag);
            break;
        }
        case 'P':
        {
            sourcefile_name = argv[optind];
            randfile_name = argv[optind+1];
            string ss(cvalue);
            ss = ss.substr(1);
            int quantum = atoi(ss.c_str());
            scheduler = new PRIO(sourcefile_name, randfile_name, quantum, vflag);
            break;
        }
        default:
            break;
    }

    //cout<<"out of while"<<endl;
    if(scheduler!=NULL) scheduler->simulate();
    else
    {
        cout<<"scheduler is null"<<endl;
    }
    return 0;
}
