/*
 * Temperatura_Serial_Com.c
 *
 * Created: 3/19/2019 8:44:14 PM
 * Author : Anthony Sakamoto
 */ 

						/********************************  Developer: Anthony Sakamoto   *********************************/
						/**/               ////        ///////////   ///////////            ////    ////                /**/ 
						/**/              ////        ////   ////   ////    ////           ////    ////                 /**/ 
						/**/ 	         ////        ////   ////   ////   ////            ////    ////                  /**/ 
						/**/ 		    ////        ///////////   ////////////           ///////////////                /**/ 
						/**/    	   //////////  ////   ////   ////     ////  ///             ////                    /**/ 
					    /**/	      //////////  ////   ////   ////////////   ///             ////                     /**/      
						/**************************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/builtins.h>
#include <stdio.h>
#include "manipulationbit.h"
#include "pb_debouncer.h"

#define a            DDD4   /*    7 Segments    */
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

#define LED_RED    DDD2    /*  Leds Indicators Modes  */
#define LED_WHITE   DDD3
#define LED_BLUE    DDB5

#define AVCC 4.570        /*   Values to ADC and Convert Functions   */
#define VREF 5.001
#define SERIE_RESISTOR 10000
#define RT_TABLE_SIZE  39	

#define PBD_HIGH_COUNT       40  // 50 mSec time debouncing High.
#define PBD_LOW_COUNT        40  // 50 mSec time debouncing Low.

#define DELAY_5_mSEC	  80000  // 5 ms delay @ 16 MHz.

#define BAUD_RATE_VALUE 103  //This is the result of formula (F_CPU / (USART_BAUDRATE * 16UL))) - 1) to 9600 baud rate.

////////////////////////////////////////////   *** PROTOTYPES ***    /////////////////////////////////////////////////////
		
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], int16_t distance_float);
void show_number();
float convert_to_volt(float adc_value);
float convert_to_resistor(float volt_in);
float convert_to_temp_celsius(float rt_values[], float rt_temp_values[], float got_value_resistor);
float linear_interpolate_formula(float res_0, float temp_0, float res_1, float temp_1, float res);
void turn_leds_visualizer(uint8_t* visualizer_status);
void visualizer_counter(uint8_t* count_led, bool pb_status);
int usart_putchar(char u8Data, FILE *mystdout);
bool pb0_read(void);

/********************************************************************************************************************/

// I/O Function to general setting initial system.
void system_setup(void)
{
	///////////////////////////////    IN SETTING    ///////////////////////////////
	
	DDRC &= ~(1<<DDC4);    // Set PC.4 as Input, change mode visualization ( Volt, Temp Celsius, Temp Faradios
	PORTC = BIT_OFF(PORTC,4); 	// Set low for High Impedance PC.4 ->- 10K RESISTOR CONNECTED.

	///////////////////////////////    OUT SETTING    ///////////////////////////////
	
	DDRC |=  (1<<e) | (1<<f) | (1<<g) | (1<<punto);    //Set PC.0 - PC.3 as output. (e f g h(punto)).
	DDRD |=  (1<<a) | (1<<b) | (1<<c) | (1<<d) | (1<<LED_RED) | (1<< LED_WHITE);   //Set PD.4 - PD.7 as output. (a b c d).   
	DDRB |= (1<<d1) | (1<<d2) | (1<<d3) | (1<<d4) | (1<<LED_BLUE); //Set PB.1 - PB.4 as output. (D1 D2 D3 D4). 
	
	PORTC = BIT_OFF(PORTC,0) | BIT_OFF(PORTC,1) | BIT_OFF(PORTC,2) | BIT_OFF(PORTC,3);
	PORTB = BIT_OFF(PORTB,1) | BIT_OFF(PORTB,2) | BIT_OFF(PORTB,3) | BIT_OFF(PORTB,4) | BIT_OFF(PORTB,0) | BIT_OFF(PORTB,5);    // Set ports outs initial value Low '0'.
	PORTD = BIT_OFF(PORTD,2) | BIT_OFF(PORTD,3) | BIT_OFF(PORTD,4) | BIT_OFF(PORTD,5) | BIT_OFF(PORTD,6) | BIT_OFF(PORTD,7);
}

void timer_0_setup (void)
{   /////// SETTING TIMER #0 ///////
	TCCR0A |= (1<<WGM01); //Setting CTC MODE TIMER 0.
	TCCR0B |= (1<<CS02); //Setting Prescaler /1024. T=64 uS f= 62.5kHz
	TCNT0 = 0; //  Counter register init 0.
	OCR0A = 249; // 4 mSec at 62.5kHz.
	
	/////// Interrupts - Timer #0 //////
	
	TIMSK0 |= (1<<OCIE0A);  //Setting the ISR (Interrupts) COMPA vect.
	sei (); //Set enable all global interrupts.
}

void timer_1_setup (void)
{  /////// SETTING TIMER #1 ///////
	// Set CTC mode.
	TCCR1B |= (1<<WGM12) | (1<<CS12);  //Setting Prescaler /256. T=16uS f= 62.5Khz.
	//Set detect trigger on rising edge.
	TCNT1 = 0; //Counter register init 0.
	OCR1B = 62; //1mSec at 62.5Khz.
}

void UART_init_setup(void)
{
	UBRR0 = BAUD_RATE_VALUE; //Set Baud Rate at 9600 bps.
	UCSR0B |= (1<TXEN0) | (1<<RXEN0);  // Set On TX and Rx.
	UCSR0B |= (1<<RXCIE0) | (0<<UDRIE0);//Enable Rx Interrup and Disable Tx Interrup
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); // Set frame format to 8 data bits, no parity, 1 stop bit.
	
	sei();
}

void ADC_init_setup(void)
{
	// Select Vref =A Vcc and set rigth adjust result. select ADC5 pin.
	ADMUX |= (1<<REFS0) | (0<<ADLAR) | (1<<MUX0) | (1<<MUX2);
	//set prescaller to 128, enable auto-triggering, enable ADC interupt and enable ADC.
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADATE) | (1<<ADIE) | (1<<ADEN);
	//set ADC trigger source - Timer1 compare match B.
	ADCSRB |= (1<<ADTS1)|(1<<ADTS0);
}


////////// VARIABLES GLOBALS AND VOLATILES/////////
volatile uint8_t position_digit = 0;
volatile uint8_t flag_100_ms = 0;
volatile uint8_t codes_show_number [4] =
{
	0x00, 0x00, 0x00, 0x00
};

volatile  float ADC_values_acumulator = 0;
volatile  float ADC_average = 0;
volatile uint8_t ADC_sample_index = 0;

volatile bool flag_adc_complete_convert = false;
bool flag_status_volt = false;
bool flag_status_celsius = false;
bool flag_status_fahrenheit = false;

//Set stream pointer.
FILE mystdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

/*****************************************   RT -10K RT vs Temperature Table   **************************************/
/**/     float rt_values[] = { 182.6, 205.5, 231.9, 262.4, 297.7, 338.7, 386.6, 442.6, 508.3, 585.7, 677.3,       /**/   
/**/	                  786, 915.4, 1070, 1256, 1481, 1753, 2084, 2490, 2989, 3605, 4372, 5330, 6535,           /**/
/**/	                  8059, 10000, 12488, 15698, 19872, 25339, 32554, 42157, 55046, 72500, 96358,             /**/
/**/                      129287, 175200, 239900, 332094 };                                                       /**/
/**/                                                                                                              /**/
/**/    float rt_temp_values[] = { 150, 145, 140, 135, 130, 125, 120, 115, 110, 105, 100, 95, 90, 85, 80, 75,     /**/
/**/	                        70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0, -5, -10, -15, -20,      /**/
/**/                       -25, -30, -35, -40 };                                                                  /**/
/********************************************************************************************************************/

int main(void)
{
	//////////////////////////   VARIABLES LOCALS   ///////////////////////////
	
	pb_debouncer_t pb0; // Create a variable to debouncer.
	float volt_value = 0.0;
	float resistor_value = 0.0;
	float temp_c_value = 0.0;
	float temp_f_value = 0.0;
	
	uint8_t visualizer_status_count = 0;
	uint8_t codes_digits[17] =
	{     //Codes binary to create a numbers on display 7 segments.
		0x3F, 0x06, 0x5B, 0x4F,
		0x66, 0x6D, 0x7D, 0x07,
		0x7F, 0x6F, 0x77, 0x7C,
		0x39, 0x5E, 0x79, 0x71,
		0x00
	};
   /////////////////////////////////////////////////////////////////////////	 
   
	 stdout = &mystdout; //assign our stream to standart I/O streams, setup our stdio stream.
	
  /***************************         Initial System.      *******************************************/
	 system_setup();
	 timer_0_setup();
	 timer_1_setup();
	 ADC_init_setup();
	 UART_init_setup();
	 pbd_init(&pb0, PBD_HIGH_COUNT, PBD_LOW_COUNT, false);  //Initing Debouncer objects.
	 
  /***************************************************************************************************/
  
    while (1) 
    {
		
	    bool pb0_count_raw = pb0_read(); // Read raw PB #0 input (PB Raw Down Count).
     	pbd_process(&pb0, pb0_count_raw); // Debouncer PB #0 'Down count' and get stable value.
		bool pb0_count_edge = pbd_get_rising_edge(&pb0,false); //get rising edge (flanco pos) to pb0_down_count.

		visualizer_counter(&visualizer_status_count, pb0_count_edge);
		turn_leds_visualizer(&visualizer_status_count);
		
		if (flag_adc_complete_convert) // This is true every 100 mSec, when the ADC convert is done and interrupt got the average ADC.
		{
			flag_adc_complete_convert = false;
			volt_value = convert_to_volt(ADC_average); //Get volt value.
			resistor_value = convert_to_resistor( volt_value); // get resistor value.
			temp_c_value = convert_to_temp_celsius(rt_values, rt_temp_values , resistor_value); //Get temperature celsius with linear interpolation.
			temp_f_value = (float) (convert_to_temp_celsius(rt_values, rt_temp_values , resistor_value) * 1.80) + 32.0; // Get temperature Fahrenheit.
			// This printf, send through serial port the DATA. 
			printf("ADC = %9.3f    V = %7.3f    R = %10.2f    C = %7.1f    F = %8.1f \n", ADC_average, volt_value, resistor_value, temp_c_value, temp_f_value);
			
		}
	
		if (flag_status_volt)  // This condition  is execute, when Led Red is On. show the volt value.
		{
			flag_status_volt = false;
		  	uint16_t volt_value_mult = volt_value * 1000; // To move decimal dot.
			separate_digit_codes_set(codes_digits, codes_show_number,volt_value_mult); // Call the function to separate show digits in code to 7 segments display.
		}
		
		if (flag_status_celsius) // This condition  is execute, when Led White is On. show the temp celsius value.
		{
			flag_status_celsius = false;
			uint16_t temp_c_value_mult = temp_c_value * 10; // To move decimal dot.
			separate_digit_codes_set(codes_digits, codes_show_number,temp_c_value_mult); // Call the function to separate show digits in code to 7 segments display.
			
		}
		if (flag_status_fahrenheit) // This condition  is execute, when Led Blue is On. show the temp fahrenheit value.
		{
			flag_status_fahrenheit = false;
			uint16_t temp_f_value_mult = temp_f_value * 10; // To move decimal dot.
			separate_digit_codes_set(codes_digits, codes_show_number,temp_f_value_mult); // Call the function to separate show digits in code to 7 segments display.
		}

		__builtin_avr_delay_cycles(DELAY_5_mSEC); // to debouncer time high and low
    }
}

//This interrupt occur when the timer0 get at 4 msec.
ISR (TIMER0_COMPA_vect)
{
	show_number(codes_show_number, position_digit);
	position_digit++;
	flag_100_ms++;
	
	if (position_digit>=4)
	{
		position_digit=0;
	}
}

// This interrupt occur when the timer1 get at 1 msec. Then ADC trigger. (AUTO TRIGGER every 1 msec).
ISR (ADC_vect)
{
	TIFR1 |= (1<< OCF1B); // Disable flag of Timer1 match B.
	ADC_values_acumulator = ADC_values_acumulator + ADC; // Store samples.
	ADC_sample_index++;
	
	if (ADC_sample_index == 100) //Sampling 100.
	{
		ADC_average = (float) ADC_values_acumulator / 100.0; // Get average.
		flag_adc_complete_convert = true; // Flag to know when adc value is ready to use.
	    ADC_values_acumulator = 0;
		ADC_sample_index = 0;
	}
}

// Function to send data, serial port. This is used by 'printf'.
int usart_putchar(char u8Data, FILE *mystdout)
{
	if(u8Data == '\n')
	{
		usart_putchar('\r', 0);
	}
	//wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
	return 0;
}

// Function counter when button is pressed, to know which mode and led should be ON.
void visualizer_counter(uint8_t* count_led, bool pb_status)
{
   if (pb_status) // Only Increment, if Button was pressed.
   {
	   if (*count_led >= 2) // Top is 3 count. (0 -> 2)
	   {
		   *count_led = 0;
	   }
	   else
	   {
		   *count_led = *count_led + 1; // Increment variable.
	   }
   } 
}

// Function to turn on and off leds indicator modes.
void turn_leds_visualizer(uint8_t* visualizer_status)
{
	if (*visualizer_status == 0) // Only RED LED is On.
	{
		BIT_ON(PORTD,LED_RED);
		BIT_OFF(PORTD, LED_WHITE);
		BIT_OFF(PORTB,LED_BLUE);
		flag_status_volt = true; // Indicate mode Volt.
	}
	else if (*visualizer_status == 1)  // Only WHITE LED is On.
	{
		BIT_ON(PORTD,LED_WHITE);
		BIT_OFF(PORTD, LED_RED);
		BIT_OFF(PORTB,LED_BLUE);
		flag_status_celsius = true; // Indicate mode Celsius.
	}
	else if (*visualizer_status == 2)  // Only BLUE LED is On.
	{
		BIT_ON(PORTB,LED_BLUE);
		BIT_OFF(PORTD, LED_WHITE);
		BIT_OFF(PORTD,LED_RED);
		flag_status_fahrenheit = true;  // Indicate mode Fahrenheit.
	}
}

// Function to separate in digits the got measure temp.
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], int16_t distance_float)
{
	uint8_t digit_0, digit_1, digit_2, digit_3;
	bool flag_negative = false;
	if (distance_float < 0)
	{
		distance_float = 0 - (distance_float);
		flag_negative = true;
	}
	uint16_t distance=distance_float; // multiply by 10 to move decimal point.
	
	digit_0 = distance % 10 ;
	digit_1 = (distance % 100) / 10 ;
	digit_2 = (distance % 1000) / 100 ;
	digit_3 = distance / 1000 ;
	
	codes_show_digit[0] = codes_number[digit_0];
	codes_show_digit[1] = codes_number[digit_1];
	codes_show_digit[2] = codes_number[digit_2];
	
	if (flag_negative)
	{
		flag_negative = false;
		if (codes_show_digit[2]!=0x3F)
		{
			codes_show_digit[3] = 0x40; //signo negativo
		}
		
		if (codes_show_digit[2]==0x3F && codes_show_digit[1]!=0x3F)
		{
			codes_show_digit[2] = 0x40;
			codes_show_digit[3] = 0x00;
		}
		
		
	}
	else
	{
		codes_show_digit[3] = codes_number[digit_3];
	}
}

//This function is the principal to show the number on display.
void show_number()
{
	PORTC &= ~(0x0F); // Clear display number and selector display.
	PORTD &=~(0xF0);
	PORTB &= ~(0x1E);
	
	
	if ((PIND & (1<<PIND2))!=0)
	{//volt
		if (position_digit == 3)
		{
			BIT_ON(PORTC, punto);
		}
	}
	
	if ((PIND & (1<<PIND3))!=0)
	{             //Celsius
		if (position_digit == 1)
		{
			BIT_ON(PORTC, punto);
		}
		if (codes_show_number[2] == 0x3F && codes_show_number[3]==0x3F) //Se
		{
			codes_show_number[2] = 0x00;
			codes_show_number[3] = 0x00;
		}
		
		if (codes_show_number[3]==0x3F && codes_show_number[2]!=0 )
		{
			codes_show_number[3] = 0x00;
		}
		if (codes_show_number[2]==0x3F && codes_show_number[3]!=0 )
		{
			codes_show_number[2] = 0x00;
		}
		
	}
	
	if ((PINB & (1<<PINB5))!=0)
	{//Fahrenheit
		if (position_digit == 1)
		{
			BIT_ON(PORTC, punto);
		}
		if (codes_show_number[2] == 0x3F && codes_show_number[3]==0x3F) //Se
		{
			codes_show_number[2] = 0x00;
			codes_show_number[3] = 0x00;
		}
		
		if (codes_show_number[3]==0x3F && codes_show_number[2]!=0 )
		{
			codes_show_number[3] = 0x00;
		}
		if (codes_show_number[2]==0x3F && codes_show_number[3]!=0 )
		{
			codes_show_number[2] = 0x00;
		}
		
		
		
	}
	
	//Port C the msl bits and port D lsb bits.
	PORTC |= codes_show_number[position_digit] >> 4;  //Turning On the leds that visualizer the numbers on 7 Segments Displays.
	PORTD |= codes_show_number[position_digit] << 4 ;
	BIT_ON(PORTB,(position_digit + 1));        //Switching the selector displays (Transistor Switching).
	
}

//Function to Read PB #0 (PC.4) and return true if pressed.
bool pb0_read(void)
{
	return (PINC & (1 << PORTC4)) != 0;
}

// Function to convert ADC value to volt value.
float convert_to_volt(float adc_value)
{
	float volt_in = 0.000;

	volt_in = (float) AVCC/1024 * adc_value;
	return volt_in;
}

// Function to convert Volt value to resistor value.
float convert_to_resistor(float volt_in)
{
	float resistor_value = 0.0;

	resistor_value =  (SERIE_RESISTOR/volt_in) * (VREF - volt_in); //cambie avcc
	
	return resistor_value;
}

// Function to get result of the linear interpolate formula.
float linear_interpolate_formula(float res_0, float temp_0, float res_1, float temp_1, float res)
{
	float temp_fuction = 0;
	
	// Returns the interpolated y-value. Saturates to y0 or y1 if x outside interval [x0, x1].
	if (res <= res_0)
	{                  // Return temp_0 if res value is littler that res_0. Is incoherent.
		return temp_0;
	}
	if (res >= res_1)
	{                  // Return temp_1 if res value is bigger than res_0. Is incoherent.
		return temp_1;
	}
	
	// Linear Interpolation Formula.
	temp_fuction = (float) (((res - res_0) / (res_1 - res_0)) * (temp_1 - temp_0) + temp_0);

	return temp_fuction;
}

// Function to get temperature, using linear interpolate.
float convert_to_temp_celsius(float rt_values[], float rt_temp_values[] , float got_value_resistor)
{
	uint8_t index_table = 0;        /**********  'res' --> X & 'temp' --> Y  **********/
	float temp_celsius  = 0;

	if (got_value_resistor > rt_values[RT_TABLE_SIZE - 1]) // Checking input (resistor value) bounds and saturate if out-of-bounds.
	{
		return rt_temp_values[RT_TABLE_SIZE - 1]; //res-value too large, saturate to max temp-value.
	}
	else if (got_value_resistor < rt_values[0]) 
	{
		return rt_temp_values[0]; // res-value too small, saturate to min temp-value.
	}

	   // Finding the segment line that holds got value resistor.
	for (index_table = 0; index_table < (RT_TABLE_SIZE - 1); index_table++)
	{
		if (got_value_resistor >= rt_values[index_table] && got_value_resistor <= rt_values[index_table + 1])
		{
			  // Applying the linear interpolate formula to get temperature in celsius.
			temp_celsius = linear_interpolate_formula(rt_values[index_table], rt_temp_values[index_table], rt_values[index_table+1], rt_temp_values[index_table+1], got_value_resistor);
			return temp_celsius;
		}
	}
	    // Something with the data was wrong if we get here. Show number '404' like error signal, not found data.

	return 404;
}