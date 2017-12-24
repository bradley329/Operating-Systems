//
// Created by Lei Guo on 2017/4/14.
//

#ifndef OS_LAB3_EVENT_H
#define OS_LAB3_EVENT_H
#include <iostream>
#include <iomanip>
//just for reminding:
/*
enum type
{
    Come = 1, Map, Unmap, In, Out, Zero
};*/
class Event
{
public:
    int type;
    int id;
    int r_w;
    int virt_idx;
    int phys_idx;

    Event(int _type, int _id, int _r_w, int _virt_idx, int _phys_idx) {
        type = _type;
        id = _id;
        r_w = _r_w;
        virt_idx = _virt_idx;
        phys_idx = _phys_idx;
    }

    void print_event()
    {
        switch (type)
        {
            case 1:
                std::cout<<"==> inst: "<<r_w<<" "<<virt_idx<<std::endl;
                break;
            case 2:
                std::cout<<id<<": MAP"<<std::right<<std::setw(6)<<virt_idx<<std::right<<std::setw(4)<<phys_idx<<std::endl;
                break;
            case 3:
                std::cout<<id<<": UNMAP"<<std::right<<std::setw(4)<<virt_idx<<std::right<<std::setw(4)<<phys_idx<<std::endl;
                break;
            case 4:
                std::cout<<id<<": IN"<<std::right<<std::setw(7)<<virt_idx<<std::right<<std::setw(4)<<phys_idx<<std::endl;
                break;
            case 5:
                std::cout<<id<<": OUT"<<std::right<<std::setw(6)<<virt_idx<<std::right<<std::setw(4)<<phys_idx<<std::endl;
                break;
            case 6:
                std::cout<<id<<": ZERO"<<std::right<<std::setw(9)<<phys_idx<<std::endl;
                break;
            default:
                break;
        }
    }
};
#endif //OS_LAB3_EVENT_H
