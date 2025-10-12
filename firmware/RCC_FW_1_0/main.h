/**
 * @file main.h
 * @brief Main header file for RCC firmware.
 *
 * Defines core macros, includes, and configuration constants used throughout the RCC embedded system project. This header provides CPU frequency, switch timing constants, delay intervals, and feature switches. It also includes key AVR libraries and modular headers for system, battery, and LED functionality.
 *
 * @author g.raf
 * @date 2025-09-28
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 *
 * @note This file is part of the RCC AVR embedded project and subject to the repository license. For updates and revision history, please visit the GitHub repository.
 *
 * @see https://github.com/0x007e/rcc "RCC - RGB LED Color Cube"
 */

#ifndef MAIN_H_
#define MAIN_H_

	#ifndef F_CPU
        /**
         * @def F_CPU
         * @brief System clock frequency definition.
         *
         * @details
         * This macro defines the operating frequency of the microcontroller's clock in Hertz. It is used by delay functions and timing calculations. The value should match the actual hardware clock frequency to ensure correct timing behavior in the software.
         */
        #define F_CPU 20000000UL
    #endif
	
	#ifndef SWITCH
		/**
		 * @def SWITCH
		 * @brief Pin mask for the user switch/button input.
		 *
		 * @details
		 * Defines the bit mask for the hardware input pin connected to the user switch. It is used to detect button press events in the firmware.
		 */
		#define SWITCH PIN7_bm
	#endif

	#ifndef SWITCH_COMMAND_EXECUTE_MS
		/**
		 * @def SWITCH_COMMAND_EXECUTE_MS
		 * @brief Time window to finalize switch command execution in milliseconds.
		 *
		 * @details
		 * Defines the maximum duration to wait for button press sequences before executing the corresponding command. Used to handle multi-press functionality within this millisecond timeout.
		 */
		#define SWITCH_COMMAND_EXECUTE_MS 3000UL
	#endif

	#ifndef SWITCH_SYSTEM_OFF_TIME_MS
		/**
		 * @def SWITCH_SYSTEM_OFF_TIME_MS
		 * @brief Duration of button press to trigger system shutdown, in milliseconds.
		 *
		 * @details
		 * If the user holds the button for this time period, the system initiates a controlled shutdown sequence.
		 */
		#define SWITCH_SYSTEM_OFF_TIME_MS 3000UL
	#endif

	#ifndef COLOR_FADE_DELAY_MS
		/**
		 * @def COLOR_FADE_DELAY_MS
		 * @brief Delay duration for color fading effect in milliseconds.
		 *
		 * @details
		 * Controls the time interval between incremental color changes during LED color fading operations.
		 */
		#define COLOR_FADE_DELAY_MS 10UL
	#endif

	#ifndef COLOR_INTENSITY_DELAY_MS
		/**
		 * @def COLOR_INTENSITY_DELAY_MS
		 * @brief Delay duration for intensity changes in milliseconds.
		 *
		 * @details
		 * Defines the interval timing for changes in LED intensity during brightness adjustment routines.
		 */
		#define COLOR_INTENSITY_DELAY_MS 350UL
	#endif

	#ifndef ENABLE_EEPROM_WRITE
		/**
		 * @def ENABLE_EEPROM_WRITE
		 * @brief Enables EEPROM write support.
		 *
		 * @details
		 * When defined, the firmware supports writing updated LED configuration data to EEPROM. If undefined, EEPROM writes are disabled to protect memory from unnecessary wear.
		 */
		#define ENABLE_EEPROM_WRITE
	#endif

	#include <avr/io.h>
	#include <avr/sleep.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include "./hal/avr0/system/system.h"
	#include "./battery/battery.h"
	#include "./led/led.h"

#endif /* MAIN_H_ */