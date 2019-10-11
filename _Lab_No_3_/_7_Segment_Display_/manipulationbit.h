/*
 * manipulationbits.h
 *
 * Created: 1/27/2019 8:56:49 PM
 *  Author: Anthony
 */ 

         /***          I created this File for simplify the coding. I can use functions like BIT_OFF to set low (OFF) a specific bit of a port.         ***/

#ifndef MANIPULATIONBITS_H_
#define MANIPULATIONBITS_H_

#include <stdint.h>

////////////////////////////////////////    MACROS DE BITWISE     ///////////////////////////////////////////

#define BIT(x)                   (1<<(x))                   // I don't use it in the main code.  /****/

#define BIT_ON(prt,num_bit)      ((prt) |= BIT(num_bit))   //Set ON a bit of the portX and position numberX.
#define BIT_OFF(prt,num_bit)     ((prt) &= ~BIT(num_bit)) //Set OFF a bit of the portX and position numberX.
#define BIT_TOGGLE(prt,num_bit)  ((prt) ^= BIT(num_bit)) //Inverts (negate ~) the current value of the one bit, of the portX and position numberX, Do a XOR


#endif /* MANIPULATIONBITS_H_ */