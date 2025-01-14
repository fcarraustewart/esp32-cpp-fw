#ifndef __IMU_h
#define __IMU_h

#include <ActiveObject.hpp>
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class IMU : public RTOS::ActiveObject<IMU>
    {
    public:
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        constexpr IMU() : RTOS::ActiveObject<IMU>(){};
    private:
        static void InitializeDriver();
        static int  Begin();
        static int  ReadRotXYZ();
        static int  ReadGyroXYZ();
        static int  ReadMagXYZ();
        static int  ReadAccelXYZ();
        static int  CountSteps(volatile uint32_t * pCount);

    };

};

#endif