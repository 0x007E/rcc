/**
 * @file led.c
 * @brief Implementation of LED control functions using SPI interface.
 *
 * This source file provides functions to initialize and control addressable LEDs, including setting colors, blinking patterns, and turning LEDs on or off. It utilizes the SPI communication peripheral to send LED data frames.
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

#include "led.h"

static void led_frame(unsigned char mode, unsigned char red, unsigned char green, unsigned char blue)
{
	spi_transfer(mode);
	spi_transfer(blue);
	spi_transfer(green);
	spi_transfer(red);
}

/**
 * @brief Initialize the LED control interface and hardware.
 *
 * @details
 * This function initializes the SPI peripheral to communicate with the LEDs using most significant bit (`MSB`) first, with clock polarity and phase set to rising edges. After SPI initialization, it sends a start-of-frame (`SOF`) signal followed by initializing all configured LEDs with the enable flag and zero color data (LEDs initially off). Finally, it sends an end-of-frame (`EOF`) signal to mark completion of the initialization sequence. This setup prepares the LEDs for subsequent color and blink control operations by configuring the communication and initial states.
 *
 * @note This function must be called before any other LED control operations. It assumes `SPI_MSB`, `SPI_Rising` macros and LED frame configurations are correctly defined.
 *
 * @see spi_init() for SPI communication setup.
 * @see LED_SOF() and LED_EOF() macros for frame delimiters.
 * @see led_frame() for sending individual LED data frames.
 */
void led_init(void)
{
    spi_init(SPI_MSB, SPI_Rising, SPI_Rising);
	
	LED_SOF();
	for (unsigned char i=0; i < LED_NUMBER_OF_LEDS; i++)
	{
		led_frame(LED_ENABLE_FLAG, 0x00, 0x00, 0x00);
	}
	LED_EOF();
}

/**
 * @brief Disable the LED hardware interface.
 *
 * @details
 * This function sends multiple sleep commands to all configured LEDs to put them into a low power state. It transmits four repeated LED sleep frames using SPI and then disables the SPI peripheral itself.
 *
 * This operation is used to safely turn off the LEDs and reduce power consumption when LED functionality is not needed.
 *
 * @note Ensure no ongoing LED data transmission occurs before calling this function to avoid communication issues.
 * @see spi_disable() for disabling the SPI communication peripheral.
 */
void led_disable(void)
{
	for (unsigned char j=0; j < 4; j++)
	{
		LED_SOF();
		for (unsigned char i=0; i < LED_NUMBER_OF_LEDS; i++)
		{
			led_frame(LED_SLEEP_FLAG, 0x00, 0x00, 0x00);
		}
		LED_EOF();
	}
	spi_disable();
}

/**
 * @brief Transmit a specified value repeatedly over SPI to form a data frame.
 *
 * @param value The byte value to be sent repeatedly.
 *
 * @details
 * This function sends the given `value` repeatedly for `LED_FRAME_SIZE` times via `SPI` using the `spi_transfer` function.
 * It is commonly used to send `start` or `stop` frames for LED data sequences to synchronize communication with the LED hardware.
 *
 * @note The function blocks until all bytes are transmitted. Ensure the SPI interface is initialized before calling this function.
 */
void led_xof(unsigned char value)
{
    for (unsigned char i=0; i < LED_FRAME_SIZE; i++)
    {
        spi_transfer(value);
    }
}

/**
 * @brief Send an LED data frame with specified color and intensity.
 *
 * @param data LED_Data structure containing intensity and RGB color values.
 *
 * @details
 * This function constructs and transmits a single LED data frame over SPI, combining the LED enable flag with the masked intensity value, followed by the blue, green, and red color components. The intensity value is masked with 0x3F to limit it to valid bits.
 *
 * The frame format is:
 * - Mode byte: `LED_ENABLE_FLAG` OR'ed with intensity (`0x3F` mask).
 * - Blue color byte.
 * - Green color byte.
 * - Red color byte.
 *
 * @note Ensure the LED is initialized before calling this function.
 */
void led_data(LED_Data data)
{
	led_frame((LED_ENABLE_FLAG | (0x3F & data.intensity)), data.red, data.green, data.blue);
}

/**
 * @brief Generate a color configuration for an LED based on its status and intensity.
 *
 * @param status The LED status indicating the system state (e.g., ready, warning, error).
 * @param intensity The brightness intensity level for the LED (`0-255`).
 * 
 * @return Returns an `LED_Data` structure populated with the RGB color values corresponding 
 *         to the specified status and intensity.
 *
 * @details
 * This function initializes an @c LED_Data structure with the provided intensity and default 
 * black color (all zero). It then assigns color values based on the status:
 * - `LED_Status_Ready` assigns full green brightness,
 * - `LED_Status_Warning` assigns full red and green brightness (yellow),
 * - `LED_Status_Error` assigns full red brightness.
 * 
 * This mapping follows common LED indicator color conventions to visually represent different states.
 *
 * @note The returned color values are maximum brightness (`0x3F`) for the color channels, regardless of the input intensity which only affects overall brightness.
 */
LED_Data led_status_color(LED_Status status, unsigned char intensity)
{
    LED_Data temp = {
        intensity,
        0x00,
        0x00,
        0x00
    };

    switch (status)
    {
        case LED_Status_Ready:
            temp.green = 0xFF;
        break;
        case LED_Status_Warning:
            temp.red = 0xFF;
            temp.green = 0xFF;
        break;
        case LED_Status_Error:
            temp.red = 0xFF;
        break;
        default:
        break;
    }

    return temp;
}

static void led_delay(LED_Delay delay)
{
    switch (delay)
    {
        case LED_Delay_MS_100:
            _delay_ms(100);
            break;
        case LED_Delay_MS_200:
            _delay_ms(200);
            break;
        case LED_Delay_MS_300:
            _delay_ms(300);
            break;
        case LED_Delay_MS_400:
            _delay_ms(400);
            break;
        case LED_Delay_MS_500:
            _delay_ms(500);
            break;
        default:
            break;
    }
}

inline static void led_off(void)
{
    led_frame(LED_ENABLE_FLAG, 0x00, 0x00, 0x00);
}

/**
 * @brief Turn off all LEDs by sending zero-intensity frames.
 *
 * @details
 * This function sends a start-of-frame signal followed by zero-intensity data frames to all configured LEDs, effectively turning them off. It concludes by sending an end-of-frame signal. This ensures all LEDs are turned off cleanly by resetting their color and intensity data.
 *
 * @note This function assumes the SPI interface is already initialized and ready for communication.
 * Use `led_off()` internally to send individual LED off frames.
 * 
 * @see `LED_SOF()`, `LED_EOF()` for frame delimiters.
 * @see `led_off()` for turning off a single LED.
 */
void leds_off(void)
{
    LED_SOF();
    for (unsigned char i=0; i < LED_NUMBER_OF_LEDS; i++)
    {
        led_off();
    }
    LED_EOF();
}

/**
 * @brief Set the colors of specified LEDs based on position flags.
 *
 * @param position Bitwise flags indicating which LEDs to color and their blinking modes.
 * @param color The LED_Data structure specifying color and intensity for the LEDs.
 *
 * @details
 * This function initiates an LED data frame sequence by sending a start-of-frame (`SOF`) signal. It then iterates over all configured LEDs and determines, based on their position and the `position` flags, whether to set the LED color to the specified `color` or turn the LED `off`.
 * 
 * For odd numbers of LEDs, the middle LED is always turned off. The function supports left and right LED positions as well as alternating blinking flags. The LED data or off frames are sent accordingly. Finally, an end-of-frame (`EOF`) signal is sent to mark the completion of the LED update.
 *
 * @note Ensure the SPI interface is initialized and the LED hardware supports the specified frame format.
 *
 * @see `LED_SOF()`, `LED_EOF()` for frame delimiters.
 * @see `led_data()` and `led_off()` for sending LED on/off frames.
 */
void led_color(LED_Position position, LED_Data color)
{
    LED_SOF();
    for (unsigned char j=0; j < LED_NUMBER_OF_LEDS; j++)
    {
        if((LED_NUMBER_OF_LEDS%2) != 0 && (j == (LED_NUMBER_OF_LEDS>>1)))
        {
            led_off();
            continue;
        }

        if(j < (LED_NUMBER_OF_LEDS>>1))
        {
            if((position & LED_Position_Left) || (position & LED_Position_Left_Alternating))
            {
                led_data(color);
            }
            else
            {
                led_off();
            }
        }
        else
        {
            if((position & LED_Position_Right) || (position & LED_Position_Right_Alternating))
            {
                led_data(color);
            }
            else
            {
                led_off();
            }
        }
    }
    LED_EOF();
}

/**
 * @brief Blink LEDs on specified positions with given color, delay and repeat count.
 *
 * @param position Bitwise flags indicating LED positions and blinking modes to be used.
 * @param color The color and intensity to blink on the LEDs.
 * @param delay Enumeration specifying delay duration between blink states.
 * @param repeat The number of times to repeat the blink sequence.
 *
 * @details
 * This function controls blinking patterns for LEDs by alternately activating the specified LED positions and their alternating blinking counterparts. It uses the function led_color to set the color, and led_delay to wait between blinking states. The blinking sequence runs for `repeat` cycles, then turns off all LEDs using `leds_off()`.
 *
 * @note Ensure the SPI interface and LED hardware are properly initialized before calling this function.
 * The delay parameter controls brightness on/off period in milliseconds.
 *
 * @see `led_color()` for setting LED colors.
 * @see `led_delay()` for insertion of delay intervals.
 * @see `leds_off()` to turn off LEDs after blinking completes.
 */
void led_blink(LED_Position position, LED_Data color, LED_Delay delay, unsigned char repeat)
{
    for (unsigned char i=0; i < (repeat + 1); i++)
    {
        led_color((position & (LED_Position_Left | LED_Position_Right)), color);
        led_delay(delay);
        led_color((position & (LED_Position_Left_Alternating | LED_Position_Right_Alternating)), color);
        led_delay(delay);
    }
    leds_off();
}