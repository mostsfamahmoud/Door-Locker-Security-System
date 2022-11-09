 /******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC MOTOR driver
 *
 * Author: Mostafa Mahmoud
 *
 * Group: 71
 *
 *******************************************************************************/

#include "dc_motor.h"

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/


/*
Description
	1) The Function responsible for setup the direction for the two motor pins through the GPIO driver.
	2) Stop at the DC-Motor at the beginning through the GPIO driver.
*/
void DcMotor_Init(void)
{
	/* Configure Pins (PA0 , PA1 , PA2) as Output pins */
	GPIO_setupPinDirection(MOTOR_PINS_PORT_ID, MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PINS_PORT_ID, MOTOR_IN2_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PINS_PORT_ID, MOTOR_EN1_PIN_ID, PIN_OUTPUT);

	/* Stop the DC-Motor at the beginning (IN1 = 0, IN2 = 0) */
	GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
}


/*
Description:
	The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
Inputs:
	State: The required DC Motor state, it should be CW or A-CW or stop.
	       DcMotor_State data type should be declared as enum or uint8.
*/
void DcMotor_Rotate(DcMotor_State state)
{
	/* To drive the DC Motor at the maximum speed */
	GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_EN1_PIN_ID, LOGIC_HIGH);

	/* Setting the DC Motor rotation direction (CW/ or A-CW or stop) based on the state value. */
	switch(state)
	{
	case STOP:
		/* Stop the DC-Motor (IN1 = 0, IN2 = 0) */
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	case CLOCKWISE:
		/* DC-Motor Mode --> ClockWise Rotation (IN1 = 0, IN2 = 1) */
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
		break;
	case Anti_CLOCKWISE:
		/* DC-Motor Mode --> Anti_ClockWise Rotation (IN1 = 1, IN2 = 0) */
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(MOTOR_PINS_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	default:
		break;
	}
}
