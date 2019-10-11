/*
 * _sensor_ultrasonic_.c
 *
 * Created: 2/28/2019 10:24:25 AM
 * Author : Anthony
 */ 

#include <avr/io.h>
#include <avr/builtins.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "manipulationbit.h"

#define TRIGGER_PIN  DDD3
#define ECHO_PIN     DDB0

#define a            DDD4
#define b            DDD5
#define c            DDD6
#define d            DDD7
#define e            DDC0
#define f            DDC1
#define g            DDC2
#define punto        DDC3
#define d1           DDB1
#define d2           DDB2
#define d3           DDB3
#define d4           DDB4

#define DELAY_10_uSEC	 160  // 10 uSec delay @ 16 MHz.

		///////////////////////////////    PROTOTYPES    ///////////////////////////////
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], float distance_float);
void shooter_trigger (void);
void show_number();
float data_filtred (float data[]);
void data_update (float data[] , float measure);


// I/O Function to general setting initial system.
void system_setup(void)
{
		///////////////////////////////    IN SETTING    ///////////////////////////////
		
		DDRB &= ~(1<<ECHO_PIN);    // Set PB.0 as Input, --> Echo
		DDRD &= ~(1<<DDD2);    // Set PD.2 as Input, change mode cm/inch.
		PORTD = BIT_OFF(PORTD,2); 	// Set low for High Impedance PD.2 ->- 10K RESISTOR CONNECTED.

		///////////////////////////////    OUT SETTING    ///////////////////////////////
		
		DDRC |=  (1<<e) | (1<<f) | (1<<g) | (1<<punto);    //Set PC.0 - PC.3 as output. (e f g h(punto)).
		DDRD |=  (1<<a) | (1<<b) | (1<<c) | (1<<d);   //Set PD.4 - PD.7 as output. (a b c d).
		DDRD |=  (1<<TRIGGER_PIN);    //Set PD.3 as output. Trigger.
		DDRB |= (1<<d1) | (1<<d2) | (1<<d3) | (1<<d4); //Set PB.1 - PB.4 as output. (D1 D2 D3 D4).
		
		PORTC = BIT_OFF(PORTC,0) | BIT_OFF(PORTC,1) | BIT_OFF(PORTC,2) | BIT_OFF(PORTC,3);
		PORTB = BIT_OFF(PORTB,1) | BIT_OFF(PORTB,2) | BIT_OFF(PORTB,3) | BIT_OFF(PORTB,4) | BIT_OFF(PORTB,0);    // Set ports outs initial value Low '0'.
		PORTD = BIT_OFF(PORTD,3) | BIT_OFF(PORTD,4) | BIT_OFF(PORTD,5) | BIT_OFF(PORTD,6) | BIT_OFF(PORTD,7);
}

void timer_0_setup (void)
{ /////// SETTING TIMER #0 ///////
	TCCR0A |= (1<<WGM01); //Setting CTC MODE TIMER 0.
	TCCR0B |= (1<<CS02); //Setting Prescaler /256. T=16 uS f= 62.5kHz
	TCNT0 = 0; //  Counter register init 0.
	OCR0A = 249; // 4 mSec at 62.5kHz.
	
	/////// Interrupts - Timer #0 //////
	
	TIMSK0 |= (1<<OCIE0A);  //Setting the ISR (Interrupts) COMPA vect.
	sei (); //Set enable all global interrupts.
}

void timer_1_setup (void)
{  /////// SETTING TIMER #1 ///////
	                                    // Set Normal mode.
	TCCR1B |= (1<<CS11) | (1<<ICES1);  //Setting Prescaler /8. T=0.5 uS f= 2Mhz.
	                                  //Set detect trigger on rising edge.
	TCNT1 = 0; //Counter register init 0.
}

////////// VARIABLES GLOBALS AND VOLATILES/////////

volatile uint8_t position_digit = 0;
volatile uint8_t flag_100_ms = 0;
volatile uint8_t flag_1_sec = 0;
volatile bool flag_100_ms_status = false;
volatile bool flag_1_sec_status = false;
volatile uint8_t codes_show_number [4] =
{
	0x00, 0x00, 0x00, 0x00
};


int main(void)
{
    // Initial System.
    system_setup();
	timer_0_setup();
    timer_1_setup();
	
   ////////// VARIABLES LOCALS/////////
    uint8_t codes_digits[17] =
    {     //Codes binary to create a numbers on display 7 segments.
	    0x3F, 0x06, 0x5B, 0x4F,
	    0x66, 0x6D, 0x7D, 0x07,
	    0x7F, 0x6F, 0x77, 0x7C,
	    0x39, 0x5E, 0x79, 0x71,
		0x00
    };
	
	 float data_measure[10] = { }; // Variable to store a 10 measure.
	  
	 float time_echo_rising = 0.000;
	 float time_echo_falling = 0.000;
	 float echo = 0.000;
	 float measure_distance = 0.000; // Variable to store the distance formula result.
	

    while (1) 
    {
		if (flag_100_ms >=25) // 25 count is equivalent at 100 msec @ 4ms counter.
		{
			flag_100_ms = 0;
			flag_100_ms_status = true; //Flag to know when the clock has reached 100 ms.
		}
		if (flag_1_sec >=250) // 250 count is equivalent at 1 sec @ 4ms counter.
		{
			flag_1_sec = 0;
			flag_1_sec_status = true; //Flag to know when the clock has reached 1 sec.
		}
		
		if ( flag_100_ms_status )  // Every 100 ms, execute code below . The code below is to do the processor to shoot a trigger and get a echo signals.
		{
			flag_100_ms_status = false;
			
			shooter_trigger(); //This shoot a trigger signal.
			
			TCNT1 = 0; //Resetting the count timer to 0.
			
			if (TIFR1 & (1<<ICF1)) // Checking if a capture event has occurred.
			{
				TIFR1 |= (1<<ICF1); //Resetting the capture event flag. 1 -> 0.
				
				if (TCCR1B & (1<<ICES1)) //Checking if a rising edge is captured.
				{
					 time_echo_rising = ICR1; // Save or store the exact count timer value when the echo signal is up.
					 TCCR1B &= ~(1 << ICES1); //Setting to capture a falling edge.
				}
				else if ((TCCR1B & (1<<ICES1)) == 0 ) //Checking if a falling edge is captured.
				{
					time_echo_falling = ICR1; //Save or store the exact count timer value when the echo signal is down.
					TCCR1B |= (1 << ICES1); //Setting to capture a rising edge.
					
					echo = (time_echo_falling - time_echo_rising); // Get the high value of the signal echo.
				}
			}
			
			measure_distance = (echo * 0.0000005 * 17150); //Convert to measure (distance) in cm.
			
			if ((PIND & (1 << PIND2)) !=0) // If the button is pressed, convert the measure to inches.
			{
				measure_distance = (measure_distance / 2.54) ;
			}
			
			data_update(data_measure,measure_distance);  // store 10 measure, every 100 ms.
		}
		
		if (flag_1_sec_status) // Every one second, this is true.
		{                                      
			flag_1_sec_status=false; 
		    measure_distance = data_filtred(data_measure); // get average of the measures. (filter)
		    separate_digit_codes_set(codes_digits, codes_show_number, measure_distance); // Call the function to separate show digits in code to 7 segments display.
		}
    }
}

//This interrupt occur when the timer0 get at 4 ms.
ISR (TIMER0_COMPA_vect)
{
	show_number(codes_show_number, position_digit); 
	position_digit++;
	flag_100_ms++;   
	flag_1_sec++;
	
		if (position_digit>=4)
		{
			position_digit=0;
		}  		
}

// This function is to store the measures on arrays of 10 elements.
void data_update (float data[] , float measure)
{
	uint8_t ind =0;
	
	for ( ind = 0 ; ind < 10  ; ind ++ )
	data[ind] = measure;
	
	return;
}

// This function get average of the measures.
float data_filtred (float data[])
{
	uint8_t i;
	//uint8_t correct=0;
	float sum_measure=0.000;
	//float measure_filtred = 0.000;
	float measure_prom = 0.000;
//	float measure_correct= 0.000;
	
	
	for ( i = 0 ; i < 10  ; i ++ )
	{	
			sum_measure+=data[i];
	}
		measure_prom = sum_measure / 10;
		return measure_prom;
}

// Function to separate in digits the got distance.
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], float distance_float)
{
	uint8_t digit_0, digit_1, digit_2, digit_3;
	uint16_t distance = distance_float * 10; // multiply by 10 to move decimal point.
	
	digit_0 = distance % 10 ;
	digit_1 = (distance % 100) / 10 ;
	digit_2 = (distance % 1000) / 100 ;
	digit_3 = distance / 1000 ;
	
	codes_show_digit[0] = codes_number[digit_0];
	codes_show_digit[1] = codes_number[digit_1];
	codes_show_digit[2] = codes_number[digit_2];
	codes_show_digit[3] = codes_number[digit_3];
}

//This function is the principal to show the number on display.
void show_number()
{
	PORTC &= ~(0x0F); // Clear display number and selector display.
	PORTD &= ~(0xF0);
	PORTB &= ~(0x1E);
	
	if (codes_show_number[2] == 0x3F && codes_show_number[3]==0x3F) //Se
	{
		codes_show_number[2] = 0x00;
		codes_show_number[3] = 0x00;
	}
	
	if (codes_show_number[3]==0x3F && codes_show_number[2]!=0)
	{
		codes_show_number[3] = 0x00;
	}
	
	if (codes_show_number[1]<=0x5B && codes_show_number[2]==0x00 && codes_show_number[3]==0x00)
	{
		codes_show_number[3] = 0x00;
		codes_show_number[2] = 0x00;
		codes_show_number[1] = 0x79;
		codes_show_number[0] = 0x79;
	}
	
	if (codes_show_number[3]>=0x66 && codes_show_number[2]!=0x00 && codes_show_number[1]!=0x00 && codes_show_number[0]!=0x00)
	{
		codes_show_number[3] = 0x00;
		codes_show_number[2] = 0x00;
		codes_show_number[1] = 0x79;
		codes_show_number[0] = 0x79;
	}
	
 //Port C the msl bits and port D lsb bits.
	PORTC |= codes_show_number[position_digit] >> 4;  //Turning On the leds that visualizer the numbers on 7 Segments Displays.
	PORTD |= codes_show_number[position_digit] << 4;
	BIT_ON(PORTB,(position_digit + 1));        //Switching the selector displays (Transistor Switching).
	
	 if (position_digit==1)
	 {                           //Turning On 'dot' display 2 ( right to left).
		 BIT_ON(PORTC,punto);
	 }
	 
}
// Function to shooter the  signal trigger.
void shooter_trigger (void)  
{
	BIT_ON(PORTD, TRIGGER_PIN);
	__builtin_avr_delay_cycles(DELAY_10_uSEC);
	BIT_OFF(PORTD, TRIGGER_PIN);
}