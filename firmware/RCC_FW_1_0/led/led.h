/**
 * @file led.h
 * @brief LED control interface for AVR microcontrollers using SPI.
 *
 * This header file defines the interface for controlling LEDs, including configuration macros, status and delay enumerations, LED position flags, and data structures for LED color and intensity. Functions are provided to initialize LED hardware, send data frames via SPI, control LED colors, and perform LED blinking actions. The LED driver supports multiple LEDs with configurable frame sizes and intensity limits.
 *
 * @author g.raf
 * @date 2025-09-28
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 *
 * @note This file is part of a larger embedded systems project and subject to the license specified in the repository. For updates  and the complete revision history, see the GitHub repository.
 *
 * @see https://github.com/0x007e/rcc "AVR ATmega GitHub Repository"
 */

#ifndef LED_H_
#define LED_H_

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

    #ifndef LED_NUMBER_OF_LEDS
        /**
         * @def LED_NUMBER_OF_LEDS
         * @brief Defines the total number of LEDs controlled by the driver.
         *
         * @details
         * This macro specifies how many individual LEDs are connected and managed. The default is 2 LEDs.
         *
         * @note Override as needed for the actual number of LEDs in the hardware configuration.
         */
        #define LED_NUMBER_OF_LEDS 2
    #endif

    #ifndef LED_FRAME_SIZE
        /**
         * @def LED_FRAME_SIZE
         * @brief Defines the size of the LED start/stop data frame in bytes.
         *
         * @details
         * This macro indicates how many bytes make up a start/stop LED data frame sent via SPI. The default frame size is 4 bytes.
         *
         * @note Modify this value if the LED hardware protocol requires a different start/stop frame size.
         */
        #define LED_FRAME_SIZE 4
    #endif

    #ifndef LED_START_VALUE
        /**
         * @def LED_START_VALUE
         * @brief Defines the start-of-frame marker value for LED data transmission.
         *
         * @details
         * This value is sent to indicate the start of a LED data frame sequence. The default start value is `0x00`.
         */
        #define LED_START_VALUE 0x00
    #endif

    #ifndef LED_STOP_VALUE
        /**
         * @def LED_STOP_VALUE
         * @brief Defines the end-of-frame marker value for LED data transmission.
         *
         * @details
         * This value is sent to mark the end of a LED data frame sequence. The default stop value is `0xFF`.
         */
        #define LED_STOP_VALUE 0xFF
    #endif
    
	#ifndef LED_ENABLE_FLAG
        /**
         * @def LED_ENABLE_FLAG
         * @brief Flag used to enable LED operation.
         *
         * @details
         * This flag value, default `0xE0`, is used in communication settings to signal enabling the LED device or functionality.
         */
		#define LED_ENABLE_FLAG 0xE0
	#endif
	
	#ifndef LED_SLEEP_FLAG
        /**
         * @def LED_SLEEP_FLAG
         * @brief Flag used to set the LED device into sleep mode.
         *
         * @details
         * The value `0xA0` is used to command the LED device to enter a low power or sleep state.
         */
		#define LED_SLEEP_FLAG 0xA0
	#endif
	
    #ifndef LED_MIN_INTENSITY
        /**
         * @def LED_MIN_INTENSITY
         * @brief The minimum intensity value for LED brightness.
         *
         * @details
         * This macro defines the lowest valid intensity level for LED brightness. The default minimum is `0x01`.
         */
        #define LED_MIN_INTENSITY 0x01
    #endif

    #ifndef LED_MAX_INTENSITY
        /**
         * @def LED_MAX_INTENSITY
         * @brief The maximum intensity value for LED brightness.
         *
         * @details
         * This macro defines the highest valid intensity level for LED brightness. The default maximum is `0x0F`.
         */
        #define LED_MAX_INTENSITY 0x0F
    #endif

    #include <avr/io.h>
    #include <util/delay.h>

    #include "../hal/avr0/spi/spi.h"

    /**
     * @enum LED_Status_t
     * @brief Enumerates possible LED status types indicating different system states.
     *
     * @details
     * This enumeration defines the LED status codes used to represent system conditions via LED color or behavior. It includes states such as Ready, Warning, and Error.
     */
    enum LED_Status_t
    {
        LED_Status_None=0,
        LED_Status_Ready,
        LED_Status_Warning,
        LED_Status_Error
    };

    /**
     * @typedef LED_Status
     * @brief Alias for enum LED_Status_t representing possible LED system status codes.
     */
    typedef enum LED_Status_t LED_Status;

    /**
     * @enum LED_Delay_t
     * @brief Defines selectable delay durations for LED blinking actions.
     *
     * @details
     * This enumeration provides predefined delay durations to control the timing of LED blink cycles, ranging from no delay to 500 ms delay intervals.
     */
    enum LED_Delay_t
    {
        LED_Delay_None=0,
        LED_Delay_MS_100,
        LED_Delay_MS_200,
        LED_Delay_MS_300,
        LED_Delay_MS_400,
        LED_Delay_MS_500
    };

    /**
     * @typedef LED_Delay
     * @brief Alias for enum LED_Delay_t representing predefined LED blink delay durations.
     */
    typedef enum LED_Delay_t LED_Delay;

    /**
     * @enum LED_Position_t
     * @brief Defines LED position flags and alternating blinking modes.
     *
     * @details
     * This enumeration specifies individual LED positions and options for alternating blinking patterns. Positions include left and right LEDs, as well as flags for alternating blink sequences.
     */
    enum LED_Position_t
    {
        LED_Position_None=0x00,
        LED_Position_Left=0x01,
        LED_Position_Right=0x02,
        LED_Position_Left_Alternating=0x04,
        LED_Position_Right_Alternating=0x08,
    };

    /**
     * @typedef LED_Position
     * @brief Alias for enum LED_Position_t representing LED position flags and blinking options.
     */
    typedef enum LED_Position_t LED_Position;

    /**
     * @struct LED_Data_t
     * @brief Represents the data structure for a single LED including color and intensity.
     *
     * @details
     * This structure bundles intensity and RGB color channels (`red`, `green`, `blue`) for one LED. Intensity is an unsigned char controlling brightness, while colors are `0-255` values.
     */
    struct LED_Data_t
    {
        unsigned char intensity;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    };

    /**
     * @typedef LED_Data
     * @brief Alias for struct LED_Data_t representing LED color and intensity data.
     */
    typedef struct LED_Data_t LED_Data;

    /**
     * @def LED_SOF
     * @brief Sends the Start-of-Frame (SOF) signal to the LED strip.
     *
     * @details
     * This macro transmits the predefined LED_START_VALUE as a start frame delimiter using the function `led_xof()`. It inserts a short delay of 10 microseconds to ensure proper timing before subsequent LED data transmission begins. The `SOF` marks the beginning of a new LED data sequence.
     */
    #define LED_SOF() { led_xof(LED_START_VALUE); _delay_us(10); }

    /**
     * @def LED_EOF
     * @brief Sends the End-of-Frame (EOF) signal to the LED strip.
     *
     * @details
     * This macro transmits the predefined LED_STOP_VALUE as an end frame delimiter using the function `led_xof()`. It inserts a short delay of 10 microseconds to ensure proper timing after LED data transmission ends. The `EOF` indicates the completion of the current LED data sequence.
     */
    #define LED_EOF() { led_xof(LED_STOP_VALUE); _delay_us(10); }

    void led_init(void);
	void led_disable(void);
    void led_xof(unsigned char value);
    void led_data(LED_Data data);
    void leds_off(void);

    LED_Data led_status_color(LED_Status status, unsigned char intensity);
    void led_color(LED_Position position, LED_Data color);
    void led_blink(LED_Position position, LED_Data color, LED_Delay delay, unsigned char repeat);

#endif /* LED_H_ */