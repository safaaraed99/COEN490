/*
 * circular_buffer.c
 *
 * Created: 2024-03-16 2:47:19 PM
 *  Author: Matthew Faigan
 */ 

#include "circular_buffer.h"

#include <string.h>

void circ_buf_init(volatile circular_buffer_t *buf)
{
	buf->read_idx = 0;
	buf->write_idx = 0;
}

void circ_buf_write(volatile circular_buffer_t *buf, char* data)
{
	circ_buf_write(buf, data, strlen(data));
}

void circ_buf_write(volatile circular_buffer_t *buf, char* data, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		// Write from the data input to the buffer, and wrap the write index back around to 0 if it reaches BUF_SIZE.
		buf->buffer[buf->write_idx] = data[i];
		// Since BUF_SIZE is power of 2, BUF_SIZE - 1 is all 1s in binary.
		// Therefore when write_idx reaches BUF_SIZE, the AND operation results in 0. In any other case, the value is unchanged.
		// This is way faster than doing modulo of an arbitrary number.
		buf->write_idx = (buf->write_idx + 1) & (BUF_SIZE - 1);
	}
}

size_t circ_buf_read(volatile circular_buffer_t *buf, char* out, size_t out_len)
{
	for (size_t i = 0; i < out_len; i++)
	{
		// Return immediately if there's nothing left to read
		if (circ_buf_get_len(buf) == 0) return i;
		
		// Copy a byte out of the buffer and increment, wrapping around if necessary
		out[i] = buf->buffer[buf->read_idx];
		buf->read_idx = (buf->read_idx + 1) & (BUF_SIZE - 1);
	}
	return out_len;
}

size_t circ_buf_get_len(volatile circular_buffer_t *buf)
{
	// Current length of the circular buffer is equivalent to (write - read) mod BUF_SIZE
	if (buf->write_idx >= buf->read_idx)
	{
		return buf->write_idx - buf->read_idx;
	}
	else
	{
		return BUF_SIZE + buf->write_idx - buf->read_idx;
	}
}
