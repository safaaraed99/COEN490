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

void setup_motors(void);

int set_motor_speed(motor motor_num, uint8_t duty);

int set_motor_phase(motor motor_num, motor_direction direction);



#endif /* MOTOR_H_ */