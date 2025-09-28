/**
 * @file battery.c
 * @brief Battery monitoring functions for AVR microcontrollers.
 *
 * This source file implements the battery monitoring interface using ADC measurements. It provides functions to initialize the ADC for battery voltage measurement and to check the battery status against a predefined empty voltage threshold.
 *
 * @author g.raf
 * @date 2025-09-18
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 *
 * @note This file is part of a larger embedded systems project and subject to the license specified in the repository. For updates  and the complete revision history, see the GitHub repository.
 *
 * @see https://github.com/0x007e/rcc "AVR ATmega GitHub Repository"
 */

#include "battery.h"

/**
 * @brief Initialize the battery measurement interface.
 *
 * @details
 * This function initializes the ADC module and configures the ADC channel used for battery voltage measurement as defined by the `BATTERY_CHANNEL` macro. Must be called before reading battery status to ensure proper ADC setup.
 */
void battery_init(void)
{
    adc_init();
    adc_channel(BATTERY_CHANNEL);
}

/**
 * @brief Disable battery voltage measurement.
 *
 * @details
 * This inline function disables the ADC hardware module used for battery voltage measurement. Calling this function reduces power consumption by shutting down the ADC when battery monitoring is not needed.
 *
 * @note Ensure that no battery measurement is required before calling this function, as it will disable all ADC functionality until reinitialized.
 *
 * @see adc_disable() for generic ADC disable function.
 */
inline void battery_disable(void)
{
    adc_disable();
}

/**
 * @brief Check the current battery status.
 *
 * @return Returns `BATTERY_Ok` if the battery voltage is above the configured minimum threshold, otherwise returns `BATTERY_Fault`.
 *
 * @details
 * This function reads the battery voltage using the ADC and compares the value  against the `BATTERY_EMPTY_VALUE` threshold to determine if the battery is considered empty or faulty.
 */
BATTERY_Status battery_status(void)
{
    if(adc_read() < BATTERY_EMPTY_VALUE)
    {
        return BATTERY_Fault;
    }
    return BATTERY_Ok;
}
