#ifndef TASK__H_H
#define TASK__H_H
#include "FreeRTOS.h"
class Task
{
public:
    Task(const std::string name) { mName = name;};
    void PrintName() { printf("mName = %s\r\n", mName.c_str()); };
    std::string GetName() { return mName; };

private:
    std::string mName;
};

template <class D>
class Base
{
public:
    Base(D* derived)
    {
        mDerived = derived;
        mName = derived->GetName();
    };
    void PrintName() { mDerived->PrintName(); };

private:
    D* mDerived;
    std::string mName;
};
#endif
