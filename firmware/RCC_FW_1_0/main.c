/**
 * @file main.c
 * @brief Main firmware file for RCC RGB LED Color Cube on AVR.
 *
 * This source file implements the main control flow and event handling for the RCC embedded system. It manages system initialization, LEDs and battery management, interrupt handling, EEPROM operations, and execution of user commands triggered by button inputs. Key functionalities include timed LED blinking, system-wide power management including shutdown and restart logic, and processing of button press events for dynamic LED control.
 *
 * @author g.raf
 * @date 2025-12-10
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2025 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 *
 * @note This file is part of a larger AVR embedded project subject to the license specified in the repository. For updates and complete revision history, see the GitHub repository.
 *
 * @see https://github.com/0x007e/rcc "RCC - RGB LED Color Cube"
 */

#include "main.h"

static unsigned int switch_count = 0UL;

char EEMEM description[] = "RCC Firmware v1.0 by";
char EEMEM author[] = "R. GAECHTER";
char EEMEM copyright[] = "Copyright 2025 g.raf engineering";
char EEMEM github[] = "github.com/0x007e/rcc";

LED_Data EEMEM ee_led1  = {
	0x03,
	0x00,
	0xFF,
	0xFF
};

LED_Data EEMEM ee_led2  = {
	0x03,
	0xFF,
	0x00,
	0xFF
};

LED_Data led1, led2;

/**
 * @brief Interrupt Service Routine for PORTA pin change or event.
 *
 * This ISR handles the interrupt triggered by an event on PORTA pins. In this implementation, it only clears the interrupt flag for PIN7 on PORTA to acknowledge and allow further interrupts. This prevents the interrupt from continuously retriggering.
 *
 * @note The ISR is registered for the PORTA interrupt vector and is executed automatically upon the corresponding hardware interrupt and is necessary for system wakeup after deep sleep.
 */
ISR(PORTA_PORT_vect)
{	
	PORTA.INTFLAGS = PORT_INT_7_bm;
}

volatile unsigned long systick;

/**
 * @brief Timer/Counter Overflow Interrupt Service Routine.
 *
 * This ISR is called when the Timer/Counter overflows. It increments the global millisecond tick counter `systick` used for system timing. The interrupt flag for the overflow is cleared to allow subsequent interrupts.
 *
 * @note Ensure the timer is properly configured and overflow interrupts are enabled for this routine to be executed correctly.
 */
ISR(TCA0_OVF_vect)
{
    systick++;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

/**
 * @brief Initializes Timer/Counter in single mode with overflow interrupt.
 *
 * This function configures the TCA0 timer as a 16-bit timer operating in single mode. It sets the overflow interrupt enable bit, loads the period register with a predefined value, and starts the timer with a clock prescaler of divide-by-8.
 *
 * @details
 * The timer will generate an interrupt when the counter overflows at the value in the PER register. The overflow interrupt is enabled to allow time-based events or system ticks. The clock source selection and enabling the timer starts the counting immediately.
 */
static void timer_init(void)
{	
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = 0x04E1;
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm;
}

/**
 * @brief Disables Timer/Counter and clears its overflow interrupt.
 *
 * This function disables the overflow interrupt for Timer/Counter TCA0, stops the timer by clearing the clock select and enable bits, and clears any pending overflow interrupt flags.
 *
 * @details
 * Disabling the timer halts counting and prevents further interrupt triggers. The interrupt flag is cleared to avoid immediately re-entering the ISR. This should be called when timer-based operations need to be stopped safely.
 */
static void timer_disable(void)
{
	TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.CTRLA = ~(TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm);
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

/**
 * @brief Performs a controlled system shutdown and restarts the microcontroller.
 *
 * This function disables timers, battery and LED subsystems, configures PORTA pins for low power, sets the microcontroller into the power-down sleep mode to minimize energy consumption, and then triggers a software reset to restart the system.
 *
 * @details
 * - Disables the Timer/Counter to stop timing interrupts
 * - Powers down battery and LED to save energy
 * - Configures PORTA pins as inputs with pull-ups or specific sensing modes
 * - Puts the CPU to sleep in the deepest power save mode (PWR_DOWN)
 * - After waking up (unlikely unless interrupt occurs), disables sleep and interrupts
 * - Triggers a software reset via the Reset Control register (RSTCTRL)
 *
 * This ensures a safe and clean shutdown and restart sequence on AVR microcontrollers.
 */
static void system_shutdown(void)
{
    // System Shutdown
    timer_disable();
    battery_disable();
    led_disable();

    PORTA.DIRCLR = PIN7_bm | PIN6_bm | PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
    PORTA.PIN7CTRL =  PORT_ISC_BOTHEDGES_gc;
    PORTA.PIN6CTRL =  PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL =  PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL =  PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL =  PORT_PULLUPEN_bm;
    PORTA.PIN0CTRL =  PORT_PULLUPEN_bm;
    PORTA.INTFLAGS = PORT_INT_7_bm;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();
    sleep_cpu();

    sleep_disable();
    cli();

    // Restart System
    CCP = CCP_IOREG_gc;
    RSTCTRL.SWRR = RSTCTRL_SWRE_bm;
}

static unsigned long last_button_press;
static unsigned char execute_command;

int main(void)
{
    system_init();
    led_init();
	battery_init();
	
    if(battery_status() == BATTERY_Ok)
    {
        led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_200, 2);
    }
    else
    {
        led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Error, LED_MIN_INTENSITY), LED_Delay_MS_200, 2);
    }

	battery_disable();
	timer_init();
	sei();
	
	// read LED data from EEPROM
    eeprom_read_block(&led1, &ee_led1, sizeof(LED_Data));
    eeprom_read_block(&led2, &ee_led2, sizeof(LED_Data));
	
    while (1)
    {	
        LED_SOF();
        led_data(led1);
        led_data(led2);
        LED_EOF();

		if(PORTA.IN & SWITCH)
		{
			led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_100, 0);
			
			switch_count++;
			last_button_press = systick;
			
			while(PORTA.IN & SWITCH)
			{
				if((systick - last_button_press) > SWITCH_SYSTEM_OFF_TIME_MS)
				{
					led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Ready,   LED_MIN_INTENSITY), LED_Delay_MS_500, 0);
					led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Warning, LED_MIN_INTENSITY), LED_Delay_MS_500, 0);
					led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Error,   LED_MIN_INTENSITY), LED_Delay_MS_500, 0);
					system_shutdown();
				}
			}
		}

        if((switch_count > 0) && ((systick - last_button_press) > SWITCH_COMMAND_EXECUTE_MS))
        {
            execute_command = switch_count;
            switch_count = 0;
        }
		
		if(execute_command)
		{	
			LED_Position position = LED_Position_Left;
			LED_Data *led;
			do 
			{
				if(position == LED_Position_Left)
				{
                    led = &led1;
                    led_blink(LED_Position_Left, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_500, 1);
				}
				else
				{
    				led = &led2;
					led_blink(LED_Position_Right, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_500, 1);
				}
				
				if(PORTA.IN & SWITCH)
				{
					_delay_ms(10);

                    if(position == LED_Position_Left)
                    {
                        position = LED_Position_Right;
                    }
                    else
                    {
                        position = LED_Position_Left;
                    }
                    last_button_press = systick;
				}

			} while ((systick - last_button_press) < SWITCH_COMMAND_EXECUTE_MS);
			
			switch (execute_command)
			{
				case 1:
					while(!(PORTA.IN & SWITCH))
					{
                        led->red++;
                        led_color(position, *led);
                        _delay_ms(COLOR_FADE_DELAY_MS);
					}
                    break;
				case 2:
				    while(!(PORTA.IN & SWITCH))
				    {
    				    led->green++;
    				    led_color(position, *led);
    				    _delay_ms(COLOR_FADE_DELAY_MS);
				    }
                    break;
				case 3:
					while(!(PORTA.IN & SWITCH))
					{
    					led->blue++;
    					led_color(position, *led);
    					_delay_ms(COLOR_FADE_DELAY_MS);
					}
                    break;
				case 4:
					while(!(PORTA.IN & SWITCH))
					{
    					led->intensity++;

                        if(led->intensity > LED_MAX_INTENSITY)
                        {
                            led->intensity = LED_MIN_INTENSITY;
                        }

    					led_color(position, *led);
    					_delay_ms(COLOR_INTENSITY_DELAY_MS);
					}
                    break;
				default:
					led_blink(LED_Position_Left | LED_Position_Right, led_status_color(LED_Status_Error, LED_MIN_INTENSITY), LED_Delay_MS_500, 4);
				break;
			}

            #ifdef ENABLE_EEPROM_WRITE
                if(position == LED_Position_Left)
                {
                    eeprom_write_block(&led1, &ee_led1, sizeof(LED_Data));
                }
                else
                {
                    eeprom_write_block(&led2, &ee_led2, sizeof(LED_Data));
                }
            #endif

			led_blink(LED_Position_Left | LED_Position_Right_Alternating, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_500, 2);

			execute_command = 0;
			switch_count = 0;
		}
    }
}