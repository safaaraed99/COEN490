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
#include <stdbool.h>
#include <util/delay.h>

#include "spi.h"
#include "uart.h"
#include "motor.h"

static volatile bool exercise_started = false;

extern volatile bool motor_active[5];
extern volatile motor_direction motor_directions[5];
extern volatile int64_t motor_active_cycles[5];
extern volatile bool motor_faulted[5];

void setup_gpio(void);

// READ ME!!!!!!!!
// Uncomment ONLY ONE of the below main functions. Each one is commented with what it does.
// There's the real main function for the final firmware and a bunch of tests for individual features.
// Make sure you build after uncommenting the one you want and commenting the others, then upload.

// REAL MAIN
int main(void)
{
	// SETUP
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	// Current and previous ADC readings, used to detect movement
	adc_readings_t current_readings;
	adc_readings_t old_readings;
	
	memset(&current_readings, 0, sizeof(adc_readings_t));
	memset(&old_readings, 0, sizeof(adc_readings_t));
	
	// Next ADC values to query (one per iteration of the main loop)
	potentiometer pot_index = POT_THUMB_1;
	motor motor_index = MOTOR_PINKY;
	
	sei();
	// LOOP
	while (1)
	{
		// Check for motor faults
		for (size_t i = 0; i < MOTOR_COUNT; i++)
		{
			if (motor_faulted[i])
			{
				bt_send_motor_warning(i);
			}
		}
		
		// Get a reading
		if (pot_index <= POT_PINKY_3)
		{
			old_readings.potentiometers[pot_index] = current_readings.potentiometers[pot_index];
			read_pot(pot_index, &current_readings);
			bt_send_reading(pot_index, current_readings.potentiometers[pot_index]);
			pot_index++;
		}
		else if (motor_index <= MOTOR_THUMB)
		{
			old_readings.motors[motor_index] = current_readings.motors[motor_index];
			read_motor(motor_index, &current_readings);
			motor_index++;
		}
		else
		{
			pot_index = POT_THUMB_1;
			motor_index = MOTOR_PINKY;
		}
		
		// Check for incoming commands
	}
}

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
				snprintf(sendbuf, 50, "%d + %d = %d; %d - %d = %d\n", i, j, sum, i, j, diff);
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
// BT TEST
int main(void)
{
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	char sendbuf[50] = {0};
	//char recvbuf[50] = {0};
	
	sei();
	while (1)
	{
		//char test[2] = {0b01010101, 0};
		strncpy(sendbuf, "ADD\n", 9);
		debug_send(sendbuf);
		//debug_send(sendbuf);
		
		//// Echo back anything received
		//size_t recv_cnt = bt_recv(recvbuf, 49);
		//if (recv_cnt)
		//{
			//bt_send(recvbuf);
			//debug_send(recvbuf);
		//}
		
		_delay_ms(1000);
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
				snprintf(sendbuf, 49, "Set index to %d\n", index);
				debug_send(sendbuf);
			}
			else
			{
				strncpy(sendbuf, "Index malformed or out of range\n", 49);
				debug_send(sendbuf);
			}
		}
		
		// Save old reading
		old_readings.potentiometers[index] = current_readings.potentiometers[index];
		// Get new reading
		read_pot(index, &current_readings);
		// Print debug message
		snprintf(sendbuf, 49, "Pot %d read %d (prev %d)\n", index, current_readings.potentiometers[index] >> POT_FILTER_SHIFT, old_readings.potentiometers[index] >> POT_FILTER_SHIFT);
		debug_send(sendbuf);
		
		_delay_ms(1000);
	}
}
*/

/*
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
	set_motor_enable(1);
	// LOOP
	while (1)
	{
		size_t bytes_read = debug_recv(recvbuf, 49);
		if (bytes_read > 0)
		{
			int conv = atoi(recvbuf);
			if (conv >= 0 && conv <= 4)
			{
				set_motor_speed(index, 0);
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
		
		set_motor_phase(index, direction);
		for (int spd = 0; spd < 200; spd += 5)
		{
			set_motor_speed(index, spd);
			_delay_ms(1000);
		}
		
		if (direction == DIRECTION_BACKWARD)
		{
			direction = DIRECTION_FORWARD;
		}
		else
		{
			direction = DIRECTION_BACKWARD;
		}
		
		// Save old reading
		old_readings.motors[index] = current_readings.motors[index];
		// Get new reading
		read_motor(index, &current_readings);
		// Print debug message
		snprintf(sendbuf, 49, "Motor %u read %u (prev %u)\n", index, current_readings.motors[index], old_readings.motors[index]);
		debug_send(sendbuf);
		
		_delay_ms(10);
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



