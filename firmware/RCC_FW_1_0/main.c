
#define F_CPU 20000000UL
#define SWITCH PIN7_bm
#define SWITCH_COMMAND_EXECUTE_MS 3000UL
#define SWITCH_SYSTEM_OFF_TIME_MS 3000UL
#define COLOR_FADE_DELAY_MS 10UL
//#define ENABLE_EEPROM_WRITE

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "./hal/avr0/system/system.h"
#include "./battery/battery.h"
#include "./led/led.h"

static unsigned int switch_count = 0UL;

char EEMEM description[] = "RCC Firmware v1.0 by";
char EEMEM author[] = "R. GAECHTER";
char EEMEM copyright[] = "Copyright 2025 g.raf engineering";
char EEMEM github[] = "github.com/0x007e/rcc";

LED_Data EEMEM ee_led1  = {
	0x01,
	0x0F,
	0x00,
	0x0A
};

LED_Data EEMEM ee_led2  = {
	0x01,
	0x00,
	0x0F,
	0x0A
};

LED_Data led1  = {
	0x01,
	0x0F,
	0x00,
	0x0A
};

LED_Data led2  = {
	0x01,
	0x00,
	0x0F,
	0x0A
};

ISR(PORTA_PORT_vect)
{	
	PORTA.INTFLAGS = PORT_INT_7_bm;
}

volatile unsigned long systick;

ISR(TCA0_OVF_vect)
{
    systick++;
}

static void timer_init(void)
{	
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = 0x0270;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm;
}

static void timer_disable(void)
{
	TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.CTRLA = ~(TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm);
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

static void system_shutdown(void)
{
    // System Shutdown
    timer_disable();
    battery_disable();
    led_disable();

    PORTA.DIRCLR = PIN7_bm;
    PORTA.PIN7CTRL =  PORT_ISC_BOTHEDGES_gc;
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
					led_blink(LED_Position_Left, led_status_color(LED_Status_Ready, LED_MIN_INTENSITY), LED_Delay_MS_500, 1);
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

					while(PORTA.IN & SWITCH);
				}
				_delay_ms(10);

			} while ((systick - last_button_press) < SWITCH_COMMAND_EXECUTE_MS);
			
			switch (switch_count)
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
    					_delay_ms(COLOR_FADE_DELAY_MS);
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

			execute_command = 0;
		}
    }
}