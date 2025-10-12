

#ifndef MAIN_H_
#define MAIN_H_

	#define F_CPU 20000000UL
	#define SWITCH PIN7_bm
	#define SWITCH_COMMAND_EXECUTE_MS 3000UL
	#define SWITCH_SYSTEM_OFF_TIME_MS 3000UL
	#define COLOR_FADE_DELAY_MS 10UL
	#define COLOR_INTENSITY_DELAY_MS 350UL
	#define ENABLE_EEPROM_WRITE

	#include <avr/io.h>
	#include <avr/sleep.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include "./hal/avr0/system/system.h"
	#include "./battery/battery.h"
	#include "./led/led.h"

#endif /* MAIN_H_ */