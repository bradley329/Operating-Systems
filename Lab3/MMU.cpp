#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "PTE.h"
#include "Pager.h"
#include "Event.h"
#include "Stat.h"
#include "NRU.h"
#include "Random.h"
#include "FIFO.h"
#include "SecondChance.h"
#include "Clock.h"
#include "Aging.h"

using namespace std;

// Global variables:
Pager * pager;
std::list<Event*> event_list;  //all the events list, for output.
int num_phys;
const int num_virt = 64;
int index_free = 0; //this points to the next free frame, if frame is full then it makes no sense!

// since we have to support random access and we are sure of the capacity of both
// pagetable and frametable, so we choose to use array instead of list.
// we wanna support random access so use vector instead of list.
std::vector<PTE> page_table(num_virt);
std::vector<int> frame_table;
// we do not need to implement a free list,
// record the num_free and when num_free reaches 0,
// we use the replacement algorithm to find the index of free frame.
Stat status = Stat();

void perform(int id, int r_w, int virt_idx)
{
    //cout<<"exe "<<op_code<<" "<<virtual_page_index<<endl;
    int phys_idx;
    Event * event = new Event(1, id, r_w, virt_idx, 0);
    event_list.push_back(event);
    page_table[virt_idx].refered = 1;
    // first check whether this instruction is present in the page_table
    if(page_table[virt_idx].present)
    {
        if(r_w==1) page_table[virt_idx].modified = 1;
        return;
    }
    else
    {
        // if not, determine whether the frame table is full
        if(index_free < num_phys)
        {
            // no need to perform replacement
            // whether it had been paged_out or not?
            phys_idx = index_free;
            if(page_table[virt_idx].pageout)
            {
                //page in operation
                event = new Event(4, id, r_w, virt_idx, phys_idx);
                event_list.push_back(event);
                status.num_inst++;
            }
            else
            {
                // zero operation
                event = new Event(6, id, r_w, virt_idx, phys_idx);
                event_list.push_back(event);
                status.num_zero++;
            }
        }
        else
        {
            // Need to perform page replacement
            phys_idx = pager->get_frame();
            // this is the virt_idx which would be unmapped
            int virt_idx_old = frame_table[phys_idx];

            // first unmap the old frame operation
            event = new Event(3, id, r_w, virt_idx_old, phys_idx);
            event_list.push_back(event);
            status.num_unmap++;
            page_table[virt_idx_old].present = 0;

            // next is where I was confused about:
            // for the replaced old one, we should examine whether it is modified
            // to determine whether pageout the old one
            if(page_table[virt_idx_old].modified)
            {
                // page out operation
                event = new Event(5, id, r_w, virt_idx_old, phys_idx);
                event_list.push_back(event);
                status.num_out++;
                page_table[virt_idx_old].pageout = 1;
                // remember to set modified to zero!
                page_table[virt_idx_old].modified = 0;
            }

            // for the coming new frame, we should determine
            // whether perform a pagein(rewrite the frame) or zero operation
            if(page_table[virt_idx].pageout)
            {
                event = new Event(4, id, r_w, virt_idx, phys_idx);
                event_list.push_back(event);
                status.num_in++;
            }
            else //zero
            {
                event = new Event(6, id, r_w, virt_idx_old, phys_idx);
                event_list.push_back(event);
                status.num_zero++;
            }
        }
        // map operation
        event = new Event(2, id, r_w, virt_idx, phys_idx);
        event_list.push_back(event);
        status.num_map++;
        page_table[virt_idx].phys_idx = (unsigned int) phys_idx;
        frame_table[phys_idx] = virt_idx;
        if(r_w==1) page_table[virt_idx].modified = 1;
        page_table[virt_idx].present = 1;
        pager->valid_list.push_back(phys_idx); //add to present list;
        index_free++;
    }
}

int main(int argc, char** argv)
{
    // a:o:f
    // NULL?
    int aflag = 0;
    char *avalue;
    int fflag = 0;
    char *fvalue;
    // o: options
    char* ovalue;
    // which scheduler to run?
    char* sourcefile_name;
    // OPFS:
    char* randfile_name;

    // cmd line:
    int c;
    opterr = 0;

    // ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile
    while((c = getopt (argc, argv, "a:o:f:"))!=-1)
    {
        switch(c)
        {
            case 'a':
            {
                aflag = 1;
                avalue = optarg;
                break;
            }

            case 'o':
            {
                ovalue = optarg;
                break;
            }

            case 'f':
            {
                fflag = 1;
                fvalue = optarg;
                break;
            }

            default:
                abort();
        }
    }
    sourcefile_name = argv[optind];
    randfile_name = argv[optind+1];

    if(aflag==0)
        avalue = (char *) "r"; //asked professor;
    if(fflag==0)
        fvalue = (char *) "32";
    num_phys = atoi(fvalue);
    //num_free = num_phys;
    frame_table.reserve((unsigned long) num_phys);

//=========pick a pager========
    switch(avalue[0])
    {
        case 'N':
        {
            pager = new NRU(1, &page_table, &frame_table, randfile_name, num_virt);
            break;
        }

        case 'r':
        {
            pager = new Random(2, &page_table, &frame_table, num_phys, randfile_name);
            break;
        }
        case 'f':
        {
            pager = new FIFO(3, &page_table, &frame_table);
            break;
        }

        case 's':
        {
            pager = new SecondChance(4, &page_table, &frame_table);
            break;
        }

        case 'c':
        {
            pager = new Clock(5, &page_table, &frame_table, num_phys, num_virt);
            break;
        }

        case 'X':
        {
            pager = new Clock(6, &page_table, &frame_table, num_phys, num_virt);
            break;
        }

        case 'a':
        {
            pager = new Aging(7, &page_table, &frame_table, num_phys, num_virt);
            break;
        }

        case 'Y':
        {
            pager = new Aging(8, &page_table, &frame_table, num_phys, num_virt);
            break;
        }

        default:
            break;
    }

//=========Option: OPFS========
    int Oflag = 0;
    int Pflag = 0;
    int Fflag= 0;
    int Sflag = 0;

    for(int i = 0; ovalue[i]; i++)
    {
        switch(ovalue[i])
        {
            case 'O':
                Oflag = 1;
                break;
            case 'P':
                Pflag = 1;
                break;
            case 'F':
                Fflag = 1;
                break;
            case 'S':
                Sflag = 1;
                break;
            default:
                continue;
        }
    }

//=======execute the instructions======
    ifstream infile;
    int r_w;
    int idx; //virtual table index
    string str;
    stringstream ss;
    int id = 0; //instruction id;
    infile.open(sourcefile_name);
    if(infile.good())
    {
        while(getline(infile, str))
        {
            if (str.at(0) == '#') continue;
            ss.str(str);
            ss >> r_w >> idx;
            // perform this instruction
            perform(id, r_w, idx);
            id++;
            status.num_inst++;
            ss.clear();
            ss.str(string());
        }
    }
    infile.close();

//=======print the results based on OPFS======
    if(Oflag==1)
    {
        //print events:
        for(list<Event*>::iterator it=event_list.begin(); it!=event_list.end(); it++)
            (*it)->print_event();
    }

    if(Pflag==1)
    {
        for(int i=0; i<64; i++)
        {
            if(page_table[i].present == 0)
            {
                if(page_table[i].pageout)
                    cout<<"#";
                else
                    cout<<"*";
            }
            else
            {
                cout<<i<<":";

                if(page_table[i].refered)
                    cout<<"R";
                else
                    cout<<"-";

                if(page_table[i].modified)
                    cout<<"M";
                else
                    cout<<"-";

                if(page_table[i].pageout)
                    cout<<"S";
                else
                    cout<<"-";
            }
            if(i!=63)
                cout<<" ";
        }
        cout<<endl;
    }

    if(Fflag==1)
    {
        for(int i=0; i<num_phys; i++)
        {
            if(i < index_free)
                cout<<frame_table[i];
            else
                cout<<"*";
            cout<<" ";
        }
        cout<<endl;
    }

    if(Sflag==1)
        status.print_stat();

    return 0;
}
