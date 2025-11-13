/**
 * @file battery.h
 * @brief Battery monitoring interface for AVR microcontrollers.
 *
 * This header file defines the interface for battery status monitoring using an ADC channel. It provides macros to configure ADC channel and battery empty threshold, a status enumeration, and function prototypes to initialize battery measurement and retrieve battery status.
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

#ifndef BATTERY_H_
#define BATTERY_H_

    #ifndef BATTERY_CHANNEL
        /**
         * @def BATTERY_CHANNEL
         * @brief Specifies the ADC channel used for battery voltage measurement.
         *
         * @details
         * This macro defines the ADC channel number used to measure the battery voltage. By default, it is set to `ADC_CH6`. Override this macro before including the battery module to use a different ADC channel if your hardware configuration requires it.
         *
         * @note Ensure the selected ADC channel is properly connected to the battery voltage sensing circuitry.
         */
        #define BATTERY_CHANNEL ADC_CH6
    #endif

    #ifndef BATTERY_EMPTY_VALUE
        /**
         * @def BATTERY_EMPTY_VALUE
         * @brief ADC threshold value indicating an empty or critically low battery.
         *
         * @details
         * This macro defines the ADC reading value below which the battery is considered empty or in fault condition. Override this macro before including the battery module to adjust the threshold value if your hardware or battery specifications differ.  
         *
         * @note The setting of the `BATTERY_EMPTY_VALUE` depends on the voltage reference of the A/D converter and the input voltage of the channel.
         */
        #define BATTERY_EMPTY_VALUE 860UL
    #endif

    #include <avr/io.h>
    #include "../hal/avr0/adc/adc.h"

    /**
     * @enum BATTERY_Status_t
     * @brief Represents the status of the battery voltage measurement.
     *
     * @details
     * This enumeration defines possible statuses indicating the state of the battery based on the analog-to-digital converter (ADC) measurement.
     *
     * Values include:
     * - `BATTERY_Ok`: Battery voltage is above the defined empty threshold, indicating a healthy battery.
     * - `BATTERY_Fault`: Battery voltage is below the threshold, indicating an empty or faulty battery.
     */
    enum BATTERY_Status_t
    {
        BATTERY_Ok=0,
        BATTERY_Fault
    };

    /**
     * @typedef BATTERY_Status
     * @brief Alias for enum BATTERY_Status_t to represent battery status codes.
     */
    typedef enum BATTERY_Status_t BATTERY_Status;

    void battery_init(void);
    void battery_disable(void);
    BATTERY_Status battery_status(void);

#endif /* BATTERY_H_ */