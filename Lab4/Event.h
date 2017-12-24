//
// Created by Lei Guo on 2017/4/25.
//

#ifndef LAB4_EVENT_H
#define LAB4_EVENT_H
class Event
{
public:
    int id;
    int arr_time;
    int track_num;
    int st_time;
    int end_time; //these two are for calculating the wait time and turn around time
    Event(int _id, int _arr_time, int _track_num, int _st_time, int _end_time)
            : id(_id), arr_time(_arr_time), track_num(_track_num), st_time(_st_time), end_time(_end_time) {}
};
#endif //LAB4_EVENT_H
