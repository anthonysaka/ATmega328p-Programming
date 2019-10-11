/*
 * 03_ALL_COMBINED_PB_2_LED.c
 *
 * Created: 1/28/2019 9:51:41 PM
 * Author : Anthony
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/builtins.h>
#include "manipulationbits.h" //I created this file for simplify the coding. I can use functions like BIT_OFF to set low (OFF) a specific bit of a port.

                                                            ///////////// PROTOTYPES FUNCTIONS /////////////
bool delay_counter_led_2(uint8_t* counter, uint8_t top_count_sec);
bool delay_counter_led_1(uint8_t* counter_1, uint8_t top_count_status_off, uint8_t top_count_status_on);        

				                                       ///////////// SETTING DELAY AND TOP COUNT ////////////
													   
#define DELAY_25_mSEC	   400000  // 25 ms delay @ 16 MHz.

#define _025_SEC_COUNT		 10  // 0.25 sec @ 25 ms count.
#define _050_SEC_COUNT       20  // 0.50 sec @ 25 ms count.
#define _075_SEC_COUNT	     30  // 0.75 sec @ 25 ms count
#define _4_SEC_COUNT		160  // 4 sec @ 25 ms count.


  // I/O Function to general setting initial system.
void system_setup(void)
{
                                           	///////////////////////////////    IN SETTING    ///////////////////////////////
											   
	DDRB &= ~(1<<DDB0);     // Set PB.0 as Input, THE PUSHBUTTON_00 IS CONNECTED.
	PORTB = BIT_OFF(PORTB,0); 	// Set low for High Impedance PB.0 -> D8.- 10K RESISTOR CONNECTED.
	
	                                       ///////////////////////////////    OUT SETTING    ///////////////////////////////
	
	DDRC |= (1<<DDC4) | (1<<DDC5);        // Set PC.4 PC.5 as output
	PORTC =  BIT_OFF(PORTC,4) | BIT_OFF(PORTC,5);   // Set PC.4 PC.5 initial Low ('0').
	
}
                   ///// MAIN /////
int main(void)
{
	uint8_t blink_delay_count_05 = 0; //Variable to store count to 05 sec (20 counts @ 25 mSec).
	uint8_t blink_delay_count_01 = 0; //Variable to store count to 1 sec (50 counts @ 25 mSec)
	uint8_t blink_delay_count_4 = 0; //Variable to store count to 4 sec (160 counts @ 25 mSec)

	// Initial system
	system_setup();
	
	while (1)
	{
		if (delay_counter_led_1(&blink_delay_count_01, _025_SEC_COUNT, _075_SEC_COUNT)) // Checking time to blink LED_1 (White) Connected in PC.5
		  {
			 //Toggle LED_1
			BIT_TOGGLE (PORTC,5); // 0.25 sec Off and 0.75 sec On , so T = 1 , f = 1/T = 1/1 = 1 Hz. LED_1 Blinking at 1 Hz.
		  }
	
		if ((PINB & (1 << PINB0)) != 0) // Checking PIN B.0 is pressed.
		  {
		        if (delay_counter_led_2(&blink_delay_count_05, _050_SEC_COUNT))
		          {
			        blink_delay_count_4 = 0;
			         //Toggle Led_2
			        BIT_TOGGLE(PORTC,4); // 0.5 sec Off and 0.5 sec On , so T = 1 , f = 1/T = 1/1 = 1 Hz. LED_2 Blinking at 1 Hz.
			      }
		  }
			
			else
			 {       //This condition is for PIN B (PB.0) isn't pressed.
				 if (delay_counter_led_2(&blink_delay_count_4, _4_SEC_COUNT))  // Checking time to blink LED_2 (Blue) Connected in PC.4
				 {
					blink_delay_count_05 = 0;
					 // Toggle Led_2
					BIT_TOGGLE (PORTC,4); // 4 sec on and 4 sec off , so T = 8 , f = 1/T = 1/8 = 0.125 Hz. LED_2 Blinking at 0.125 Hz.
				 }
			 }
		__builtin_avr_delay_cycles(DELAY_25_mSEC); // Principal Clock ( Master Clock )
	}
}
                 ///// END MAIN /////

                                            ///////////////////////////////    FUNCTIONS   ///////////////////////////////
											
//1. Function to delay counter X Sec with Duty Cycle 50%.
bool delay_counter_led_2(uint8_t* counter, uint8_t top_count_sec) 
{
	if (*counter >= top_count_sec) // Checking  the count has reached top 
	{
		*counter = 0;  // Reset counter and return bool value True.
		return true;
	}
	else          //This condition is for the top count  not reached.
	{
		*counter = *counter + 1; // if the top count not reached, Increment count.
		return false;
	}
}

 // 2. Function to delay loop counter 1 Sec with Duty Cycle 75% (0.25 Off , 0.75 On).
 bool delay_counter_led_1 (uint8_t* counter_1, uint8_t top_count_status_off, uint8_t top_count_status_on )
 {
	 uint8_t top_count_01_sec = (top_count_status_on + top_count_status_off); // Variable to store 1 sec top count.
	 
	 //Checking the counter is equal to top count 025 ( 50 = 25mSec).
	 if ( *counter_1 == top_count_status_off)
	 {
		 *counter_1 = *counter_1 + 1; //Increment counter and return bool value True.
		 return true;
	 }
	 //
	 else if (*counter_1 > top_count_status_off && *counter_1 < top_count_status_on)
	  {
		  *counter_1 = *counter_1 + 1;
		 return false;
	 }
	 //
	 else if (*counter_1 == top_count_01_sec)
	 {
		 *counter_1 = 0; //Reset counter and return bool value True.
		 return true;
	 }
	 else
	 {    //Increment counter, if the top count 025 not reached.
		 *counter_1 = *counter_1 + 1 ;
		 return false;
	 }
 }