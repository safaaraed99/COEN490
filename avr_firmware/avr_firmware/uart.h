/*
 * uart.h
 *
 * Created: 2024-03-16 2:07:45 PM
 *  Author: matt_
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdlib.h>

void setup_uart(void);

void debug_send(char* msg);

size_t debug_recv(char* dest, size_t dest_len);

// TODO Bluetooth communication

#endif /* UART_H_ */