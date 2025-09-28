/**
 * @file spi.c
 * @brief Source file with implementation of hardware SPI functions and macros.
 *
 * This file contains the definitions of function implementations and macros for hardware-based SPI communication on AVR-0/1/2-Series microcontrollers.
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
 * @important The SS pin must be HIGH during initialization of the bus to avoid unintended slave mode configuration.
 * 
 * @see spi.h for declarations and related information.
 * @see https://github.com/0x007e/avr0 "AVR ATmega GitHub Repository"
 */

#include "spi.h"

/**
 * @brief Initialize the SPI hardware interface in master mode with specified configuration.
 *
 * @param direction Specifies the bit order for SPI data transmission (MSB or LSB first).
 * @param setup Specifies the clock polarity (SPI_Polarity) to configure the clock idle state.
 * @param sample Specifies the clock phase (SPI_Phase) to configure the clock sampling edge.
 *
 * @return Returns an SPI_Status code indicating the result of initialization. Possible return values:
 * - SPI_None: Initialization completed successfully.
 * - SPI_Master_Abort: Initialization was aborted due to the Slave Select (SS) pin being low when `SPI_ENABLE_MASTER_ABORT` is enabled.
 *
 * @details
 * This function configures the SPI peripheral registers on the AVR ATTiny series device to initialize the SPI bus in master mode. It sets the SPI port multiplexer, configures the data direction for SPI pins, and sets up the SPI clock phase and polarity according to the specified parameters.
 * 
 * @important If the macro `SPI_ENABLE_MASTER_ABORT` is defined, special handling is enabled: if during initialization the SS pin is detected low, 
 * the SPI master initialization is aborted by disabling the SPI peripheral and returning the status `SPI_Master_Abort`.
 *
 * The function also sets pull-up resistors on the MISO and SS pins according to configuration, and configures SPI interrupts if the `SPI_SPIE` macro is defined.
 *
 * @note Ensure that the SPI pins and port multiplexer settings correspond to your hardware configuration. The function assumes a default SPI clock prescaler; customize `SPI_CLOCK` and `SPI2X_ENABLE` macros for different clock rates.
 *
 * @see SPI_Direction, SPI_Polarity, SPI_Phase for parameter options and configuration.
 * @see spi_disable() for disabling the SPI peripheral in case of master abort.
 */
SPI_Status spi_init(SPI_Direction direction, SPI_Polarity setup, SPI_Phase sample)
{
    PORTMUX.CTRLB &= ~PORTMUX_SPI0_bm;
    PORTMUX.CTRLB |= SPI_PORTMUX;

	PORTA.DIRCLR = SPI_MOSI | SPI_MISO | SPI_SCK;

	#ifdef SPI_ENABLE_MASTER_ABORT
		SPI_PORT.DIRCLR = SPI_SS;
		SPI_PORT.SPI_SS_PINCTRL = PORT_PULLUPEN_bm;
	#else
		SPI_PORT.DIRSET = SPI_SS;
	#endif

	SPI_PORT.SPI_MISO_PINCTRL = PORT_PULLUPEN_bm;
	
	SPI0.CTRLA = SPI_MASTER_bm
	#ifdef SPI2X_ENABLE
		| SPI_CLK2X_bm
	#endif
	
	#ifdef SPI_CLOCK
		| SPI_CLOCK
	#endif
		| (SPI_DORD_bm & (direction<<SPI_DORD_bp));
	
    SPI0_CTRLB &= ~SPI_MODE_gm;
	SPI0.CTRLB |= ((SPI_MODE_1_bm & (setup<<SPI_MODE_1_bp)) | (SPI_MODE_0_bm & (sample<<SPI_MODE_0_bp)));

	#ifdef SPI_SPIE
		SPI0.INTCTRL |= SPI_IE_bm;
	#endif
	
	SPI0.CTRLA |= SPI_ENABLE_bm;

	#ifdef SPI_ENABLE_MASTER_ABORT
		if((SPI0.CTRLA & SPI_MASTER_bm) && (SPI0.INTFLAGS & SPI_IF_bm))
		{
			spi_disable();
			return SPI_Master_Abort;
		}
	#endif
	
	// PORT configuration gets overwritten from SPI controller
	SPI_PORT.DIR |= SPI_MOSI | SPI_MISO | SPI_SCK | SPI_SS;

	return SPI_None;
}

/**
 * @brief Disable the SPI hardware interface and reset related pins.
 *
 * @details
 * This function disables the SPI peripheral on the AVR ATTiny series device by clearing the master and enable bits in the SPI control register. It also resets the SPI mode bits to their default state. The function configures the SPI-related pins (`MOSI`, `MISO`, `SCK`, `SS`) by clearing their direction bits, effectively disabling SPI pin control, and drives these pins low by clearing the output register. Additionally, the pull-up resistor on the SS pin is disabled. If SPI interrupts are enabled via the `SPI_SPIE` macro, this function disables SPI interrupt requests. The port multiplexer configuration for SPI is cleared and the SPI interrupt flags are reset.
 *
 * @note After calling this function, the SPI peripheral and associated pins are fully disabled,
 * which is necessary before changing SPI configurations or powering down the SPI module to reduce power consumption.
 */
void spi_disable(void)
{
	// Disable SPI
	SPI0.CTRLA &= ~(SPI_MASTER_bm | SPI_ENABLE_bm);
    SPI0_CTRLB &= ~SPI_MODE_gm;

	SPI_PORT.DIRCLR = SPI_MOSI | SPI_MISO | SPI_SCK | SPI_SS;
	SPI_PORT.OUTCLR = SPI_MOSI | SPI_MISO | SPI_SCK | SPI_SS;
	SPI_PORT.SPI_SS_PINCTRL = ~PORT_PULLUPEN_bm;
	
	#ifdef SPI_SPIE
		SPI0.INTCTRL &= ~SPI_IE_bm;
	#endif
	
	PORTMUX.CTRLB &= ~PORTMUX_SPI0_bm;
	SPI0.INTFLAGS = SPI_IF_bm;
}

#ifndef SPI_SPIE

	/**
	 * @brief Control the SPI Slave Select (SS) pin to enable or disable the SPI slave device.
	 *
	 * @param mode Specifies the SPI select state, either SPI_Enable or SPI_Disable.
	 *
	 * @details
	 * This function manages the SPI Slave Select (SS) pin using the configured SPI port. When `SPI_Enable` is passed, the `SS` pin is driven low to select (activate) the SPI slave device. For any other value, the `SS` pin is driven high, deselecting (deactivating) the slave. This control is essential for ensuring that only one SPI slave device communicates with the master at any given time on a shared SPI bus.
	 *
	 * @note The SS pin behavior is active low; pulling it low selects the slave. Ensure that the pin configuration and port macros reflect your hardware's SPI setup.
	 */
    void spi_select(SPI_Select mode)
    {
	    switch(mode)
	    {
		    case SPI_Enable : SPI_PORT.OUTCLR = SPI_SS;	break;
		    default         : SPI_PORT.OUTSET = SPI_SS;	break;
	    }
    }

	/**
	 * @brief Transfer a single byte of data over the SPI bus.
	 *
	 * @param data The byte value to be sent via SPI.
	 *
	 * @return Returns the byte received simultaneously from the SPI slave device.
	 *
	 * @details
	 * This function writes the provided data byte to the SPI data register, initiating the SPI transmission. It then waits in a busy loop until the SPI interrupt flag indicates that the transfer is complete. Upon completion, the function reads and returns the received byte from the SPI data register. SPI communication is full-duplex, so while sending a byte, a byte is received simultaneously. This function therefore effectively performs a combined send/receive operation.
	 *
	 * @note This is a blocking call that waits until the SPI hardware signals transmission completion. Ensure SPI is properly initialized before calling this function.
	 */
    unsigned char spi_transfer(unsigned char data)
    {
	    SPI0.DATA = data;

	    while (!(SPI0.INTFLAGS & SPI_IF_bm))
        {
            asm volatile("nop");
        }
	    return SPI0.DATA;
    }

#endif