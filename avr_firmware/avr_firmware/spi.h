/*
 * spi.h
 *
 * Created: 2024-03-13 4:24:38 PM
 *  Author: matt_
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <stdint.h>

enum 
{
	POT_THUMB_1 = 0,
	POT_THUMB_2 = 1,
	POT_INDEX_1 = 2,
	POT_INDEX_2 = 3,
	POT_INDEX_3 = 4,
	POT_MIDDLE_1 = 5,
	POT_MIDDLE_2 = 6,
	POT_MIDDLE_3 = 7,
	POT_RING_1 = 8,
	POT_RING_2 = 9,
	POT_RING_3 = 10,
	POT_PINKY_1 = 11,
	POT_PINKY_2 = 12,
	POT_PINKY_3 = 13
};

typedef struct adc_readings 
{
	uint16_t potentiometers[14];
	uint16_t motors[5];
} adc_readings_t;

void setup_spi(void);

/**
 * \brief Reads the potentiometer with the specified index and stores the result in an adc_readings_t struct.
 * 
 * \param pot_index The potentiometer index to read, 0-13.
 * \param dest The destination structure to store the result in.
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int read_pot(uint8_t pot_index, adc_readings_t *dest);

/**
 * \brief Reads the motor current with the specified index and stores the result in an adc_readings_t struct.
 * 
 * \param pot_index The motor index to read, 0-4.
 * \param dest The destination structure to store the result in.
 * 
 * \return int 0 if the operation was successful. Nonzero indicates an argument out of range.
 */
int read_motor(uint8_t motor_index, adc_readings_t *dest);

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