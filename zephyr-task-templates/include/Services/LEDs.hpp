/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */
#ifndef _________SERVICE_LEDS_H____________
#define _________SERVICE_LEDS_H____________

#include <zephyr/sys/util.h>
namespace Service {		
	class LEDs {
		public:
		static void init(void);
		static void show(void);

		static size_t mColor;
		static int mRc;
	};
}

#endif //_________SERVICE_LEDS_H____________
