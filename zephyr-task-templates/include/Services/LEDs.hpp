/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */
#ifndef _________SERVICE_LEDS_H____________
#define _________SERVICE_LEDS_H____________

#include "ActiveObject.hpp"

namespace Service {		
	class LEDs : public RTOS::ActiveObject<LEDs>
    {
	private:
		static void InitializeDriver(void);
		static void show(void);
    public:
        static void Initialize();
        static void Handle(const uint8_t arg[]);
        static void End(){
        };

        LEDs() : RTOS::ActiveObject<LEDs>(){};

	public:
		static size_t mColor;
		static int mRc;
	};
}

#endif //_________SERVICE_LEDS_H____________
