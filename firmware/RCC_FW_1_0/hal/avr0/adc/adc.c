/**
 * @file adc.c
 * @brief AVR ADC driver implementation for initialization, configuration, and reading.
 *
 * This source file provides functions to initialize the ADC peripheral on AVR microcontrollers,
 * configure ADC input channels and sample accumulation, enable or disable the ADC,
 * and read ADC conversion results either by polling or averaging multiple samples.
 *
 * It supports configuration of ADC reference voltage, resolution, prescaler, sample delays, and accumulation modes.
 * Interrupt-driven ADC operation is supported externally through user-defined interrupt handlers.
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
 * @see https://github.com/0x007e/avr0 "AVR GitHub Repository"
 */

#include "adc.h"

/**
 * @brief Initialize the ADC peripheral with pre-configured settings.
 *
 * @details
 * Configures the ADC control registers for capacitance, reference voltage, prescaler, sample delay variation, initial delay, and sample length according to compile-time macros. Enables the ADC with the selected resolution. If the ADC interrupt mode is enabled, the ADC result ready interrupt is also enabled. When using the internal voltage reference, the voltage reference control register is configured accordingly. This function must be called before starting any ADC conversions to ensure proper setup.
 *
 * @note Interrupt vectors and ADC interrupt handling routines must be implemented separately when enabling interrupts.
 */
void adc_init(void)
{
    ADC0.CTRLC = (ADC_CAPACITANCE<<ADC_SAMPCAP_bp) | ADC_REFERENCE | ADC_PRESCALER;
    ADC0.CTRLD = ADC_SAMPLE_DELAY_VARIATION | ADC_INITDLY_DLY0_gc | (ADC_SAMPLE_DELAY<<ADC_SAMPDLY_gp);
    ADC0.SAMPCTRL = ADC_SAMPLE_LENGTH;
    ADC0.CTRLA = ADC_RESOLUTION | ADC_ENABLE_bm;
    
    // Check if ADC interrupt handler is active
    #ifdef ADC_ADIE
        ADC0.INTCTRL = ADC_RESRDY_bm;   // Enable ADC interrupt
    #endif

    #if ADC_REFERENCE == ADC_REFSEL_INTREF_gc
        VREF.CTRLA = VREF_REFSEL | (VREF.CTRLA & 0x0F);
    #endif
}

/**
 * @brief Disable the ADC module.
 *
 * @details
 * This inline function disables the ADC by clearing the enable bit in the ADC control register. Disabling the ADC conserves power when ADC functionality is not required.
 * 
 * @note It is recommended to disable the ADC before entering low-power modes to minimize current consumption.
 */
inline void adc_disable(void)
{
    ADC0.CTRLA &= ~(ADC_RUNSTBY_bm | ADC_ENABLE_bm);
}

/**
 * @brief Select the ADC input channel.
 *
 * @param channel ADC channel to select from the `ADC_Channel` enumeration.
 *
 * @details
 * This inline function sets the ADC multiplexer positive input selection register to the specified channel, masking and shifting the channel value appropriately. The channel selects which analog input pin or internal reference the ADC samples.
 *
 * @note Ensure the channel value is valid and corresponds to a supported ADC input.
 */
inline void adc_channel(ADC_Channel channel)
{
    ADC0.MUXPOS = ((0x1F & channel)<<ADC_MUXPOS_gp);   // Select ADC Channel
}

/**
 * @brief Set the ADC sample accumulation mode.
 *
 * @param samples The ADC_Accumulation enum value specifying how many ADC samples to accumulate and average.
 *
 * @details
 * This function configures the ADC to accumulate multiple samples before producing a conversion result, which helps reduce noise and improve measurement accuracy. The accumulation count is set by writing to the ADC control register B.
 *
 * @note The ADC must be configured and enabled for the accumulation mode to take effect.
 */
void adc_accumulation(ADC_Accumulation samples)
{
    ADC0.CTRLB = samples;
}

#ifndef ADC_ADIE

    /**
     * @brief Perform a single ADC conversion and return the result (polling mode).
     *
     * @details
     * Starts an ADC conversion by setting the START command bit in the ADC command register. The function then polls the command register until the conversion completes by checking the START bit. After completion, it returns the 16-bit (10-bit ADC result) value from the ADC result register.
     *
     * @note This function blocks execution until the conversion is finished.
     *
     * @return The ADC conversion result as an unsigned int.
     */
    unsigned int adc_read(void)
    {
        ADC0.COMMAND = ADC_STARTEI_bm;

        while(ADC0.COMMAND & ADC_STARTEI_bm)
        {
            ;
        }
        
        return ADC0.RES;
    }

    /**
     * @brief Perform multiple ADC conversions and return the average result (calculated in software).
     *
     * @param samples Number of ADC samples to average.
     *
     * @details
     * This function reads the ADC conversion result multiple times by calling adc_read(), sums the results, and calculates the average value to reduce noise and improve stability. It returns the averaged ADC value as an unsigned int.
     *
     * @note This method provides a simple software averaging filter for ADC measurements.
     *
     * @return The averaged ADC conversion result.
     */
    unsigned int adc_average(unsigned char samples)
    {
        unsigned long average = 0;
        
        for(unsigned char i=0; i < samples; i++)
        {
                average += adc_read();
        }
        
        average /= samples;
        
        return (unsigned int)(average);
    }

#endif