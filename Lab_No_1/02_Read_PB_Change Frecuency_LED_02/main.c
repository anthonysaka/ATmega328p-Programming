/*
 * 02_Read_PB_Change Frecuency_LED_02.c
 *
 * Created: 1/27/2019 7:19:50 PM
 * Author : Anthony
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/builtins.h>
#include "manipulationbits.h"  //I created this library for simplify the coding. I can use functions like BIT_OFF to set low (OFF) a specific bit of a port.

                                                       ///////////// PROTOTYPES FUNCTIONS /////////////	
bool delay_counter_led(uint8_t* counter, uint8_t top_count_sec);

                                                       ///////////// SETTING DELAY AND TOP COUNT /////////////	

#define DELAY_25_mSEC	   400000  // 25 ms delay @ 16 MHz.

#define _050_SEC_COUNT         20  // 0.5 sec @ 25 ms count.
#define  _4_SEC_COUNT		  160  // 4 sec @ 25 ms count.

// I/O Function to general setting initial system.   
void system_setup(void)
{                                        ///////////////////////////////    IN SETTING    ///////////////////////////////
	
	DDRB |= (1<<DDB0);     //Set PB.0 as Input, THE PUSHBUTTON_00 IS CONNECTED.
	PORTB = BIT_OFF(PORTB,0);   //Set low for High Impedance PB.0 -> D8.- 10K RESISTOR CONNECTED.
	
                                           ///////////////////////////////    OUT SETTING    ///////////////////////////////
										   
	DDRC |= (1<<DDC4);  // Set Low PC.4 as output.
	PORTC =  BIT_OFF(PORTC,4); //Set PC.4 initial Low ('0').
}

                   ///// MAIN /////
int main(void)
{
	// Initial system
	system_setup();
	
	uint8_t blink_delay_count_05 = 0; //Variable to store count to 05 sec (20 counts @ 25 mSec).
    uint8_t blink_delay_count_4 = 0; //Variable to store count to 4 sec (160 counts @ 25 mSec).

	while (1)
	{
		if ((PINB & (1 << PINB0)) != 0)  // Checking PIN B (PB.0) is pressed.
		{
	           if (delay_counter_led(&blink_delay_count_05, _050_SEC_COUNT))
	           {
				   blink_delay_count_4 = 0;
				   //Toggle Led_2
				   BIT_TOGGLE(PORTC,4); // 0.5 sec Off and 0.5 sec On , so T = 1 , f = 1/T = 1/1 = 1 Hz. LED Blinking at 1 Hz.
	           }
		}
		else
		{          //This condition is for PIN B (PB.0) isn't pressed.
		    if (delay_counter_led(&blink_delay_count_4, _4_SEC_COUNT))  // Checking time to blink LED_2 (Blue) Connected in PC.4
			{
				blink_delay_count_05 = 0;
				// Toggle Led_2
				BIT_TOGGLE (PORTC,4); // 4 sec Off and 4 sec On , so T = 8 , f = 1/T = 1/8 = 0.125 Hz. LED Blinking at 0.125 Hz.
			}
		}
		__builtin_avr_delay_cycles(DELAY_25_mSEC); // Principal Clock (Master Clock).
	}
}
                 ///// END MAIN /////
				
                                            ///////////////////////////////    FUNCTIONS   ///////////////////////////////
											
//1. Function to delay counter LEDs Sec with Duty Cycle 50%.
bool delay_counter_led (uint8_t* counter, uint8_t top_count_sec) 
{
	if (*counter >= top_count_sec) // Checking  the count has reached top (800 counts = 4sec).
	{
		*counter = 0;  // Reset counter and return bool value True.
		return true;
	}
	else                 //This condition is for the top count (4 sec) not reached.
	{
		*counter = *counter + 1; // if the top count not reached, Increment count.
		return false;
	}
}
