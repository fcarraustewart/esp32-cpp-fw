#ifndef TASK__H_H
#define TASK__H_H
#include "FreeRTOS.h"

template <class D>
class Base
{
public:
    Base(D* derived)
    {
        mDerived = derived;
    };
    void PrintName() { mDerived->Run(); };

private:
    D* mDerived;
protected:
    std::string mName;
};


class Service : public Base<Service>
{
public:
    Service(const std::string name) : Base<Service>(this) { Base::mName = name;};
    void Run() 
    { 
        printf("Running task mName = %s\r\n", mName.c_str()); 
    };
private:
};

#endif
