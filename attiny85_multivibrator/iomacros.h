//***********************************************************************************
//
// iomacros.h	(rev 1)
//
// Author:	Jeremy Greenwood
//
// Description:
//			Includes basic macro functions for input and output tasks. Intended for
//		use with AVR microcontrollers compiled with GCC.
//
//		To use this in a project:
//												
//		A)	1. Add this file to project	  or	B)	1.	Add this file to avr/include
//			2. #include "avriomacros.h"				2.	#include <avriomacros.h>
//
// Example usage to turn an LED on with a pushbutton:
/*

#include <avr/io.h>
#include "iomacros.h"

#define LED		3,B				// declare LED pin and port (always specify pin first)
#define BUTTON	2,B				// declare BUTTON pin and port

int main (void) {

	out(LED);					// set LED as an output
	in(BUTTON);					// set BUTTON as an input
	pullup(BUTTON);				// activate internal pullup resistor for BUTTON
	
	while(1) {					// forever loop

		if(get(BUTTON) == 0)	// if button is pressed...
			on(LED);			//     turn on LED
		else					// otherwise
			off(LED);			//     turn off LED
	}
}

*/
//***********************************************************************************

/* Macro function to declare an output pin */
#define out(x)			_out(x)
#define _out(bit,port)	DDR##port |= (1 << bit)

/* Macro function to declare an input pin */
#define in(x)			_in(x)
#define _in(bit,port)	DDR##port &= ~(1 << bit)

/* Macro function to set an output pin high */
#define on(x)			_on(x)
#define _on(bit,port)	PORT##port |= (1 << bit)

/* Macro function to set an output pin low */
#define off(x)			_off(x)
#define _off(bit,port)	PORT##port &= ~(1 << bit)

/* Macro function to toggle an output pin */
#define flip(x)			_flip(x)
#define _flip(bit,port)	PORT##port ^= (1 << bit)

/* Macro function to set internal pullup resistor of input pin (same as "on" macro)*/
#define pullup(x)		_on(x)

/* Macro function to get state of input pin */
#define get(x)			_get(x)
#define _get(bit,port)	(PIN##port & (1 << bit))













