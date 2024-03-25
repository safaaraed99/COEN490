/*
 * avr_firmware.c
 *
 * Created: 2024-03-10 5:43:47 PM
 * Author : Matthew Faigan
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>

#include "spi.h"
#include "uart.h"
#include "motor.h"

void setup_gpio(void);

// READ ME!!!!!!!!
// Uncomment ONLY ONE of the below main functions. Each one is commented with what it does.
// There's the real main function for the final firmware and a bunch of tests for individual features.
// Make sure you build after uncommenting the one you want and commenting the others, then upload.


/*
// DEBUG TEST
int main(void)
{
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	char sendbuf[50] = {0};
	char recvbuf[50] = {0};
	
	sei();
	while (1)
	{
		// Write a bunch of data
		for (int i = 0; i < 10; i++)
		{
			for (int j = 10; j > 0; j--)
			{
				int sum = i + j;
				int diff = i - j;
				snprintf(sendbuf, 50, "%d + %d = %d; %d - %d = %d", i, j, sum, i, j, diff);
				debug_send(sendbuf);
				
				// Busy wait, try reducing the limit to see how fast the data is pumped out of the debug send buffer
				_delay_ms(100);
			}
		}
		
		// Echo back anything received
		size_t recv_cnt = debug_recv(recvbuf, 49);
		if (recv_cnt)
		{
			debug_send(recvbuf);
		}
	}
}
*/

/*
// SPI TEST
int main(void)
{
	// SETUP
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	adc_readings_t current_readings;
	adc_readings_t old_readings;
	
	memset(&current_readings, 0, sizeof(adc_readings_t));
	memset(&old_readings, 0, sizeof(adc_readings_t));
	
	char sendbuf[50] = {0};
	char recvbuf[50] = {0};
		
	uint8_t index = 0;
	
	sei();
	// LOOP
	while (1)
	{
		size_t bytes_read = debug_recv(recvbuf, 49);
		if (bytes_read > 0)
		{
			int conv = atoi(recvbuf);
			if (conv >= 0 && conv <= 13)
			{
				index = (uint8_t)conv;
				snprintf(sendbuf, 49, "Set index to %d", index);
				debug_send(sendbuf);
			}
			else
			{
				strncpy(sendbuf, "Index malformed or out of range", 49);
				debug_send(sendbuf);
			}
		}
		
		// Save old reading
		old_readings.potentiometers[index] = current_readings.potentiometers[index];
		// Get new reading
		read_pot(index, &current_readings);
		// Print debug message
		snprintf(sendbuf, 49, "Pot %d read %d (prev %d)", index, current_readings.potentiometers[index], old_readings.potentiometers[index]);
		debug_send(sendbuf);
		
		_delay_ms(10);
	}
}
*/


// MOTOR TEST
int main(void)
{
	// SETUP
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	adc_readings_t current_readings;
	adc_readings_t old_readings;
	
	memset(&current_readings, 0, sizeof(adc_readings_t));
	memset(&old_readings, 0, sizeof(adc_readings_t));
	
	char sendbuf[50] = {0};
	char recvbuf[50] = {0};
	
	uint8_t index = 0;
	motor_direction direction = DIRECTION_FORWARD;
	
	sei();
	// LOOP
	while (1)
	{
		size_t bytes_read = debug_recv(recvbuf, 49);
		if (bytes_read > 0)
		{
			int conv = atoi(recvbuf);
			if (conv >= 0 && conv <= 4)
			{
				index = (uint8_t)conv;
				snprintf(sendbuf, 49, "Set index to %d", index);
				debug_send(sendbuf);
			}
			else
			{
				strncpy(sendbuf, "Index malformed or out of range", 49);
				debug_send(sendbuf);
			}
		}
		
		
		
		// Save old reading
		old_readings.potentiometers[index] = current_readings.potentiometers[index];
		// Get new reading
		read_pot(index, &current_readings);
		// Print debug message
		snprintf(sendbuf, 49, "Pot %d read %d (prev %d)", index, current_readings.potentiometers[index], old_readings.potentiometers[index]);
		debug_send(sendbuf);
		
		_delay_ms(10);
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

