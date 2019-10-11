/*
 * 01_Blink_LED_02_0125Hz.c
 *
 * Created: 1/27/2019 7:10:47 PM
 * Author : Anthony
 */ 

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/builtins.h>
#include "manipulationbits.h"


#define DELAY_4_SEC		64000000  // 4 sec @ 16Mhz. 16Mhz/64M = 0.25Hz ; 1/0.25Hz = 4 sec


void system_setup(void)
{                                    ///////////////////////////////    OUT SETTING    ///////////////////////////////
	// Set PC.4 as output.
	DDRC = (1<<DDC4); // 0010 0000 (1<<4)
	
	//Set PC.4 initial Low ('0')
    PORTC =  BIT_OFF(PORTC,4); // 0000 0000
}

int main(void)
{
	// Initial system.
	system_setup();
	
	while (1)
	{
		__builtin_avr_delay_cycles(DELAY_4_SEC); // Wait 4 second.
		//Toggle LED
		BIT_TOGGLE (PORTC,4);      // 4 sec on and 4 sec off , so T = 8 , f = 1/T = 1/8 = 0.125 Hz. LED Blinking at 0.125 Hz.
		
		
	}
}
