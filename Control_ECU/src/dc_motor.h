 /******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for the DC MOTOR driver
 *
 * Author: Mostafa Mahmoud
 *
 * Group: 71
 *
 *******************************************************************************/
#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_


#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MOTOR_PINS_PORT_ID         PORTA_ID

#define MOTOR_IN1_PIN_ID           PIN0_ID
#define MOTOR_IN2_PIN_ID           PIN1_ID
#define MOTOR_EN1_PIN_ID           PIN2_ID



/* Enum DcMotor_State to Select type of motion of DC-Motor (CW, A_CW, Stop) */
typedef enum{
	STOP,Anti_CLOCKWISE,CLOCKWISE
}DcMotor_State;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
Description
	1) The Function responsible for setup the direction for the two motor pins through the GPIO driver.
	2) Stop at the DC-Motor at the beginning through the GPIO driver.
*/
void DcMotor_Init(void);


/*
Description:
	The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
Inputs:
	State: The required DC Motor state, it should be CW or A-CW or stop.
	       DcMotor_State data type should be declared as enum or uint8.
*/
void DcMotor_Rotate(DcMotor_State state);



#endif /* DC_MOTOR_H_ */
