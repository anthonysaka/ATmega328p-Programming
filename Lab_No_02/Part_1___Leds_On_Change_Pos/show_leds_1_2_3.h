/*
 * show_leds_1_2_3.h
 *
 * Created: 2/11/2019 11:07:55 PM
 *  Author: Anthony
 */ 

#ifndef SHOW_LEDS_1_2_3_H_
#define SHOW_LEDS_1_2_3_H_

#include <stdint.h>
#include <stdbool.h>

           /// PROTOTYPES FUNCTIONS ///
		   
void show_leds_1_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable);
void show_leds_2_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable);
void show_leds_3_process(uint16_t* counter_pos, bool pb_up, bool pb_down, bool up_stable, bool down_stable);

#endif /* SHOW_LEDS_1_2_3_H_ */