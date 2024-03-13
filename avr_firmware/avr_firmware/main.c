/*
 * avr_firmware.c
 *
 * Created: 2024-03-10 5:43:47 PM
 * Author : matt_
 */ 

#include <avr/io.h>

#include "spi.h"

void setup_gpio(void);

int main(void)
{
	// SETUP
	setup_gpio();
	
	// LOOP
	while (1)
	{
	}
}

void setup_gpio(void)
{
	// PORTxn : If port x, pin n is input: 1 enables internal pull-up. If port x, pin n is output: sets value of port.
	// DDxn : Data Direction, Port x, Pin n. 1 = Output, 0 = Input
	PORTB = (1<<PORTB7) | (1<<PORTB6);
	DDRB = (1<<DDB0);
	
	PORTC = (1<<PORTC5) | (1<<PORTC3) | (1<<PORTC2);
	DDRC = (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2);
	
	PORTD = (1<<PORTD4);
	DDRD = (1<<DDD7) | (1<<DDD2);
	
	PORTE = (1<<PORTE2) | (1<<PORTE1) | (1<<PORTE0);
	DDRE = (1<<DDE2);
}

