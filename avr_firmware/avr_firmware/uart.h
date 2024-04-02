/*
 * uart.h
 *
 * Created: 2024-03-16 2:07:45 PM
 *  Author: Matthew Faigan
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdlib.h>
#include <stdint.h>

#include "glove_enums.h"

/**
 * \brief Initializes UART0 and UART1. Must be called during startup.
 * 
 * \return void
 */
void setup_uart(void);

/**
 * \brief Transmits an array of characters over the debug UART.
 * 
 * \param msg A null-terminated string to transmit.
 * 
 * \return void
 */
void debug_send(char* msg);

/**
 * \brief Receives up to a given number of characters over the debug UART. Returns early if there isn't enough data to read, does NOT block until data comes in.
 * 
 * \param dest The destination array for the received data.
 * \param dest_len The maximum number of bytes to receive.
 * 
 * \return size_t The actual number of bytes received.
 */
size_t debug_recv(char* dest, size_t dest_len);

void bt_send_motor_warning(motor motor_num);

void bt_send_reading(potentiometer pot_num, int16_t reading);

#endif /* UART_H_ */