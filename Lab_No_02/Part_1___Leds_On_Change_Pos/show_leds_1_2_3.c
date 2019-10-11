/*
 * show_leds_1.c
 *
 * Created: 2/11/2019 11:05:27 PM
 *  Author: Anthony
 */ 

#include "show_leds_1_2_3.h"
#include "manipulationbit.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>


// 1. This function is for the part one. Allows changing sequentially between led, where it has a lower [led_0] and upper [led_4] top.
void show_leds_1_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable)
{
	
	if (up_stable && down_stable)
	{                                // Don't allow press both button at same time.
		return;
	}

	if (pb_up && *counter_pos < 4) // Checking pushbutton up and counter (4 count), to only do this, if PB up is pressed and the position led is less than 4.
	{
		*counter_pos = *counter_pos + 1; //Increment counter_pos.
		PORTC = (1<<(5 - *counter_pos)); //Left shift to set bit high between ( PC.5 - PC.1) --> (Led[0] - Led[4])
	}
	else if (pb_down  && *counter_pos > 0) // Checking pushbutton down and counter, to only do this, if PB down is pressed and the position led is higher than 0. 
	{
		*counter_pos = *counter_pos - 1; // Decrement counter_pos.
		PORTC = (1<<(5 - *counter_pos)); //Left shift to set bit low between ( PC.5 - PC.1) --> (Led[0] - Led[4])
	}
	else if (*counter_pos==0)
	{                                    //Init Led[0] high. --> PC.5
		PORTC = (1<<(5 - *counter_pos));    //Initially the Led_0 is On.
	}
}

// 2. This function is for the part two. Allows implement a bar led volume (Level).
void show_leds_2_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable)
{	
	
	if (up_stable && down_stable)  // Don't allow press both button at same time.
	{
		return;
	}
	
	if (pb_up && *counter_pos < 5) // Checking pushbutton up and counter (5 count), to only do this, if PB up is pressed and the position led is [4] or less.
	{
		BIT_ON(PORTC, (5 - *counter_pos)); // Set High bit of the position number 5-counter.
		*counter_pos = *counter_pos + 1; // Increment counter
	}                                          
	else if (pb_down && *counter_pos >= 1) // Checking pushbutton down and counter, to only do this, if PB down is pressed and the position led is [0] or higher.
	{
		BIT_OFF(PORTC, ( 6 - *counter_pos)); // Set Low bit of the position number 6-counter.
		*counter_pos = *counter_pos - 1;	// Decrement counter.
	}
	else if (pb_down && *counter_pos == 0)
	{                                      /**  The numbers 5 and 6 is cause the using ports are PC.5 - PC.1 **/
		BIT_OFF(PORTC, ( 5 - *counter_pos));
	}
}

// 3. This function is for the part three. (Counter Binary)
void show_leds_3_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable)
{
	if (up_stable && down_stable)  // Don't allow press both button at same time.
	{
		return;
	}
	/*** NOTE : the number two that is added and subtracted respectively is equivalent to subtracting or adding one. 
	this is because led 0 is connected to PC 1. ***/

	if (pb_up && *counter_pos < 31) // ADD one (binary number 8-bits) if up button is pressed. the top is 31 binary number.
	{
		PORTC = PORTC +  2;
	   *counter_pos = *counter_pos + 1;                       
	}
	else if (pb_down && *counter_pos > 0) // SUBTRACT one (binary number 8-bits) if down button is pressed. the top is 0 binary number.
	{
		PORTC = PORTC - 2;
		*counter_pos = *counter_pos - 1;     
    }
}