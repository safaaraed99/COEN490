/*
 * avr_firmware.c
 *
 * Created: 2024-03-10 5:43:47 PM
 * Author : Matthew Faigan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "spi.h"
#include "uart.h"

void setup_gpio(void);

// READ ME!!!!!!!!
// Uncomment ONLY ONE of the below main functions. Each one is commented with what it does.
// There's the real main function for the final firmware and a bunch of tests for individual features.
// Make sure you build after uncommenting the one you want and commenting the others, then upload.

// DEBUG TEST
int main(void)
{
	setup_gpio();
	setup_spi();
	setup_uart();
	
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
				snprintf(sendbuf, 49, "%d2 + %d2 = %d2; %d2 - %d2 = %d2", i, j, sum, i, j, diff);
				debug_send(sendbuf);
				
				// Busy wait, try reducing the limit to see how fast the data is pumped out of the debug send buffer
				for (uint32_t wait = 0; wait < 1000000; wait++);
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

/*
// REAL MAIN
int main(void)
{
	// SETUP
	setup_gpio();
	setup_spi();
	setup_uart();
	
	adc_readings_t current_readings;
	adc_readings_t old_readings;
	uint8_t pot_index = 0;
	uint8_t motor_index = 0;
	
	memset(&current_readings, 0, sizeof(adc_readings_t));
	memset(&old_readings, 0, sizeof(adc_readings_t));
	
	sei();
	// LOOP
	while (1)
	{
		// ADC reading
		// Read one value per iteration of the main loop to not block for too long
		if (pot_index <= POT_PINKY_3)
		{
			old_readings.potentiometers[pot_index] = current_readings.potentiometers[pot_index];
			read_pot(pot_index, &current_readings);
			pot_index++;
		}
		else if (motor_index <= MOTOR_THUMB)
		{
			old_readings.motors[motor_index] = current_readings.motors[motor_index];
			read_motor(motor_index, &current_readings);
			motor_index++;
		}
		
		if (pot_index > POT_PINKY_3 && motor_index > MOTOR_THUMB)
		{
			pot_index = 0;
			motor_index = 0;
		}
	}
}
*/

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

