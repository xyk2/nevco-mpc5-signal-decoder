#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "iomacros.h"

/*
	ATmega328 Fuse Bits: -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
	ATTiny85 Fuse Bits: -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
	16MHz clock
*/

#if defined (__AVR_ATmega328__)
	#define INT0_PIN 2,D
	#define CLOCK_PULSE 5,D
	#define START_BIT_PULSE 6,D

#elif defined (__AVR_ATtiny85__)
	#define INT0_PIN 2,B
	#define CLOCK_PULSE 1,B
	#define START_BIT_PULSE 0,B

#endif


static volatile uint8_t clocks = 0;
static volatile uint8_t enableInterrupt = 1;

int main(void) {
	out(CLOCK_PULSE);
	out(START_BIT_PULSE);
	in(INT0_PIN);

	sei(); // Enable interrupts

	#if defined (__AVR_ATmega328__)
		EICRA |= (1 << ISC00); // Trigger on rising edge
		EICRA |= (1 << ISC01);
		EIMSK |= (1 << INT0);  // Enable INT0 interrupt vector

	#elif defined (__AVR_ATtiny85__)
		MCUCR |= (1 << ISC00); // Trigger on rising edge
		MCUCR |= (1 << ISC01);
		GIMSK |= (1 << INT0);  // Enable INT0 interrupt vector

	#endif

	while(1) {
		clocks = 0;

		// Detect start bit - 7us pulse
		while(get(INT0_PIN)) {
			clocks++;

			if(clocks > 4) {
				if(enableInterrupt == 10) {
					on(START_BIT_PULSE); // Send start bit pulse
					out(CLOCK_PULSE); // Enable interrupt clock OUTPUT
					enableInterrupt = 0; // Reset counter
					__builtin_avr_delay_cycles(50);
					off(START_BIT_PULSE);

				} else {
					in(CLOCK_PULSE); // DISABLE interrupt clock OUTPUT
					enableInterrupt++;
				}

				break;
			}
		}
	}
}


ISR(INT0_vect, ISR_NAKED) {
	asm( // Standard ISR entry, minus "known zero" setup
		"push r0\n"
		"in r0, 0x3F\n"  // Save status
		"push r0" // Might not be needed
	);
	on(CLOCK_PULSE);
	__builtin_avr_delay_cycles(5);
	off(CLOCK_PULSE);


	asm( // low-overhead ISR exit...
		"pop r0\n"  // matches "might not be needed"
		"out 0x3f, r0\n"  // restore status
		"pop r0\n"  // restore r0
		"reti\n"
	);
}

