/*
 * spi.c
 *
 * Created: 2024-03-13 4:24:27 PM
 *  Author: matt_
 */ 

#include "spi.h"

int toggle_adc_ss(uint8_t adc_num)
{	
	switch (adc_num)
	{
	case 0:
		PORTE ^= (1<<PORTE2);
		break;
	case 1:
		PORTC ^= (1<<PORTC2);
		break;
	case 2:
		PORTC ^= (1<<PORTC3);
		break;
	default:
		// ADC number out of range
		return 1;
	}
	return 0;
}

int read(uint8_t channel_num, uint16_t *dest)
{
	if (channel_num > 7)
	{
		return 1;
	}
}
