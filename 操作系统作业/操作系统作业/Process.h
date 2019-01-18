#pragma once
class Process
{
public:
    char name[20]; //进程名臣
    int need_time; //所需要的时间
    int allocation; //占用CPU的情况
    char state;     //目前的

    Process();
    ~Process();
};

