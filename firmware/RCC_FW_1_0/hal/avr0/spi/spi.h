/**
 * @file spi.h
 * @brief Header file with declarations and macros for hardware SPI.
 * 
 * This file provides function prototypes, type definitions, and constants
 * for hardware-based SPI communication on AVR0 microcontrollers.
 * 
 * @author g.raf
 * @date 2025-09-27
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 * 
 * @note This file is part of a larger project and subject to the license specified in the repository. For updates and the complete revision history, see the GitHub repository.
 *
 * @see https://github.com/0x007e/avr0  "AVR ATmega GitHub Repository"
 */

#ifndef SPI_H_
#define SPI_H_

    #ifndef SPI2X_ENABLE
        /**
         * @def SPI2X_ENABLE
         * @brief Enables the SPI double speed mode.
         *
         * @details
         * When defined, this macro activates the `SPI_CLK2X` bit, which reduces the SPI clock prescaler from `4` to `2`, effectively doubling the SPI clock speed. The SPI2X_ENABLE mode is useful when higher SPI data transfer speeds are required. It modifies the SPI clock frequency by changing the division factor applied to the peripheral clock (`F_PER`).
         *
         * @note Typically used in conjunction with the SPI clock prescaler bits (`PRESC[1:0]`) in the SPI control register (`CTRLA`).
         */
        //#define SPI2X_ENABLE

        #ifdef _DOXYGEN_    // Used for documentation, can be ignored
            #define SPI2X_ENABLE
        #endif
    #endif

    #ifndef SPI_CLOCK
        /**
         * @def SPI_CLOCK
         * @brief Default SPI clock prescaler setting.
         *
         * @details
         * This macro sets the SPI clock prescaler to configure the SPI clock frequency. If not previously defined, it defaults to `SPI_PRESC_DIV4_gc`. The prescaler divides the peripheral clock frequency (`F_PER`) according to the table below, influenced by the `SPI2X` bit (double speed mode):
         *
         * +------------------------------+----------+----------+
         * |         SPI2X                |     0    |     1    |
         * +------------------------------+----------+----------+
         * | SPI_PRESC_DIV4_gc   -> F_PER | F_PER/4  | F_PER/2  |
         * | SPI_PRESC_DIV16_gc  -> F_PER | F_PER/16 | F_PER/8  |
         * | SPI_PRESC_DIV64_gc  -> F_PER | F_PER/64 | F_PER/32 |
         * | SPI_PRESC_DIV128_gc -> F_PER | F_PER/128| F_PER/64 |
         * +------------------------------+----------+----------+
         *
         * Where `F_PER` is the peripheral clock frequency.
         *
         * @note To prevent this, do not exceed the maximum clock frequency of the slave to prevent unwanted system behavior.
         */
        #define SPI_CLOCK SPI_PRESC_DIV4_gc
    #endif

    #ifndef SPI_PORTMUX
        /**
         * @def SPI_PORTMUX
         * @brief Selects the alternate port location for SPI pins.
         *
         * @details
         * This macro configures which physical port pins are used for the SPI interface. It determines the pin mapping of SPI signals such as MOSI, MISO, SCK, and SS.
         *
         * The value can be set to predefined routing codes:
         * ATTINY:
         * - `PORTMUX_SPI0_DEFAULT_gc`: Default mapping referenced in datasheet (e.g. ATTiny1606 -> PA[4:1]).
         * - `PORTMUX_SPI0_ALTERNATE_gc`: Default mapping referenced in datasheet (e.g. ATTiny1606 -> PC[3:0]).
         * ATMEGA(4808):
         * - `PORTMUX_SPI0_DEFAULT_gc`: Default mapping referenced in datasheet (e.g. ATmega4808 -> PA[7:4]).
         * - `PORTMUX_SPI0_ALT1_gc`: Alternative 1 referenced in datasheet (e.g. ATmega4808 -> PC[3:0]).
         * - `PORTMUX_SPI0_ALT2_gc`: Alternative 2 referenced in datasheet (e.g. ATmega4808 -> PE[3:0]).
         * - `PORTMUX_SPI0_NONE_gc`: Signals not connected to any pins
         *
         * By default, `SPI_ALTERNATE_PORT` is set to `PORTMUX_SPI0_ALTERNATE_gc`. Override this macro prior to including the SPI module to select a different port mapping.
         *
         * @note Changing this affects the hardware SPI pin assignment and may require corresponding changes in the PCB or wiring.
         */
        #define SPI_PORTMUX PORTMUX_SPI0_DEFAULT_gc

        #ifdef _DOXYGEN_    // Used for documentation, can be ignored
            #define SPI_PORTMUX PORTMUX_SPI0_DEFAULT_gc
        #endif
    #endif

    #ifndef SPI_PORT
        /**
         * @def SPI_PORT
         * @brief Specifies the port register for the SPI interface.
         *
         * @details
         * This macro defines the default port register used by the SPI bus It determines which hardware port is used for SPI pin control and direction settings.
         
         @important When using alternate SPI pin mappings, with `SPI_ALTERNATE_PORT`, this macro should be updated to reflect the correct port.
         *
         * By default, `SPI_PORT` is set to `PORTA`. Override this macro before including the SPI module if an alternate port configuration is needed.
         *
         * @note Always verify and configure the SPI port to match the selected pin routing, especially when alternate port options are enabled.
         */
        #define SPI_PORT PORTA
    #endif

    #ifndef SPI_MOSI
        /**
         * @def SPI_MOSI
         * @brief Specifies the bit mask for the SPI MOSI (Master Out Slave In) pin.
         *
         * @details
         * This macro defines the pin mask used to identify the `MOSI` signal in SPI operations. The `MOSI` pin transmits data from the master to the slave device in SPI communication. By default, `SPI_MOSI` is set to `PIN1_bm`. Override this macro before including the SPI module if a different pin assignment for MOSI is needed.
         *
         * @note Ensure this value matches the physical pin layout for your hardware and alternate SPI port selection if enabled.
         */
        #define SPI_MOSI PIN1_bm
    #endif
    
    #ifndef SPI_MISO
        /**
         * @def SPI_MISO
         * @brief Specifies the bit mask for the SPI MISO (Master In Slave Out) pin.
         *
         * @details
         * This macro defines the pin mask used to identify the `MISO` signal in SPI operations. The `MISO` pin is used for data transmission from the slave to the master device in SPI communication. By default, `SPI_MISO` is set to `PIN2_bm`. Override this macro before including the SPI module if alternate pin assignment enabled for `MISO` is needed.
         *
         * @note Ensure this value matches your hardware configuration and any alternate SPI port selection.
         */
        #define SPI_MISO SET_PIN(2, _bm)

        #ifndef SPI_MISO_PINCTRL
            /**
             * @def SPI_MISO_PINCTRL
             * @brief Specifies the pin control register for the SPI MISO (Master In Slave Out) pin.
             *
             * @details
             * This macro defines the pin control configuration register associated with the `MISO` signal in SPI communication. The `MISO` pin control register is used to configure settings such as pull-up resistors, invertion of input and interrupt triggering. By default, `SPI_MISO_PINCTRL` is set to `PIN2CTRL`. Override this macro before including the SPI module if a different pin control configuration is needed.
             *
             * @note Ensure the pin control register matches the selected hardware SPI port and pin mapping for reliable communication.
             */
            #define SPI_MISO_PINCTRL SET_PIN(2, CTRL)
        #endif
    #endif

    #ifndef SPI_SCK
        /**
         * @def SPI_SCK
         * @brief Specifies the bit mask for the SPI SCK (Serial Clock) pin.
         *
         * @details
         * This macro defines the pin mask used to identify the `SCK` (Serial Clock) signal in SPI operations. The `SCK` pin provides the clock signal used to synchronize data transfer between the master and slave devices. By default, `SPI_SCK` is set to `PIN3_bm`. Override this macro before including the SPI module if a different pin assignment for SCK is required.
         *
         * @note Ensure this value matches your hardware configuration and any alternate SPI port selection for reliable communication.
         */
        #define SPI_SCK PIN3_bm
    #endif

    #ifndef SPI_SS
        #define SPI_SS SET_PIN(4, _bm)

        #ifndef SPI_ENABLE_MASTER_ABORT
            /**
             * @def SPI_ENABLE_MASTER_ABORT
             * @brief Enables the SPI master abort feature.
             *
             * @details
             * This macro activates the SPI master abort functionality, which provides a mechanism to safely initialize the master mode. If not defined elsewhere, this macro is automatically defined, along with the `SPI_SS_PINCTRL` macro, which specifies the pin control register for the SPI Slave Select (SS) pin. The `SPI_SS_PINCTRL` macro defines settings such as pull-up resistors, input inversion, and interrupt triggering on the SS pin,
             * ensuring reliable communication and proper SPI master abort behavior on initialization.
             *
             * @note Override `SPI_ENABLE_MASTER_ABORT` and `SPI_SS_PINCTRL` in project configuration if custom behavior or pin settings are required.
             *
             * @see SPI_SS_PINCTRL for SS pin control configuration.
             */
            #define SPI_ENABLE_MASTER_ABORT

            #ifndef SPI_SS_PINCTRL
                /**
                 * @def SPI_SS_PINCTRL
                 * @brief Specifies the pin control register for the SPI SS (Slave Select) pin.
                 *
                 * @details
                 * This macro defines the pin control configuration register associated with the `SS` signal in SPI communication. The `SS` pin control register is used to configure settings such as pull-up resistors, invertion of input and interrupt triggering. By default, `SPI_SS_PINCTRL` is set to `PIN4CTRL`. Override this macro before including the SPI module if a different pin control configuration is needed.
                 *
                 * @note Ensure the pin control register matches the selected hardware SPI port and pin mapping for reliable communication.
                 */
                #define SPI_SS_PINCTRL SET_PIN(4, CTRL)
            #endif
        #endif
    #endif

    #ifndef SPI_SPIE
        /**
         * @def SPI_SPIE
         * @brief Enable or disable SPI interrupt-based processing.
         *
         * @details
         * This macro controls whether the SPI processing uses interrupts or polling. When `SPI_SPIE` is defined, SPI operation is expected to use interrupt-driven processing. If it is undefined or commented out, the SPI module uses polling for data transfer completion. The current library implementation does **not** include interrupt handling. If interrupts are enabled by defining this macro, polling should be disabled by commenting out or undefining this macro to avoid conflicts.
         *
         * @note Interrupts and polling should not be used simultaneously. Uncomment or define this macro to enable interrupt-based SPI handling,or comment it out to use polling-based SPI processing.
         */
        // #define SPI_SPIE
    #endif

    #include <avr/io.h>
    #include "../../../common/enums/SPI_enums.h"
    #include "../../../common/macros/PORT_macros.h"

    SPI_Status spi_init(SPI_Direction direction, SPI_Polarity setup, SPI_Phase sample);
    void spi_disable(void);
    void spi_select(SPI_Select mode);

    #ifndef SPI_SPIE
	    unsigned char spi_transfer(unsigned char data);
    #endif

#endif /* SPI_H_ */