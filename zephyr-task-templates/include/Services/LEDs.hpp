/*
 * Copyright (c) 2024 fcarraustewart.
 *
 */

#include <zephyr/sys/util.h>

class LEDs {
	public:
	static void init(void);
	static void show(void);

	static size_t mColor;
	static int mRc;
};
