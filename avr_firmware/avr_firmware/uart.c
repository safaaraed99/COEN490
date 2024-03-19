/*
 * uart.c
 *
 * Created: 2024-03-16 2:15:14 PM
 *  Author: matt_
 */ 

#include "uart.h"

#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "circular_buffer.h"

// The clock rate of the system is 8 MHz.
// When not running the UART at double speed, UBRR = f_osc / (16*Baud) - 1
// The below value is for 19200 baud.
#define DEBUG_UBRR 25

// Circular buffers for debug send and receive.
// When the main loop sends data, it's copied into the buffer.
// Data is asynchronously transmitted out of the buffer in the background using interrupts.
static volatile circular_buffer_t debug_send_buf;
static volatile circular_buffer_t debug_recv_buf;

void setup_uart(void)
{
	// Initialize circular buffers for debug
	circ_buf_init(&debug_send_buf);
	circ_buf_init(&debug_recv_buf);
	
	// TODO Configure USART0 registers (BT module)
	
	/* UART1 (Debug serial) initialization */
	// Enable RX complete, TX data register empty interrupts. Enable receiver and transmitter.
	UCSR1B = (1<<RXCIE1) | (1<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1);
	// Set asynchronous, no parity, 1 stop bit, 8 data bits.
	UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);
	// Set baud rate.
	UBRR1H = (unsigned char)(DEBUG_UBRR >> 8);
	UBRR1L = (unsigned char)DEBUG_UBRR;
}

void debug_send(char* msg)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		circ_buf_write(&debug_send_buf, msg);
		// Enable the transmit data register empty interrupt
		UCSR1B |= (1<<UDRIE1);
	}
}

size_t debug_recv(char* dest, size_t dest_len)
{
	size_t bytes_read;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		bytes_read = circ_buf_read(&debug_recv_buf, dest, dest_len);
	}
	return bytes_read;
}

// Fires when transmit data register is empty, indicating we can pump in the next byte
ISR(USART1_UDRE_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Check if there's a byte to send; if there is then copy it into the data register
		char temp;
		size_t bytes_read = circ_buf_read(&debug_send_buf, &temp, 1);
		if (bytes_read == 1)
		{
			UDR1 = temp;
		}
		
		// Disable this interrupt if there's no more data in the buffer, otherwise this ISR will keep getting called forever
		if (circ_buf_get_len(&debug_send_buf) == 0)
		{
			UCSR1B &= ~(1<<UDRIE1);
		}
	}	
}

// Fires when the receive data register is full, indicating we can read in an incoming byte
ISR(USART1_RX_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Copy the incoming byte out of the data register into a null terminated string
		char temp[2] = {0, 0};
		temp[0] = UDR1;
		
		// Write the incoming byte into the receive buffer
		circ_buf_write(&debug_recv_buf, temp);
	}
}