/*
 * Part_1___Leds_On_Change_Pos.c
 *
 * Created: 2/9/2019 7:23:46 PM
 * Author : Anthony
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/builtins.h>
#include <stdio.h>

#include "manipulationbit.h"
#include "pb_debouncer.h"
#include "show_leds_1_2_3.h"

                                                           ////////// PROTOTYPES FUNCTIONS //////////
bool delay_counter_led_9(uint8_t* counter, uint8_t top_count_sec);
bool pb0_read(void);
bool pb1_read(void);
				                                      ///////////// SETTING DELAY AND TOP COUNT ////////////
													   
#define DELAY_5_mSEC	  80000  // 5 ms delay @ 16 MHz.

#define _050_SEC_COUNT      100  // 0.50 sec @ 5 ms count.
#define PBD_HIGH_COUNT       40  // 200 mSec time debouncing High.
#define PBD_LOW_COUNT        40  // 200 mSec time debouncing Low.


  // I/O Function to general setting initial system.
  void system_setup(void)
  {
	  ///////////////////////////////    IN SETTING    ///////////////////////////////
	  
	  DDRB &= ~(1<<DDB0);    // Set PB.0 as Input, THE PUSHBUTTON_00 IS CONNECTED. --> D8 (Decrement)
	  DDRB &= ~(1<<DDB1);    // Set PB.1 as Input, THE PUSHBUTTON_01 IS CONNECTED. --> D9 (Increment)
	
	  PORTB = BIT_OFF(PORTB,0); 	// Set low for High Impedance PB.0 -> D8.- 10K RESISTOR CONNECTED.
	  PORTB = BIT_OFF(PORTB,1); 	// Set low for High Impedance PB.1 -> D9.- 10K RESISTOR CONNECTED.
	  
	  ///////////////////////////////    OUT SETTING    ///////////////////////////////
	  
	  DDRC |=  (1<<DDC1) | (1<<DDC2) | (1<<DDC3) | (1<<DDC4) | (1<<DDC5);    // Set PC.1 - PC.5 as output. 
	  DDRB |= (1<<DDB5);  //LED#9 -> PB5
	 
	  
	  PORTC =  BIT_OFF(PORTC,1) | BIT_OFF(PORTC,2) | BIT_OFF(PORTC,3) | BIT_OFF(PORTC,4) | BIT_OFF(PORTC,5);   // Set PC.1 - PC.5 initial Low ('0').
	  PORTB = BIT_OFF(PORTB,5) ; // Set PB.5 initial Low ('0').
	
  }
  
                               ////////// MAIN //////////
int main(void)
{
    // Initial System.
    system_setup();
    
    uint8_t blink_delay_count_05 = 0; //Variable to store count to 05 sec (100 counts @ 5 mSec).
	uint16_t count_limit = 0;

    pb_debouncer_t pb1_up; // Create a variable to debouncer.
	pb_debouncer_t pb0_down; // Create a variable to debouncer.
	
 
    pbd_init(&pb1_up, PBD_HIGH_COUNT, PBD_LOW_COUNT, false);  //Initing Debouncer objects.
    
	                                      /////***** EXECUTION BLOCK (PRINCIPAL EXECUTION) - Backbone - *****/////
    while (1) 
    {
	    if (delay_counter_led_9(&blink_delay_count_05,_050_SEC_COUNT))// Checking time to blink LED_9 (Blue) 
		{
			//Toggle Led_9.
			BIT_TOGGLE(PORTB,5);// 0.50 sec Off and 0.50 sec On , so T = 1 , f = 1/T = 1/1 = 1 Hz. LED_9 Blinking at 1 Hz.
		}
		
		bool pb1_up_count_raw = pb1_read(); // Read raw PB #1 input (PB Raw Up Count).
		bool pb0_down_count_raw = pb0_read(); // Read raw PB #0 input (PB Raw Down Count).
		
		pbd_process(&pb1_up, pb1_up_count_raw); // Debouncer PB #1 'Up count' and 
		pbd_process(&pb0_down, pb0_down_count_raw); // Debouncer PB #0 'Down count' and get stable value.
		
		bool pb1_up_count_edge = pbd_get_rising_edge(&pb1_up,false); //get rising edge (flanco pos) to pb1_up_count.
		bool pb0_down_count_edge = pbd_get_rising_edge(&pb0_down,false); //get rising edge (flanco pos) to pb0_down_count.
		
		bool pb1_up_stable = pbd_get_stable_value(&pb1_up);
	    bool pb0_down_stable = pbd_get_stable_value(&pb0_down);
	
	            /////***** PART 1 *****/////
				
		//	 show_leds_1_process(&count_limit, pb1_up_count_edge, pb0_down_count_edge, pb1_up_stable, pb0_down_stable);
				 
	           /////***** PART 2 *****/////
			   
		 //    show_leds_2_process(&count_limit, pb1_up_count_edge, pb0_down_count_edge, pb1_up_stable, pb0_down_stable);
			
               /////***** PART 3 *****/////
			   
		 //    show_leds_3_process(&count_limit, pb1_up_count_edge, pb0_down_count_edge, pb1_up_stable, pb0_down_stable);
		

		__builtin_avr_delay_cycles(DELAY_5_mSEC); // Principal Clock ( Master Clock )
    }
	
}
             ////////// END MAIN //////////
      
                                 ///////////////////////////////    FUNCTIONS   ///////////////////////////////
												
// 1. Function to delay counter X Sec with Duty Cycle 50%.													
bool delay_counter_led_9(uint8_t* counter_pos, uint8_t top_count_sec)
{
	if (*counter_pos >= top_count_sec) // Checking  the count has reached top.
	{
		*counter_pos = 0;  // Reset counter and return bool value True.
		return true;
	}
	else      //This condition is for the top count not reached.
	{
		*counter_pos = *counter_pos + 1; // if the top count not reached, Increment count.
		return false;
	}
}

// 2. Function to Read PB #0 (PB.0) 'Down count', and return true if pressed.
bool pb0_read(void)
{
	return (PINB & (1 << PORTB0)) != 0;
}

// 3. Function to Read PB #0 (PB.1) 'Up count', and return true if pressed.
bool pb1_read(void)
{
	// Read PB #1 (PB.1) 'Up count', and return true if pressed.
	return (PINB & (1 << PORTB1)) != 0;
}

