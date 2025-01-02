/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */
#ifndef _________SERVICE_LEDS_H____________
#define _________SERVICE_LEDS_H____________

#include <ActiveObject.hpp>
#define CMD_LEDs_SHOW 									0x04
#define CMD_BUZZER_INITIALIZATION_COMPLETE_SONG 		0x05
#define CMD_WORKQUEUE_SONG 								0x06
#define CMD_BUZZER_AVAILABLE 							0x55
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

	private:
		static size_t mColor;
		static int mRc;
		static bool mBuzzerDriverRunning;
	};
}

#endif //_________SERVICE_LEDS_H____________
