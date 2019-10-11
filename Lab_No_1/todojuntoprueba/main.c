
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
#include "manipulationbits.h" //I created this library for simplify the coding. I can use functions like BIT_OFF to set low (OFF) a specific bit of a port.

                                                       ///////////// SETTING DELAY AND TOP COUNT ////////////
#define DELAY_025_SEC		 4000000  // 0.25 sec delay @ 16Mhz
#define DELAY_075_SEC		12000000  // 0.75 sec delay @ 16Mhz
#define DELAY_05_mSEC	       80000  // 5 mSec delay @ 16 MHz

#define _4_SEC_COUNT		     800  // 4 sec @ 5 ms count
#define _075_SEC_COUNT		     150  // 0.75 sec @ 5 ms count
#define _025_SEC_COUNT		      50  // 0.25 sec @ 5 ms count


// I/O Function to general setting initial system.
void system_setup(void)
{
                                           	///////////////////////////////    IN SETTING    ///////////////////////////////
	//Set PB.0 as Input, THE PUSHBUTTON_00 IS CONNECTED.
	DDRB |= (1<<DDB0);
	
	//Set low for High Impedance PB.0 -> D8.- 10K RESISTOR CONNECTED.
	PORTB = BIT_OFF(PORTB,0);
	
	                                      ///////////////////////////////    OUT SETTING    ///////////////////////////////
	// Set PC.4 PC.5 as output
	DDRC |= (1<<DDC4) | (1<<DDC5); // 0010 0000 (1<<4)
	
	//Set PC.4 PC.5 initial Low (cero).
	PORTC =  BIT_OFF(PORTC,4) | BIT_OFF(PORTC,5); // 0000 0000
	
}

 // Function to delay loop counter 1 Sec with Duty Cycle 75% (0.25 Off , 0.75 On).
bool delay_loop_counter_1_SEC(uint8_t* counter_1, uint8_t top_count_025_sec, uint8_t top_count_075_sec )
{
	uint8_t top_count_01_sec = (top_count_075_sec + top_count_025_sec);
	
       //Checking the counter is equal to top count 025 ( 50 = 25mSec).
	  if ( *counter_1 == top_count_025_sec)
	  {
		  BIT_TOGGLE (PORTC,5);
		  *counter_1 = *counter_1 + 1; //Increment counter and return bool value True.
		  return true;
	  }
	    //
	  else if (*counter_1 >= top_count_025_sec && *counter_1 < top_count_075_sec) {
		  
		  *counter_1 = *counter_1 + 1;
		  return false;
	  }
	  
	  else if (*counter_1 >= top_count_01_sec)
	   {
		  BIT_TOGGLE (PORTC,5);
		  *counter_1 = 0; //Reset counter and return bool value True.
		  return true;
	  }

	  else
	  {    //Increment counter, if the top count 025 not reached.
		  *counter_1 = *counter_1 + 1;
		  return false;
	  }
}
	 
bool delay_loop_counter_4_SEC(uint16_t* counter_04, uint16_t top_count_04_sec)
{
	// Checking  the count has reached top (800 = 4sec).
	if (*counter_04 >= top_count_04_sec)
	{
		// Reset counter.
		*counter_04 = 0;
		return true;
	}
	else
	{
		// if the top count not reached, Increment counter.
		*counter_04 = *counter_04 + 1;
		return false;
	}
}

int main(void)
{
	uint8_t blink_delay_loop_count_01 = 0; //Variable to store count to 1 sec (200 counts @ 5 mSec)
	uint16_t blink_delay_loop_count_04 = 0; //Variable to store count to 4 sec (800 counts @ 5 mSec)

	// Init system
	system_setup();
	
	while (1)
	{
		
		delay_loop_counter_1_SEC(&blink_delay_loop_count_01, _025_SEC_COUNT, _075_SEC_COUNT);
		
		    // Checking time to blink LED_1 (White) Connected in PC.5
	/*	if (delay_loop_counter_1_SEC(&blink_delay_loop_count_01, _025_SEC_COUNT, _075_SEC_COUNT))
		  {
			//Toggle LED
			BIT_TOGGLE (PORTC,5);
		  }*/
	
			// Checking PIN B.0 is pressed.
		  if ((PINB & (1 << PINB0)) != 0)
			{
				
				//Reset counters.
				 blink_delay_loop_count_01 = 0;
				 blink_delay_loop_count_04 = 0;
		        
				delay_loop_counter_1_SEC(&blink_delay_loop_count_01, _025_SEC_COUNT, _075_SEC_COUNT);
				//Set off LEDs for Blinking Coordination.
			 //    BIT_OFF (PORTC,4);
			  //   BIT_OFF (PORTC,5);
				 
				 //Wait 0.25 sec off and 0.75 on.
			/*	 __builtin_avr_delay_cycles(DELAY_025_SEC);
				 BIT_TOGGLE(PORTC,4) , BIT_TOGGLE(PORTC,5);
				 __builtin_avr_delay_cycles(DELAY_075_SEC);
				 BIT_TOGGLE(PORTC,4) , BIT_TOGGLE(PORTC,5);*/
			}
			
			else {
					// Checking time to blink LED_2 (Blue) Connected in PC.4
					if (delay_loop_counter_4_SEC(&blink_delay_loop_count_04, _4_SEC_COUNT))
					{
						// Toggle Led #1
						BIT_TOGGLE (PORTC,4); // 4 sec on and 4 sec off , so T = 8 , f = 1/T = 1/8 = 0.125 Hz. LED Blinking at 0.125 Hz.
					}
			}
		__builtin_avr_delay_cycles(DELAY_05_mSEC); // Principal Clock ( Master Clock )
	}
}





























///*
 //* todojuntoprueba.c
 //*
 //* Created: 2/3/2019 8:51:21 PM
 //* Author : Anthony
 //*/ 
//
//#include <avr/io.h>
//#include <avr/builtins.h>
//
//#define F_CPU 160000000UL
//#include <util/delay.h>
//
//int main(void)
//{
	//DDRC |=(1<<5);//LED 1
	//DDRC |=(1<<4);//LED 2
	//PORTB = 0x00;//Puch Button
	//
	//int cont=0;//Primer Contador
	//int cont2=0;//Segundo Contador
	//int cont3=0;//Tercer Contador
	//
	//while (1)
//
	//{
		//__builtin_avr_delay_cycles(800000);//Delay de 0.5seg.
		//
		//if (cont == 15)//Encender LED 1 por 0.75seg.
		//{
			//PORTC ^=(1<<5);//Puerto 5.
		//}
//
		//if (cont == 20)//Apagar el LED 1 por 0.25seg.
		//{
			//PORTC ^=(1<<5);//Puerto 5.
			//cont = 0;//Primer Contador.
		//}
//
		//cont++;
		//
		//if(cont2 > 80) {//Para que el segundo contador, cuando pase de 4seg., se inicialice.
			//cont2 = 0;//Segundo Contador
//
		//}
//
		//// Si se presiona el Push Button.
		//if((PINB)&(1<<0))// Activa el Push Button.
		//{
			//if (cont3 == 0)//Apagar el LED 2 por 0.5seg.
			//{
				//PORTC ^=(1<<4);//Puerto 4.
			//}
//
//
			//if (cont3 == 10)//Encender el LED 2 por 0.5seg.
//
			//{
				//PORTC ^=(1<<4);//Puerto 4.
				//cont3 = 0;//Tercer Contador
			//}
//
			//cont3++;//Aumentar el Tercer Contador.
		//}
//
		//// Mientras no se presione el Push Button
		//else
		//{
			//if (cont2 == 0)//Apagar el LED 2 por 4 seg.
			//{
				//PORTC ^=(1<<4);//Puerto 4.
			//}
			//if (cont2 == 80)//Encender el LED 2 por 4 seg.
			//{
				//PORTC ^=(1<<4);//Puerto 4.
				//cont2 = 0;//Segundo Contador.
			//}
			//cont2++;//Aumentar el Segundo Contador.
		//}
	//}
//}