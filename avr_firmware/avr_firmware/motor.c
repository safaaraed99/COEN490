/*
 * motor.c
 *
 * Created: 2024-03-19 1:37:31 PM
 *  Author: Matthew Faigan
 */ 

#include "motor.h"

#include <avr/io.h>

// TODO motor fault ISRs

void setup_motors(void)
{
	// N.B. The different hardware counters have different capabilities and configuration methods.
	// Before you modify any of the setup code here, make sure you read the appropriate section in the datasheet.
	
	/* TC0 (thumb and index finger) */
	// Set PD5 and PD6 as outputs
	DDRD |= (1<<DDD6) | (1<<DDD5);
	// Set OC0A and OC0B to fast PWM, non-inverting
	TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
	// Set no prescaling
	TCCR0B = (1<<CS00);
	// Default both output compares to 0
	OCR0A = 0;
	OCR0B = 0;
	
	/* TC1 (middle and ring fingers) */
	// Set PB1 and PB2 as outputs
	DDRB |= (1<<DDB2) | (1<<DDB1);
	// Set fast PWM non-inverting with top value of 0x00FF, no prescaling
	TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10);
	TCCR1B = (1<<WGM12) | (1<<CS10);
	// Default output compares to 0
	OCR1A = 0;
	OCR1B = 0;
	
	/* TC2 (pinky) */
	// Set PD3 as output
	DDRD |= (1<<DDD3);
	// Leave OC2A unused, set OC2B to fast PWM non-inverting
	TCCR2A = (1<<COM2B1) | (1<<WGM21) | (1<<WGM20);
	// No prescaling
	TCCR2B = (1<<CS20);
	// Default output compare to 0
	OCR2B = 0;
}

int set_motor_speed(motor motor_num, uint8_t duty)
{
	switch (motor_num)
	{
		case MOTOR_THUMB:
			OCR0A = duty;
			break;
		case MOTOR_INDEX:
			OCR0B = duty;
			break;
		case MOTOR_MIDDLE:
			OCR1A = duty;
			break;
		case MOTOR_RING:
			OCR1B = duty;
			break;
		case MOTOR_PINKY:
			OCR2B = duty;
			break;
		default:
			return 1;
	}
	return 0;
}

// This is ugly but I don't know a better way to do it!!
int set_motor_phase(motor motor_num, motor_direction direction)
{
	// Get the current value of the register containing the bit to change
	uint8_t current_value;
	uint8_t mask;
	switch (motor_num)
	{
		case MOTOR_THUMB:
			current_value = PORTD;
			mask = 1<<PORTD2;
			break;
		case MOTOR_INDEX:
		case MOTOR_MIDDLE:
			current_value = PORTC;
			mask = 1<<PORTC4;
			break;
		case MOTOR_RING:
		case MOTOR_PINKY:
			current_value = PORTD;
			mask = 1<<PORTD7;
			break;
		default:
			return 1;
	}
	
	// Motor has forward direction if PH = 1, backwards if PH = 0
	uint8_t new_value;
	switch (direction)
	{
		case DIRECTION_FORWARD:
			new_value = current_value | mask;
			break;
		case DIRECTION_BACKWARD:
			new_value = current_value & ~mask;
			break;
		default:
			return 1;
	}
	
	// Set the new value
	switch (motor_num)
	{
		case MOTOR_THUMB:
		case MOTOR_RING:
		case MOTOR_PINKY:
			PORTD = new_value;
			break;
		case MOTOR_INDEX:
		case MOTOR_MIDDLE:
			PORTC = new_value;
			break;
		default:
			return 1;
	}
	return 0;
}
