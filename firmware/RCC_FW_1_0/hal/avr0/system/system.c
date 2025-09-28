/**
 * @file system.c
 * @brief Source file with implementation of system clock initialization.
 * 
 * This file contains the implementation of the system initialization routine, which configures the main system clock, optional prescaler, and oscillator standby behavior for AVR microcontrollers.
 * 
 * Functionality includes:
 * - Switching to a selected system clock source
 * - Waiting until the clock source is stable
 * - Optional configuration of the peripheral clock prescaler
 * - Enabling standby operation for the internal 20 MHz oscillator
 * 
 * @author g.raf
 * @date 2025-09-18
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 * 
 * @note This file is part of a larger embedded systems project and subject to the license specified in the repository. For updates and the complete revision history, see the GitHub repository.
 * 
 * @see https://github.com/0x007e/avr0 "AVR GitHub Repository"
 */

#include "system.h"

/**
 * @brief Initializes the system clock configuration of the microcontroller.
 *
 * @details
 * This function configures and enables the main system clock source and (optionally) the peripheral clock prescaler. The initialization sequence includes:
 *
 * 1. Writing the selected system clock source (`SYSTEM_CLOCK`) to `CLKCTRL.MCLKCTRLA`.  
 * 2. Waiting until the new clock source is active and stable, by checking `CLKCTRL.MCLKSTATUS` against `SYSTEM_CLOCK_BIT`.  
 * 3. If `SYSTEM_PER_CLOCK_PRESCALER` is defined, configuring the peripheral clock division ratio via `CLKCTRL.MCLKCTRLB`.  
 * 4. If the selected system clock is the internal 20?MHz oscillator (`CLKCTRL_CLKSEL_OSC20M_gc`), enabling the oscillator in standby mode by setting `CLKCTRL.OSC20MCTRLA`.  
 *
 * @note
 * - The function makes use of the **Configuration Change Protection** (CCP) mechanism (`CCP = CCP_IOREG_gc`) before writing to protected `CLKCTRL` registers.  
 * - Changing the system clock affects all timing-sensitive peripherals (UART, timers, ADC clock, etc.).  
 * - If no prescaler (`SYSTEM_PER_CLOCK_PRESCALER`) is defined, the peripheral clock will run at `F_CPU / 1`.  
 *
 * @warning This function should be called **once during system startup** before any timing-dependent peripherals are initialized.
 */
void system_init(void)
{
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA = SYSTEM_CLOCK;
    
    while(!(CLKCTRL.MCLKSTATUS & SYSTEM_CLOCK_BIT))
    {
        asm volatile("NOP");
    }
    
    #ifdef SYSTEM_PER_CLOCK_PRESCALER
        CCP = CCP_IOREG_gc;
        CLKCTRL.MCLKCTRLB = SYSTEM_PER_CLOCK_PRESCALER | CLKCTRL_PEN_bm;
    #endif
    
    CCP = CCP_IOREG_gc;
    #if SYSTEM_CLOCK == CLKCTRL_CLKSEL_OSC20M_gc
        CLKCTRL.OSC20MCTRLA = CLKCTRL_RUNSTDBY_bm;
    #elif SYSTEM_CLOCK == CLKCTRL_CLKSEL_OSCULP32K_gc
        CLKCTRL.OSC32KCTRLA = CLKCTRL_RUNSTDBY_bm;
    #else
        #error "No system clock defined"
    #endif
}

