#ifndef SERVICE_LEDs__H_H
#define SERVICE_LEDs__H_H
#include "ActiveObject.hpp"
#define ADD_TO_BLINK_COLOR_OPCODE 0xAA
#define RESET_BLINK_COLOR_OPCODE 0xBB
#define FIRE_BLINK_COLOR_OPCODE 0xCC
/**
 * Customize the static methods of an RTOS::ActiveObject
 */
namespace Service
{
    class LEDs : public RTOS::ActiveObject<LEDs>
    {
    public:
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };
        static void Fire2012WithPalette();
        LEDs() : RTOS::ActiveObject<LEDs>(){};
    private:

    };

}

#endif
