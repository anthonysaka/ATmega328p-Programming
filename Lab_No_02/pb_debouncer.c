#include "pb_debouncer.h"

void pbd_init(pb_debouncer_t* pbd, uint8_t high_count, uint8_t low_count, bool initial_state)
{
	// Set parameters
	pbd->high_count = high_count;
	pbd->low_count = low_count;
	
	// Set members and working variables
	pbd->counter = 0;
	pbd->stable_value = initial_state;
	pbd->raw_input = initial_state;
	pbd->rising_edge = false;
	pbd->falling_edge = false;
	
}
void pdb_reset(pb_debouncer_t* pbd, bool reset_state)
{
	// Reset members and working variables
	pbd->counter = 0;
	pbd->stable_value = reset_state;
	pbd->raw_input = reset_state;
	pbd->rising_edge = false;
	pbd->falling_edge = false;
}

void pbd_process(pb_debouncer_t* pbd, bool raw_input)
{
	// Clear old edge flags and save current input
	pbd->rising_edge = false;
	pbd->falling_edge = false;
	pbd->raw_input = raw_input;
	
	// Get stable value and counter
	bool stable_value = pbd->stable_value;
	uint8_t counter = pbd->counter;
	
	// Check stable value is same current input
	if (stable_value == raw_input)
	{
		// Reset consecutive counter and exit
		pbd->counter = 0;
		return;
	}
	
	// Increment consecutive counter
	counter++;
	
	// Check stable low and high count reached
	if (raw_input == true && counter >= pbd->high_count)
	{
		// Change stable value to high, set rising edge flag, clear counter
		pbd->counter = 0;
		pbd->stable_value = true;
		pbd->rising_edge = true;
	}
	// Check stable high and low count reached
	else if (raw_input == false && counter >= pbd->low_count)
	{
		// Change stable value to high, set falling edge flag, clear counter
		pbd->counter = 0;
		pbd->stable_value = false;
		pbd->falling_edge = true;
	}
	else
	{
		// Only update the consecutive counter
		pbd->counter = counter;
	}
}

bool pbd_get_stable_value(pb_debouncer_t* pbd)
{
	// Return the stable filtered value
	return pbd->stable_value;
}

bool pbd_get_raw_input(pb_debouncer_t* pbd)
{
	// Return the stable filtered value
	return pbd->raw_input;
}
bool pbd_get_rising_edge(pb_debouncer_t* pbd, bool clear)
{
	// Get rising edge flag
	bool rising_edge = pbd->rising_edge;
	
	// Clear flag if asked
	if (clear)
	pbd->rising_edge = false;
	
	// Return rising edge flag
	return rising_edge;
}

bool pbd_get_falling_edge(pb_debouncer_t* pbd, bool clear)
{
	// Get falling edge flag
	bool falling_edge = pbd->falling_edge;
	
	// Clear flag if asked
	if (clear)
	pbd->falling_edge = false;
	
	// Return rising edge flag
	return falling_edge;
}