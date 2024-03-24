/*
 * spi.c
 *
 * Created: 2024-03-13 4:24:27 PM
 *  Author: Matthew Faigan
 */ 

#include "spi.h"
#include <stdlib.h>

void setup_spi(void)
{
	// Set MOSI1 and SCK1 output
	DDRC |= (1<<DDC1);
	DDRE |= (1<<DDE3);
	// Enable SPI1 in master mode, MSB first, CPOL = 0, CPHA = 0, clock division factor = 16
	// N.B. I put /16 here as a default, we can try ramping up the speed later if everything is stable
	SPCR1 = (1<<SPE1) | (1<<MSTR1) | (1<<SPR1);
}

int read_pot(potentiometer pot_index, adc_readings_t *dest)
{
	if (pot_index > POT_PINKY_3 || dest == NULL)
	{
		return 1;
	}
	
	// 7 channels used per ADC
	uint8_t adc_num = pot_index / 7;
	uint8_t adc_ch = pot_index % 7;
	
	if (toggle_adc_ss(adc_num)) return 1;
	
	uint16_t result;
	if (read(adc_ch, &result)) return 1;
	if (toggle_adc_ss(adc_num)) return 1;
	
	// Convert the reading to fixed point
	result <<= POT_FILTER_SHIFT;
	// Save the previous reading from dest
	uint16_t prev_out = dest->potentiometers[pot_index];
	// Perform the filtering operation and store the new filter output
	dest->potentiometers[pot_index] = prev_out + ((result - prev_out) >> POT_FILTER_SHIFT);
	return 0;
}

int read_motor(motor motor_index, adc_readings_t *dest)
{
	if (motor_index > MOTOR_THUMB || dest == NULL)
	{
		return 1;
	}
	
	if (toggle_adc_ss(2)) return 1;
	
	uint16_t result;
	if (read(motor_index, &result)) return 1;
	if (toggle_adc_ss(2)) return 1;
	
	dest->motors[motor_index] = result;
	return 0;
}

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
	
	uint16_t result;
	
	// 1. Send 0b00000001. The final bit acts as the start bit to the ADC.
	SPDR1 = 0x01;
	while (!(SPSR1 & (1<<SPIF1)));
	
	// 2. Send 0b1XXX0000, where XXX is the 3-bit channel number.
	// The MSB is 1 to indicate single ended conversion as opposed to differential pair.
	// The remaining bits are don't-care bits that are left as 0 for simplicity.
	// The ADC will respond with the most significant 2 bits of the converted value.
	uint8_t byte2 = 0b10000000;
	byte2 |= (channel_num << 4) & 0b01110000;
	SPDR1 = byte2;
	
	while (!(SPSR1 & (1<<SPIF1)));
	result = (SPDR1 & 0b00000011) << 8;
	
	// 3. Send 0b00000000. This entire byte is a don't-care byte, but we need to send something to keep the clock going while we read the response.
	// The ADC will respond with the remaining 8 bits.
	SPDR1 = 0;
	while (!(SPSR1 & (1<<SPIF1)));
	result |= SPDR1;
	
	*dest = result;
	return 0;
}
