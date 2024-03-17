/*
 * avr_firmware.c
 *
 * Created: 2024-03-10 5:43:47 PM
 * Author : matt_
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "spi.h"
#include "uart.h"

void setup_gpio(void);

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

