/*
 * pb_debouncer.h
 *
 * Created: 2/10/2019 5:55:22 PM
 *  Author: Anthony
 */ 

                          /**********/  //////////  This Code Was Developed By The Teacher (Jorge Luna). //////////     /**********/

#ifndef PB_DEBOUNCER_H_
#define PB_DEBOUNCER_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct _pb_debouncer
{
	// Parameters
	uint8_t high_count;				// Number of consecutive high values to change
	uint8_t low_count;				// Number of consecutive low values to change
	
	// Working variables
	uint8_t counter;				// Consecutive values counter
	
	// Member variables
	bool stable_value;				// Stable (debounced) value
	bool raw_input;					// Current raw input;
	bool rising_edge;				// Rising edge event detected
	bool falling_edge;				// Falling edge event detected
}
pb_debouncer_t;

void pbd_init(pb_debouncer_t* pbd, uint8_t high_count, uint8_t low_count, bool initial_state);
void pdb_reset(pb_debouncer_t* pbd, bool reset_state);
void pbd_process(pb_debouncer_t* pbd, bool raw_input);
bool pbd_get_stable_value(pb_debouncer_t* pbd);
bool pbd_get_raw_value(pb_debouncer_t* pbd);
bool pbd_get_rising_edge(pb_debouncer_t* pbd, bool clear);
bool pbd_get_falling_edge(pb_debouncer_t* pbd, bool clear);



#endif /* PB_DEBOUNCER_H_ */