#pragma once
class Process
{
public:
    char name[20]; //��������
    int need_time; //����Ҫ��ʱ��
    int allocation; //ռ��CPU�����
    char state;     //Ŀǰ��

    Process();
    ~Process();
};

