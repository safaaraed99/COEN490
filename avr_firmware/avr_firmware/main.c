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

// Number of timer 3 overflows to occur for ~0.5s of real time to pass
#define TC3_OVF_HALF_SECOND 61

extern volatile bool motor_faulted[MOTOR_COUNT];

static volatile int8_t motor_cycles_remaining[MOTOR_COUNT];

void setup_gpio(void);
int8_t check_finger_flexion(adc_readings_t *current, adc_readings_t *prev, potentiometer knuckle1, potentiometer knuckle2, potentiometer knuckle3);

// REAL MAIN
int main(void)
{
	// SETUP
	setup_gpio();
	setup_spi();
	setup_uart();
	setup_motors();
	
	// Enable timer 3 to track motor active time
	// No prescaling
	TCCR3B = (1<<CS30);
	// Overflow interrupt enabled
	TIMSK3 = (1<<TOIE3);
	
	// Current and previous ADC readings, used to detect movement
	adc_readings_t current_readings;
	adc_readings_t old_readings;
	memset(&current_readings, 0, sizeof(adc_readings_t));
	memset(&old_readings, 0, sizeof(adc_readings_t));
	
	bool exercise_started = false;
	uint8_t motor_speed = 100;
	
	potentiometer pot_index = POT_THUMB_1;
	
	char recvbuf[64] = {0};
		
	// Used to prevent triggering the motors until 255 iterations of the main loop have happened, while the filters stabilize.
	uint8_t stabilize_delay = 255;
	
	sei();
	set_motor_enable(1);
	// LOOP
	while (1)
	{
		// Check for motor faults
		for (size_t i = 0; i < MOTOR_COUNT; i++)
		{
			if (motor_faulted[i])
			{
				exercise_started = false;
				bt_send_motor_warning(i);
			}
		}
		
		// Check for incoming commands
		// N.B. This should normally use the BT UART, but we are using an external module on the debug UART for the final demo because of timing issues.
		debug_recv(recvbuf, 64);
		if (recvbuf[0] == 0x85 && !exercise_started)
		{
			// Set resistance, only while exercise is stopped.
			// Higher resistance value => lower motor speed (smaller value set to 8 bit output compare register)
			switch (recvbuf[1])
			{
				case 1:
					motor_speed = 200;
					break;
				case 2:
					motor_speed = 175;
					break;
				case 3:
					motor_speed = 150;
					break;
				case 4:
					motor_speed = 125;
					break;
				case 5:
					motor_speed = 100;
					break;
				default:
					break;
			}
		}
		else if (recvbuf[0] == 0x01)
		{
			// Start exercise
			exercise_started = true;
			//set_motor_enable(1);
		}
		else if (recvbuf[0] == 0x82)
		{
			// Stop exercise
			exercise_started = false;
			//set_motor_enable(0);
		}
		
		// Read all pots and motors
		memcpy(&old_readings, &current_readings, sizeof(adc_readings_t));
		for (potentiometer i = POT_THUMB_1; i <= POT_PINKY_3; i++)
		{
			read_pot(i, &current_readings);
		}
		for (motor i = MOTOR_PINKY; i <= MOTOR_THUMB; i++)
		{
			read_motor(i, &current_readings);
		}

		// Wait for all the filters to stabilize before doing anything else
		if (stabilize_delay > 0)
		{
			stabilize_delay--;
			continue;
		}
		
		// Only send two readings per iteration to not overflow the send buffer
		bt_send_reading(pot_index, current_readings.potentiometers[pot_index] >> POT_FILTER_SHIFT);
		bt_send_reading(pot_index+1, current_readings.potentiometers[pot_index+1] >> POT_FILTER_SHIFT);
		pot_index += 2;
		if (pot_index > POT_PINKY_3)
		{
			pot_index = POT_THUMB_1;
		}
		
		int8_t flexion[MOTOR_COUNT];
		flexion[MOTOR_PINKY] = check_finger_flexion(&current_readings, &old_readings, POT_PINKY_1, POT_PINKY_2, POT_PINKY_3);
		flexion[MOTOR_RING] = check_finger_flexion(&current_readings, &old_readings, POT_RING_1, POT_RING_2, POT_RING_3);
		flexion[MOTOR_MIDDLE] = check_finger_flexion(&current_readings, &old_readings, POT_MIDDLE_1, POT_MIDDLE_2, POT_MIDDLE_3);
		flexion[MOTOR_INDEX] = check_finger_flexion(&current_readings, &old_readings, POT_INDEX_1, POT_INDEX_2, POT_INDEX_3);
		flexion[MOTOR_THUMB] = check_finger_flexion(&current_readings, &old_readings, POT_THUMB_1, POT_THUMB_2, POT_THUMB_2);
		
		for (motor i = MOTOR_PINKY; i <= MOTOR_THUMB; i++)
		{
			if (motor_cycles_remaining[i] > 0)
			{
				continue;
			}
			
			if (flexion[i] > 0)
			{
				set_motor_phase(i, DIRECTION_FORWARD);
				set_motor_speed(i, motor_speed);
				motor_cycles_remaining[i] = TC3_OVF_HALF_SECOND;
			}
			else if (flexion[i] < 0)
			{
				set_motor_phase(i, DIRECTION_BACKWARD);
				set_motor_speed(i, motor_speed);
				motor_cycles_remaining[i] = TC3_OVF_HALF_SECOND;
			}
			else
			{
				set_motor_speed(i, 0);
			}
		}
		
		_delay_ms(10);
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
	
	char sendbuf[64] = {0};
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
			_delay_ms(250);
			// Save old reading
			old_readings.motors[index] = current_readings.motors[index];
			// Get new reading
			read_motor(index, &current_readings);
			// Print debug message
			snprintf(sendbuf, 64, "Motor %u read %u (prev %u). Dir %d\n", index, current_readings.motors[index], old_readings.motors[index], direction);
			debug_send(sendbuf);
		}
		
		set_motor_speed(index, 0);
		if (direction == DIRECTION_BACKWARD)
		{
			direction = DIRECTION_FORWARD;
		}
		else
		{
			direction = DIRECTION_BACKWARD;
		}
			
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

// Check if a finger is flexing. Returns 1 if it's more flexed than the previous iteration, -1 if it's less flexed, 0 if unchanged.
int8_t check_finger_flexion(adc_readings_t *current, adc_readings_t *prev, potentiometer knuckle1, potentiometer knuckle2, potentiometer knuckle3)
{
	if (current->potentiometers[knuckle1] >> POT_FILTER_SHIFT < prev->potentiometers[knuckle1] >> POT_FILTER_SHIFT || 
		current->potentiometers[knuckle2] >> POT_FILTER_SHIFT < prev->potentiometers[knuckle2] >> POT_FILTER_SHIFT || 
		current->potentiometers[knuckle3] >> POT_FILTER_SHIFT < prev->potentiometers[knuckle3] >> POT_FILTER_SHIFT)
	{
		return 1;
	}
	else if (current->potentiometers[knuckle1] >> POT_FILTER_SHIFT > prev->potentiometers[knuckle1] >> POT_FILTER_SHIFT || 
			 current->potentiometers[knuckle2] >> POT_FILTER_SHIFT > prev->potentiometers[knuckle2] >> POT_FILTER_SHIFT || 
			 current->potentiometers[knuckle3] >> POT_FILTER_SHIFT > prev->potentiometers[knuckle3] >> POT_FILTER_SHIFT)
	{
		return -1;
	}
	return 0;
}

ISR(TIMER3_OVF_vect)
{
	for (motor i = MOTOR_PINKY; i <= MOTOR_THUMB; i++)
	{
		if (motor_cycles_remaining[i] > 0)
		{
			motor_cycles_remaining[i]--;
		}
	}
}
