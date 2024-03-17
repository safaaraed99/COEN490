/*
 * circular_buffer.h
 *
 * Created: 2024-03-16 2:44:39 PM
 *  Author: matt_
 */ 


#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdlib.h>

// This value must be a power of 2 so that the modulus operation is fast when we wrap around
#define BUF_SIZE 64

typedef struct circular_buffer
{
	char buffer[BUF_SIZE];
	size_t read_idx;
	size_t write_idx;
} circular_buffer_t;

void circ_buf_init(volatile circular_buffer_t *buf);

void circ_buf_write(volatile circular_buffer_t *buf, char* data);

size_t circ_buf_read(volatile circular_buffer_t *buf, char* out, size_t out_len);

size_t circ_buf_get_len(volatile circular_buffer_t *buf);

#endif /* CIRCULAR_BUFFER_H_ */