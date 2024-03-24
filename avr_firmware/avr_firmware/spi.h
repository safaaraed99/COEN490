/*
 * spi.h
 *
 * Created: 2024-03-13 4:24:38 PM
 *  Author: Matthew Faigan
 */ 


#ifndef SPI_H_
#define SPI_H_

// Determines the scaling factor used for the noise filtering on the potentiometer readings.
// The scaling factor is 1/2^N where N is the value defined here.
// The division is implemented by a right shift.
#define POT_FILTER_SHIFT 3

#include <avr/io.h>
#include <stdint.h>

#include "glove_enums.h"

typedef struct adc_readings 
{
	uint16_t potentiometers[14];
	uint16_t motors[5];
} adc_readings_t;

/**
 * \brief Configures the pins and registers for SPI1.
 * 
 * \param 
 * 
 * \return void
 */
void setup_spi(void);

/**
 * \brief Reads the potentiometer with the specified index and stores the result in an adc_readings_t struct.
 * 
 * \param pot_index The potentiometer index to read, 0-13.
 * \param dest The destination structure to store the result in.
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int read_pot(potentiometer pot_index, adc_readings_t *dest);

/**
 * \brief Reads the motor current with the specified index and stores the result in an adc_readings_t struct.
 * 
 * \param pot_index The motor index to read, 0-4.
 * \param dest The destination structure to store the result in.
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int read_motor(motor motor_index, adc_readings_t *dest);

/**
 * \brief Toggles the slave select/chip select (SS/CS) pin of a particular ADC. SS pins are active low and idle high. Only one SS pin should be low at a time.
 * 
 * \param adc_num The ADC to set (0-2).
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int toggle_adc_ss(uint8_t adc_num);

/**
 * \brief Issues commands to read a particular channel, and then stores the returned data at a destination address. The corresponding SS pin must already be low.
 * 
 * \param channel_num The channel to read (0-7).
 * \param dest A memory address to store the returned value in.
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int read(uint8_t channel_num, uint16_t *dest);

#endif /* SPI_H_ */