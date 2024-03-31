/*
 * circular_buffer.h
 *
 * Created: 2024-03-16 2:44:39 PM
 *  Author: Matthew Faigan
 */ 


#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdlib.h>

// This value must be a power of 2 so that the modulus operation is fast when we wrap around.
#define BUF_SIZE 64

// Defines a circular buffer used to send and receive data from the UARTs.
typedef struct circular_buffer
{
	char buffer[BUF_SIZE];
	size_t read_idx;
	size_t write_idx;
} circular_buffer_t;

/**
 * \brief Initializes a newly created circular_buffer_t instance. Should be called before doing anything else with the object.
 * 
 * \param buf A pointer to the buffer to initialize.
 * 
 * \return void
 */
void circ_buf_init(volatile circular_buffer_t *buf);

/**
 * \brief Writes data into a circular buffer. Does not perform bounds checking; old data may be overwritten.
 * 
 * \param buf A pointer to the buffer to write into.
 * \param data A null-terminated string of data to write.
 * 
 * \return void
 */
void circ_buf_write(volatile circular_buffer_t *buf, char* data);

/**
 * \brief Writes data into a circular buffer. Does not perform bounds checking; old data may be overwritten.
 * 
 * \param buf A pointer to the buffer to write into.
 * \param data A buffer of data to write.
 * \param len The number of bytes to write.
 * 
 * \return void
 */
void circ_buf_write(volatile circular_buffer_t *buf, char* data, size_t len);

/**
 * \brief Reads up to a given number of characters from a circular buffer. Returns early if there is no more data to read.
 * 
 * \param buf The buffer to read from.
 * \param out An array to copy the read bytes into.
 * \param out_len The maximum number of bytes to read.
 * 
 * \return size_t The actual number of bytes that were read.
 */
size_t circ_buf_read(volatile circular_buffer_t *buf, char* out, size_t out_len);

/**
 * \brief Returns the amount of data in the buffer that can be read.
 * 
 * \param buf The buffer to check.
 * 
 * \return size_t The number of bytes that are valid to read from the buffer.
 */
size_t circ_buf_get_len(volatile circular_buffer_t *buf);

#endif /* CIRCULAR_BUFFER_H_ */