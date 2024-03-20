/*
 * glove_enums.h
 *
 * Created: 2024-03-19 1:57:33 PM
 *  Author: Matthew Faigan
 */ 


#ifndef GLOVE_ENUMS_H_
#define GLOVE_ENUMS_H_

typedef enum
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
} potentiometer;

// N.B. The motor channels were connected in reverse order to make the PCB layout easier
typedef enum
{
	MOTOR_PINKY = 0,
	MOTOR_RING = 1,
	MOTOR_MIDDLE = 2,
	MOTOR_INDEX = 3,
	MOTOR_THUMB = 4
} motor;

typedef enum
{
	DIRECTION_FORWARD,
	DIRECTION_BACKWARD
} motor_direction;

#endif /* GLOVE_ENUMS_H_ */