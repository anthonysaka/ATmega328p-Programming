/*
 * _7_Segment_Display.c
 *
 * Created: 2/20/2019 2:25:17 PM
 * Author : Anthony
 */ 
/*
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/builtins.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "manipulationbit.h"

#define DELAY_1_mSEC	  16000  // 1 mSec delay @ 16 MHz.
#define DELAY_10_uSEC	    160  // 10 uSec delay @ 16 MHz.

#define _100_mSEC_COUNT	    10000  // 100 msec @ 1 ms count

#define TRIGGER_PIN  DDD3
#define ECHO_PIN DDB0

#define a DDD4
#define b DDD5
#define c DDD6
#define d DDD7
#define e DDC0
#define f DDC1
#define g DDC2
#define punto DDC3
#define d1 DDB1
#define d2 DDB2
#define d3 DDB3
#define d4 DDB4

void display_select (volatile uint8_t* counter_position);
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], float distance_float);
bool general_counter (uint16_t* counter, uint16_t top_count_);
//void show_number_display (uint8_t code_to_show, uint8_t position_display);
void shoot_trigger(void);
void show_number_display (volatile uint8_t code_to_show, uint8_t screen);
void capture_time_echo ();


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
	
	/////// SETTING TIMER #0 ///////
	
	//cli(); //Set init disable all interrupts.
	TCCR0A |= (1<<WGM01); //Setting CTC MODE TIMER 0.
	TCCR0B |= (1<<CS00) | (1<<CS01); //Setting Prescaler /64. T=4 uS f= 250 kHz
	TCNT0 = 0; //  Counter register init 0.
	OCR0A = 249; // 1 mSec at 250kHz.
	
	/////// Interrupts - Timer #0 //////
	 
	TIMSK0 |= (1<<OCIE0A);  //Setting the ISR (Interrupts) COMPA vect.
	
	sei (); //Set enable all global interrupts.
}

void timer_1_setup (void)
{
	/////// SETTING TIMER #1 ///////
	
	TCCR1B |= (1<<CS11) |(1<<ICES1);  //Setting Prescaler /8. T=0.5 uS f= 2Mhz.
	                                  //Set detect trigger on rising edge.
	TCNT1 = 0; //Counter register init 0.
	
//	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1); //Setting enable interrupts input capture and overflow.
//	sei();
}


//volatile uint8_t flag_status = 0;
/*volatile */ float time_echo_rising = 0.0;
/*volatile */ float time_echo_falling = 0.0;
/*
//Cuando el echo suba y luego baje.
ISR(TIMER1_CAPT_vect)
{
	//capture_time_echo();
	if(flag_status == 0)
	{
		time_echo_rising = ICR1;
		TCCR1B &= ~ (1<<ICES1);
		flag_status =  1;
		return;
	}
	if (flag_status == 1)
	{
		time_echo_falling = ICR1;
		TCCR1B |= (1<<ICES1);
		flag_status = 0;
		return;
	}
}
*//*
volatile uint8_t codes_show_number [4] =
 {
	 0x00, 0x00, 0x00, 0x00
 };
 
volatile	 uint8_t position_display = 0;

ISR (TIMER0_COMPA_vect)
{
	 display_select(&position_display);
	
}

int main(void)
{
     // Initial System.
    system_setup();
	timer_1_setup();
	
	
	 uint8_t codes_digits[16] = 
	 { 
		 0x3F, 0x06, 0x5B, 0x4F,
		 0x66, 0x6D, 0x7D, 0x07,
		 0x7F, 0x6F, 0x77, 0x7C,
		 0x39, 0x5E, 0x79, 0x71
	 };
	 
	
		 
	 float echo = 0.0;
	 float measure_distance = 0.0;
	 uint16_t count_1=0;

	 bool medicion = false;
	 bool trigger = false;
	 bool pulso = false;
	 
    while (1) 
    {	 
		 separate_digit_codes_set(codes_digits, codes_show_number, measure_distance); 
		  show_number_display(codes_show_number[position_display - 1], position_display);
		 
	  /*   if (general_counter(&count_1, _100_mSEC_COUNT))
	      {
			   BIT_ON(PORTC, TRIGGER_PIN);
			   __builtin_avr_delay_cycles(DELAY_10_uSEC);
			   BIT_OFF(PORTC, TRIGGER_PIN);
			   
			   TCNT1 = 0;
			   
			  if ( (TIFR1 & (1<<5)) ) //detectar si hubo una captura de datos
			  {
				  //si hubo una captura
				  TIFR1 &= ~(1 << 5); //Colocando la bandera de captura en 0 de nuevo
				  
				  if ( (TCCR1B & (1 << ICES1)) ) // si estaba capturando el flanco possitivo
				  {
					  time_echo_rising = ICR1;
					  TCCR1B &= ~(1 << ICES1); //Para capturar el flanco negativo (poniendolo en 0)
				  }
				  else
				  if ( (TCCR1B & (1 << ICES1)) == 0 ) // si estaba capturando el flanco negativo
				  {
					  time_echo_falling = ICR1;
					  TCCR1B |= (1 << ICES1); //Para capturar el flanco Positivo (volviendo a ponerlo en 1)
			          echo = (time_echo_falling - time_echo_rising); 
				  }
				  
	      }
		 
		  }
		
	      
		    measure_distance = (echo * 0.0000005 * 17150); //Conversion a distancia.
			
         */
		   
		/*   if (TIFR0 & (1<<OCF0A)) // cuando exista un match en timer 0 cada 1 ms
	        {
				TIFR0 |= (1 << OCF0A);
			   
		    } 
			*/
			//
			/*
			if(general_counter(&count_1, _100_mSEC_COUNT)){
				 trigger = true;
				 pulso = true;
			}
			
			if(trigger){
				if(pulso){
					pulso = false;
					PORTD |= (1 << PORTD3);
					__builtin_avr_delay_cycles(DELAY_10_uSEC);
					PORTD &= ~(1 << PORTD3);
				}
				
				echo = PINB & (1 << PINB0);
				
				if(echo && !medicion){
					TCNT1 = 0;
					medicion = true;
				}
				
				if(!echo && medicion){
					measure_distance = ((TCNT1 * 0.0000005) * 33000) / 2;
					trigger = false;
					medicion = false;
				}
			}
			
		  __builtin_avr_delay_cycles(DELAY_10_uSEC);
	}
}


bool general_counter (uint16_t* counter, uint16_t top_count_)
  {
	  if (*counter >= top_count_) // Checking  the count has reached top
	  {
		  *counter = 0;  // Reset counter and return bool value True.
		  return true;
	  }
	  else          //This condition is for the top count  not reached.
	  {
		  *counter = *counter + 1;
		  return false;
	  }
  }
  
/*  
void capture_time_echo ()
	{
		if(flag_status == 0)
		{
			time_echo_rising = ICR1;
			TCCR1B &= ~ (1<<ICES1);
			flag_status =  1;
			return;
		}
		if (flag_status == 1)
		{
			time_echo_falling = ICR1;
			TCCR1B |= (1<<ICES1);
			flag_status = 0;
			return;
		}
	}
*/

/*
void display_select (volatile uint8_t* counter_position)
 {                                    // para seleccionar cual pantalla esta prendida.
	 if (*counter_position==4)
	 {
		 *counter_position=1;
	 }
	 else
	 {
		 *counter_position = *counter_position + 1;
	 }
 }
 
void separate_digit_codes_set(uint8_t codes_number[], volatile uint8_t codes_show_digit[], float distance_float)
{

	uint8_t digit_0, digit_1, digit_2, digit_3;
	uint16_t distance = distance_float * 10;
	
	digit_0 = distance %10 ;
	digit_1 = (distance % 100) / 10 ;
	digit_2 = (distance % 1000) / 100 ;
	digit_3 = distance / 1000 ;
	
	if (digit_2 == 0 && digit_3 == 0)
	{
		codes_show_digit[2] = 0x00;
	}
	else if (digit_3 == 0)
	{
		codes_show_digit[3] = 0x00;
	}
	
	codes_show_digit[0] = codes_number[digit_0];
	codes_show_digit[1] = codes_number[digit_1];
	codes_show_digit[2] = codes_number[digit_2];
	codes_show_digit[3] = codes_number[digit_3];
}

void show_number_display (uint8_t code_to_show, uint8_t screen)
{
	bool ss[8];
	//bool status_led[8];
	
	for(uint8_t i = 0; i < 8; i++){
		ss[i] = (code_to_show & (1 << i));
	}
	
	PORTB |= (1 << screen);
	
	for(uint8_t i = 1; i <= 4; i++){
		if(i != screen){
			PORTB &= ~(1 << i);
		}
	}
	
	for(uint8_t i = 0; i <= 3; i++){
		if(ss[i])
		{
			PORTD |= (1 << (i+4));
			}else{
			PORTD &= ~(1 << (i+4));
		}
	}
	
	for(uint8_t i = 4; i <= 7; i++){
		if(ss[i])
		{
			PORTC |= (1 << (i-4));
			}else{
			PORTC &= ~(1 << (i-4));
		}
	}
	
	if(screen == 2 && code_to_show != 0x00){
		PORTC |= (1 << PORTC3);
		}else{
		PORTC &= ~(1 << PORTC3);
	}

}

*/
/*
 * Measure distance.c
 *
 * Created: 2/22/2019 12:01:22 PM
 * Author : Jorge
 */ 

#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/builtins.h>


#define DELAY_4_MS			64000		// 4 ms delay real @ 16 MHz
#define DELAY_10_US			160			// 1 us delay real @ 16 MHz

#define DELAY_100_MS_COUNT	25			// 100 ms counter por ciclos

#define LCDS				4			// Cantidad de digitos que tiene el display
#define DATAMAX				10			// Cantidad de datos a capturar
#define FORMULA				0.0085		// Resultado de la formula de distancia por eco
#define INCH				2.54		// 1 cm = 2.54 pulg


void data_update ( float [DATAMAX] , float );
float data_average ( float [DATAMAX] , char );
void data_divisor ( float , uint8_t [LCDS] );

void display_dynamic ( uint8_t , float , uint8_t [DATAMAX] , char );
void display_clear ( void );
void display_print ( uint8_t , uint8_t );



void system_setup ( void )
{
	// ----------------------------------
	//    4 DIGIT - 7 SEGMENTS DISPLAY
	// ----------------------------------
	
	// Salidas - Segmentos [DCBA]
	DDRD |= (1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD4);
	
	// Salidas - Segmentos [HGFE]
	DDRC |= (1 << PORTC3) | (1 << PORTC2) | (1 << PORTC1) | (1 << PORTC0);
	
	// Salidas - Transistores (Selectores de digito)
	DDRB |= (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1);
	
	
	// ----------------------------------
	//        ULTRASONIC SENSOR
	// ----------------------------------
	
	// Salida - Trigger
	DDRD |= (1 << PORTD3);
	
	// Entrada - Echo
	DDRB |= (0 << PORTB0);
	
	
	// ----------------------------------
	//        TIMER / COUNTER
	// ----------------------------------
	
	TCCR1A |= (0 << WGM10) | (0 << WGM11);
	TCCR1B |= (0 << WGM12) | (0 << WGM13);
	TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
	TCCR1B |= (1 << ICES1);
	TCNT1 = 0; // Inicializar el contador
	// Modo CTC
	// WGM: Wave Generation Mode
	// CS#: Set prescaler to system clock (F_CPU)
	// ICES1: = 1 for trigger on rising edge
	
	
	// ----------------------------------
	//        PUSH BUTTON
	// ----------------------------------
	
	// Entrada - PB #0 (PA.0)
	DDRD |= (0 << PORTD2);
	PORTD |= (0 << PORTD2);
}


bool delay_loop_counter ( uint16_t *count , uint16_t top_count )
{
	// Check top count achieved
	if ( *count == top_count )
	{
		// Reset counter and return loop counter top reached
		*count = 0;
		
		return true;
	}
	else
	{
		// Increment counter and signal top count not reached
		*count = *count + 1;
		return false;
	}
}


int main(void)
{
	// System setup
	system_setup();
	
	// Local variables
	float number = 0; // Numero obtenido por cada medida del sensor
	float data[DATAMAX] = {0}; // Conjunto de datos tomados por el sensor
	uint8_t position_number = 0; // Posicion del digito a imprimir en el display
	char modes = '0'; // Modos: Centimetros y Pulgadas
	uint8_t digit[LCDS]; // Digitos separados para visualizar
	
	float echo = 0.0, time_up = 0.0, time_down = 0.0; // Tiempos del eco
	
	uint16_t delay_count_1 = 0, delay_count_2 = 0; // Contadores de ciclos o delays
	
    /* Replace with your application code */
    while (1)
    {
		// ----------------------------------
		//        ULTRASONIC SENSOR
		// ----------------------------------
		if ( delay_loop_counter(&delay_count_1,DELAY_100_MS_COUNT) )
		{
			PORTD |= (1 << PORTD3);
			__builtin_avr_delay_cycles(DELAY_10_US);
			PORTD &= ~(1 << PORTD3);
			
			TCNT1 = 0; //colocando el contador del timer1 en 0 para cuando inicie el echo			
			 
			if ( (TIFR1 & (1<<5)) ) //detectar si hubo una captura de datos
			{
				//si hubo una captura
				TIFR1 &= ~(1 << 5); //Colocando la bandera de captura en 0 de nuevo
				
				if ( (TCCR1B & (1 << ICES1)) ) // si estaba capturando el flanco possitivo
				{
					time_up = ICR1;
					TCCR1B &= ~(1 << ICES1); //Para capturar el flanco negativo (poniendolo en 0)
				}	 
				else
					if ( (TCCR1B & (1 << ICES1)) == 0 ) // si estaba capturando el flanco negativo
					{
						time_down = ICR1;
						TCCR1B |= (1 << ICES1); // Para capturar el flanco Positivo (volviendo a ponerlo en 1)
						echo = (time_down - time_up);
					}
			}
			
			number = FORMULA * echo; // Obtiene la distancia real
			data_update(data,number); // Actualiza el pack de datos obtenidos
		}


		// ----------------------------------
		//    4 DIGIT - 7 SEGMENTS DISPLAY
		// ----------------------------------

		// Tiempo de actualizacion de los datos
		if ( delay_loop_counter(&delay_count_2,DELAY_100_MS_COUNT) )
		{	
			// Si el boton esta precionado: convierte a pulgada
			if ( (PIND & (1 << PORTD2)) != 0 )
			{
				modes = '1'; // Modo pulgada ('1') seleccionado
				number = data_average(data,modes); // Pormedio de datos obtenido en pulgadas
				data_divisor(number,digit); // Separador de digitos
			}
			// Si el boton no esta precionado: convierte a centimetro
			else
			{
				modes = '0'; // Modo centimetro ('0') seleccionado				
				number = data_average(data,modes); // Promedio de datos obtenidos en centimetros
				data_divisor(number,digit); // Separador de digitos
			}
		}

		display_dynamic(position_number,number,digit,modes); // Dinamizar segun los datos (Digito a imprimir,numero,tipo de modo)
		display_clear(); // Limpiar pantalla (registros) para sobreescribir los datos
		display_print(digit[position_number],position_number); // Imprime el dato segun el digito en la posicion
		
		// Selector de posicion
		position_number ++; // Aumenta cada ciclo de while (4ms)
		if ( position_number > (LCDS - 1) ) //  Limita a la maxima cantidad de digitos
			position_number = 0; // Vuelve al primer digito una vez alcanzado el tope
		
		// ----------------------------------
		//          MASTER CLOCK
		// ----------------------------------
		
		// Reloj general, 4ms por ciclo de while (aproximadamente)
		__builtin_avr_delay_cycles(DELAY_4_MS);
	}
}



void data_update ( float data[DATAMAX] , float number )
{
	uint8_t ind;
	
	for ( ind = 0 ; ind < DATAMAX - 1 ; ind ++ )
	data[ind] = data[ind+1];
	
	data[ind-1] = number;

	return;
}


float data_average ( float data[DATAMAX] , char modes )
{
	float number = 0.0;
	uint8_t ind;
	
	if ( modes == '0' )
	{
		for ( ind = 0 ; ind < DATAMAX ; ind ++ )
			number += data[ind];
	
		return (number / DATAMAX);
	}
	else
	{
		if ( modes == '1' )
		{	
			for ( ind = 0 ; ind < DATAMAX ; ind ++ )
				number += data[ind];
			
			return ((number / DATAMAX) / INCH);
		}
	}
	
	return 0;
}


void data_divisor ( float auxiliar , uint8_t digit[LCDS] )
{
	int number = 0;
	
	number = auxiliar * 10;
	
	digit[3] = number / 1000;
	digit[2] = (number % 1000) / 100;
	digit[1] = ((number % 1000) % 100) / 10;
	digit[0] = ((number % 1000) % 100) % 10 / 1;

	return;
}


void display_dynamic ( uint8_t position , float number , uint8_t digit[DATAMAX] , char modes )
{
	uint8_t ind;
	
	if ( position == 0 )
		PORTB |= (1 << PORTB1);
	else
		PORTB &= ~(1 << PORTB1);
	
	if ( position == 1 )
		PORTB |= (1 << PORTB2);
	else
		PORTB &= ~(1 << PORTB2);

	if ( position == 2 )
		PORTB |= (1 << PORTB3);
	else
		PORTB &= ~(1 << PORTB3);

	if ( position == 3 )
		PORTB |= (1 << PORTB4);
	else
		PORTB &= ~(1 << PORTB4);

	// Aqui se apagan los digitos innecesarios (ceros a la izquierda)
	if ( modes == '0' || modes == '1' )
	{
		if ( number >= 0.000 && number <= 9.999 )
		{
			PORTB &= ~(1 << PORTB4);
			PORTB &= ~(1 << PORTB3);
		}
		
		if ( number >= 10.000 && number <= 99.999 )
			PORTB &= ~(1 << PORTB4);


		// Error en pantalla para modo centimetros
		if ( modes == '0' )
			if ( number < 2.000 || number > 400.000 )
			{
				PORTB &= ~(1 << PORTB3);
				PORTB &= ~(1 << PORTB2);
				PORTB &= ~(1 << PORTB1);
			
				for ( ind = 0 ; ind < LCDS ; ind ++ )
					digit[ind] = 10;
			}
		
		// Error en pantalla para modo pulgadas
		if ( modes == '1' )
			if ( number < (2.000 / 2.540) || number > (400.000 / 2.540) )
			{
				PORTB &= ~(1 << PORTB3);
				PORTB &= ~(1 << PORTB2);
				PORTB &= ~(1 << PORTB1);
			
				for ( ind = 0 ; ind < LCDS ; ind ++ )
					digit[ind] = 10;
			}
	}

	return;
}


void display_clear ( void )
{
	uint8_t ind;
	
	// Limpia los segmentos DCBA
	for ( ind = 0 ; ind < 4 ; ind ++ )
		PORTC &= ~(1 << ind);
	
	// Limpia los segmentos HGFE
	for ( ind = 7 ; ind > 3 ; ind -- )
		PORTD &= ~(1 << ind);
	
	return;
}


void display_print ( uint8_t digit , uint8_t position )
{
	switch ( digit )
	{
		case 0:
			PORTD |= 0b11110000;
			PORTC |= 0b00000011;
			break;
		
		case 1:
			PORTD |= 0b01100000;
			PORTC |= 0b00000000;
			break;
		
		case 2:
			PORTD |= 0b10110000;
			PORTC |= 0b00000101;
			break;
		
		case 3:
			PORTD |= 0b11110000;
			PORTC |= 0b00000100;
			break;
		
		case 4:
			PORTD |= 0b01100000;
			PORTC |= 0b00000110;
			break;
		
		case 5:
			PORTD |= 0b11010000;
			PORTC |= 0b00000110;
			break;
		
		case 6:
			PORTD |= 0b11010000;
			PORTC |= 0b00000111;
			break;
		
		case 7:
			PORTD |= 0b01110000;
			PORTC |= 0b00000000;
			break;
		
		case 8:
			PORTD |= 0b11110000;
			PORTC |= 0b00000111;
			break;
		
		case 9:
			PORTD |= 0b01110000;
			PORTC |= 0b00000110;
			break;
		
		// Imprime 'E' (Error)
		case 10:
			PORTD |= 0b10010000;
			PORTC |= 0b00000111;
			break;
	}
	
	// Punto decimal
	if ( position == 1 && digit != 10 )
		PORTC |= (1 << PORTC3);
	
	return;
}
