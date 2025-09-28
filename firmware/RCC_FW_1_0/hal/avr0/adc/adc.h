/**
 * @file adc.h
 * @brief ADC (Analog-to-Digital Converter) configuration and control for AVR microcontrollers.
 *
 * This header file defines macros, enums, and function prototypes to configure and control the ADC peripheral of AVR microcontrollers. It allows the setup of ADC resolution, reference voltage, prescaler, sample accumulation, and channel selection. The ADC supports customizable parameters including sample delay, sample length, and reference voltage selection. Interrupt handling is not included in this library and should be implemented separately if needed.
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

#ifndef ADC_H_
#define ADC_H_

    #ifndef ADC_RESOLUTION
        /**
         * @def ADC_RESOLUTION
         * @brief Defines the resolution of the ADC conversion.
         *
         * @details
         * This macro selects the ADC resolution mode for the AVR microcontroller's ADC.
         * The available options are:
         * - `ADC_RESSEL_10BIT_gc` for 10-bit resolution (default)
         * - `ADC_RESSEL_8BIT_gc` for 8-bit resolution
         *
         * The resolution affects the precision and size of the ADC conversion result. A higher resolution provides finer granularity but may require longer conversion times.
         *
         * @note Choose the resolution based on the application's precision requirements and system constraints.
         */
        #define ADC_RESOLUTION ADC_RESSEL_10BIT_gc
    #endif

    #ifndef ADC_CAPACITANCE
        /**
         * @def ADC_CAPACITANCE
         * @brief Configures the ADC input capacitance setting for the AVR microcontroller.
         *
         * @details
         * This macro selects the recommended capacitor setting connected to the ADC input reference pin based on the reference voltage level used. The options are:
         * - `0`: Recommended for reference voltages below 1V
         * - `1`: Recommended for higher reference voltages (default)
         *
         * Proper capacitance selection can improve ADC stability and accuracy by stabilizing the reference voltage.
         *
         * @note Ensure that the physical capacitor used on the ADC reference pin matches this setting for optimal performance.
         */
        #define ADC_CAPACITANCE 1
    #endif

    #ifndef ADC_REFERENCE
        /**
         * @def ADC_REFERENCE
         * @brief Selects the reference voltage source for the ADC conversions.
         *
         * @details
         * This macro defines the reference voltage used by the ADC for analog-to-digital conversions.
         * Options typically include:
         * - `ADC_REFSEL_INTREF_gc`: Internal reference voltage (default)
         * - `ADC_REFSEL_VDDREF_gc`: VDD (power supply) as reference voltage
         *
         * The chosen reference voltage affects the conversion range and accuracy. The internal reference provides a stable known voltage, whereas VDD varies with supply voltage.
         *
         * @note Ensure the selected reference voltage corresponds to the hardware setup for correct ADC readings.
         */
        #define ADC_REFERENCE ADC_REFSEL_INTREF_gc 
    #endif

    #if ADC_REFERENCE == ADC_REFSEL_INTREF_gc
        #ifndef VREF_REFSEL
            /**
             * @def VREF_REFSEL
             * @brief Selects the internal reference voltage level for the ADC when using the internal reference.
             *
             * @details
             * This macro defines the exact internal voltage reference selected for the ADC when the `ADC_REFERENCE` macro is set to `ADC_REFSEL_INTREF_gc` (internal reference).
             * Possible options include:
             * - `VREF_ADC0REFSEL_0V55_gc`  : `0.55 V` internal reference
             * - `VREF_ADC0REFSEL_1V1_gc`   : `1.1 V` internal reference
             * - `VREF_ADC0REFSEL_1V5_gc`   : `1.5 V` internal reference (default)
             * - `VREF_ADC0REFSEL_2V5_gc`   : `2.5 V` internal reference
             *
             * Proper selection of the internal reference voltage is critical for accurate ADC conversions and must match the hardware configuration and application requirements.
             *
             * @note This macro is only defined and used if `ADC_REFERENCE` is set to the internal reference.
             */
            #define VREF_REFSEL VREF_ADC0REFSEL_1V5_gc
        #endif
    #endif

    #ifndef ADC_PRESCALER
        /**
         * @def ADC_PRESCALER
         * @brief Sets the division factor for the ADC input clock derived from the system clock.
         *
         * @details
         * This macro defines the prescaler setting that divides the system clock (`F_PER`) to generate the ADC clock frequency. The ADC requires an input clock frequency typically between `50 kHz` and `1.5 MHz` for accurate conversions, especially when using `10-bit` resolution.
         *
         * Valid prescaler options and their division factors include:
         * - `ADC_PRESC_DIV2_gc`   : `F_PER / 2`
         * - `ADC_PRESC_DIV4_gc`   : `F_PER / 4`
         * - `ADC_PRESC_DIV8_gc`   : `F_PER / 8`
         * - `ADC_PRESC_DIV16_gc`  : `F_PER / 16`
         * - `ADC_PRESC_DIV32_gc`  : `F_PER / 32`
         * - `ADC_PRESC_DIV64_gc`  : `F_PER / 64`
         * - `ADC_PRESC_DIV128_gc` : `F_PER / 128`
         * - `ADC_PRESC_DIV256_gc` : `F_PER / 256` (default)
         *
         * Choosing the appropriate prescaler ensures the ADC clock frequency lies within the recommended range, balancing conversion speed and accuracy.
         *
         * @note Incorrect prescaler settings may lead to inaccurate ADC readings or failure to meet timing requirements.
         */
        #define ADC_PRESCALER ADC_PRESC_DIV256_gc
    #endif

    #ifndef ADC_INIT_DELAY
        /**
         * @def ADC_INIT_DELAY
         * @brief Defines the ADC clock delay before starting the conversion.
         *
         * @details
         * This macro sets the number of ADC clock cycles to wait after enabling the ADC and before starting
         * a conversion. The delay allows the ADC internal circuitry to stabilize, improving conversion accuracy.
         * Possible values with corresponding clock cycles include:
         * - `ADC_INITDLY_DLY0_gc`   : `0 cycles` (no delay)
         * - `ADC_INITDLY_DLY16_gc`  : `16 cycles`
         * - `ADC_INITDLY_DLY32_gc`  : `32 cycles`
         * - `ADC_INITDLY_DLY64_gc`  : `64 cycles`
         * - `ADC_INITDLY_DLY128_gc` : `128 cycles`
         * - `ADC_INITDLY_DLY256_gc` : `256 cycles`
         *
         * The default value is `ADC_INITDLY_DLY0_gc` (no delay).
         *
         * @note Proper delay selection may depend on the ADC configuration and the application requirements. Too small a delay may cause inaccurate conversions.
         */
        #define ADC_INIT_DELAY ADC_INITDLY_DLY0_gc
    #endif

    #ifndef ADC_SAMPLE_DELAY_VARIATION
        /**
         * @def ADC_SAMPLE_DELAY_VARIATION
         * @brief Enables or disables automatic sampling delay variation for the ADC.
         *
         * @details
         * This macro configures whether the ADC hardware automatically varies the delay between sample acquisitions.
         * Options include:
         * - `ADC_ASDV_ASVOFF_gc` : Automatic sampling delay variation disabled (default)
         * - `ADC_ASDV_ASVON_gc`  : Automatic sampling delay variation enabled
         *
         * Enabling this feature randomizes the sampling delay slightly between consecutive ADC conversions, which can help reduce noise and interference effects such as harmonic disturbances in the analog signal.
         *
         * @note This feature is especially useful in applications where periodic noise patterns interfere with accurate ADC readings. Disabling it uses a fixed sample delay.
         */
        #define ADC_SAMPLE_DELAY_VARIATION ADC_ASDV_ASVOFF_gc
    #endif

    #ifndef ADC_SAMPLE_DELAY
        /**
         * @def ADC_SAMPLE_DELAY
         * @brief Configures the delay time between individual ADC sample acquisitions.
         *
         * @details
         * This macro defines the number of clock cycles to wait between samples during ADC conversion. Valid values range from `0` to `15` clock cycles. The default value is set to `8` cycles. Adjusting this delay allows tuning of the ADC sampling frequency and can help reduce noise or interference effects.
         *
         * @note The optimal delay depends on the specific ADC hardware and application requirements.
         */
        #define ADC_SAMPLE_DELAY 8
    #endif

    #ifndef ADC_SAMPLE_LENGTH
        /**
         * @def ADC_SAMPLE_LENGTH
         * @brief Specifies the sample length duration for ADC conversions.
         *
         * @details
         * This macro defines the amount of time, in ADC clock cycles, that the ADC samples the input signal before starting the conversion. Valid values range from `0` to `31` cycles. The default value is set to `8` cycles. Longer sample lengths can improve measurement stability and accuracy, especially when measuring high-impedance sources.
         *
         * @note Adjust this setting based on the source impedance and noise characteristics of the signal.
         */
        #define ADC_SAMPLE_LENGTH 8
    #endif

    #ifndef ADC_ADIE
        /**
         * @def ADC_ADIE
         * @brief Defines whether ADC interrupt is enabled or disabled in the library.
         *
         * @details
         * This macro controls the inclusion of ADC interrupt support in the ADC driver.
         * - If defined (default), ADC interrupt handling is assumed to be enabled outside this library.
         * - If not defined or left empty, ADC operations use polling method instead of interrupts.
         *
         * The library itself does not include or implement interrupt service routines (ISR) for the ADC. Users must provide the ISR implementation and vector placement externally, usually in the main application file.
         *
         * @note For interrupt-based ADC operation, ensure that the ADC interrupt enable bit (ADIE) and global interrupt enable (I-bit in SREG) are properly set and that the ISR for the ADC interrupt vector is implemented.
         */
        //#define ADC_ADIE
    #endif

    #include <avr/io.h>

    /**
     * @enum ADC_Accumulation_t
     * @brief Defines the number of ADC samples to be accumulated and averaged.
     *
     * @details
     * This enumeration provides options for accumulating multiple ADC samples into a single averaged result. Accumulation helps reduce noise and increase measurement accuracy by averaging multiple samples.
     *
     * Values correspond to the number of samples used in accumulation:
     * - `ADC_Sample_None` (1 sample)
     * - `ADC_Sample_2`  (2 samples)
     * - `ADC_Sample_4`  (4 samples)
     * - `ADC_Sample_8`  (8 samples)
     * - `ADC_Sample_16` (16 samples)
     * - `ADC_Sample_32` (32 samples)
     * - `ADC_Sample_64` (64 samples)
     */
    enum ADC_Accumulation_t
    {
        ADC_Sample_None=ADC_SAMPNUM_ACC1_gc,
        ADC_Sample_2=ADC_SAMPNUM_ACC2_gc,
        ADC_Sample_4=ADC_SAMPNUM_ACC4_gc,
        ADC_Sample_8=ADC_SAMPNUM_ACC8_gc,
        ADC_Sample_16=ADC_SAMPNUM_ACC16_gc,
        ADC_Sample_32=ADC_SAMPNUM_ACC32_gc,
        ADC_Sample_64=ADC_SAMPNUM_ACC64_gc
    };

    /**
     * @typedef ADC_Accumulation
     * @brief Alias for enum ADC_Accumulation_t representing ADC sample accumulation modes.
     */
    typedef enum ADC_Accumulation_t ADC_Accumulation;

    /**
     * @enum ADC_Channel_t
     * @brief Selects the ADC input channel.
     *
     * @details
     * This enumeration represents the available analog input channels for the ADC on the AVR microcontroller. It includes external analog input pins, internal reference voltages, and ground.
     *
     * Channels include:
     * - `ADC_CH0` to `ADC_CH11` : External ADC input pins `AIN0` to `AIN11`
     * - `ADC_INTREF`            : Internal reference voltage input
     * - `ADC_GND`               : Ground reference input
     */
    enum ADC_Channel_t
    {
        ADC_CH0=ADC_MUXPOS_AIN0_gc,
        ADC_CH1=ADC_MUXPOS_AIN1_gc,
        ADC_CH2=ADC_MUXPOS_AIN2_gc,
        ADC_CH3=ADC_MUXPOS_AIN3_gc,
        ADC_CH4=ADC_MUXPOS_AIN4_gc,
        ADC_CH5=ADC_MUXPOS_AIN5_gc,
        ADC_CH6=ADC_MUXPOS_AIN6_gc,
        ADC_CH7=ADC_MUXPOS_AIN7_gc,
        ADC_CH8=ADC_MUXPOS_AIN8_gc,
        ADC_CH9=ADC_MUXPOS_AIN9_gc,
        ADC_CH10=ADC_MUXPOS_AIN10_gc,
        ADC_CH11=ADC_MUXPOS_AIN11_gc,
        ADC_INTREF=ADC_MUXPOS_INTREF_gc,
        ADC_GND=ADC_MUXPOS_GND_gc
    };

    /**
     * @typedef ADC_Channel
     * @brief Alias for enum ADC_Channel_t representing ADC input channel selections.
     */
    typedef enum ADC_Channel_t ADC_Channel;

                void adc_init(void);
                void adc_channel(ADC_Channel channel);
                void adc_accumulation(ADC_Accumulation samples);
                void adc_disable(void);
            
    #ifndef ADC_ADIE
        unsigned int adc_read(void);
        unsigned int adc_average(unsigned char samples);
    #endif

#endif /* ADC_H_ */