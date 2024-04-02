/*
 * motor.h
 *
 * Created: 2024-03-19 1:37:20 PM
 *  Author: Matthew Faigan
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

#include "glove_enums.h"

#define MOTOR_COUNT 5

void setup_motors(void);

int set_motor_speed(motor motor_num, uint8_t duty);

int set_motor_phase(motor motor_num, motor_direction direction);

int set_motor_enable(uint8_t state);

#endif /* MOTOR_H_ */