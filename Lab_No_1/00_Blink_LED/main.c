/*
 * 00_Blink_LED_01.c
 *
 * Created: 1/27/2019 5:22:17 PM
 * Author : Anthony
 */

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/builtins.h>
#include "manipulationbits.h" //I created this library for simplify the coding. I can use functions like BIT_OFF to set low (OFF) a specific bit of a port.

#define DELAY_025_SEC		4000000  // 0.25 sec @ 16Mhz.
#define DELAY_075_SEC		12000000  // 0.75 sec @ 16Mhz.


void system_setup(void)
{                                    ///////////////////////////////    OUT SETTING    ///////////////////////////////
	// Set PC.5 as output
	DDRC = (1<<DDC5); // 0010 0000 (1<<5)
	
	//Set PC.5 initial Low ('0')
	PORTC =  BIT_OFF(PORTC,5); // 0000 0000
}

int main(void)
{
	// Initial system.
	system_setup();
	
	while (1)
	{
		__builtin_avr_delay_cycles(DELAY_025_SEC); // Wait 0.25 sec.
		BIT_TOGGLE (PORTC,5); //Toggle LED_1
		
		__builtin_avr_delay_cycles(DELAY_075_SEC); // Wait 0.75 second
		BIT_TOGGLE (PORTC,5); //Toggle LED_1.
	}
}


