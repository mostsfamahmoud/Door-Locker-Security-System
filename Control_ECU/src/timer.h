 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.h
 *
 * Description: header file for the AVR TIMER driver
 *
 * Author: Mostafa Mahmoud
 *
 * Group: 71
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define DUMMY   0xFF

typedef enum {
	NO_CLOCK, FCPU_CLOCK, FCPU_8, FCPU_64, FCPU_256, FCPU_1024, EXTERNAL_CS_T0_FALLING, EXTERNAL_CS_T0_RISING
} Timer_Prescaler;


typedef enum {
	NO_CLK, FCLK_1, FCLK_8, FCLK_32, FCLK_64, FCLK_128, FCLK_256, FCLK_1024
} Timer2_Prescaler;


typedef enum {
	OVERFLOW_MODE, COMPARE_MODE, FAST_PWM_MODE
} Timer_Mode;

typedef enum {
	TIMER0, TIMER1, TIMER2
} Timer_ID;

typedef struct {
	Timer_ID timerId;
	Timer_Mode mode;
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer_Prescaler prescaler;
	Timer2_Prescaler prescaler2;
} Timer_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
 Description:
     Function to initialize the Timer driver
 Inputs:
     Pointer to the configuration structure with type Timer1_ConfigType.
*/
void Timer_init(const Timer_ConfigType *Config_Ptr);


/*
 Description:
     Function to set the Call Back function address.
 Inputs:
    1) Pointer to Call Back function.
    2) Variable of enum type Timer_ID to select which Timer you want to set Call Back function for it
*/
void Timer_deInit(Timer_ID timerID);


/*
 Description:
     Function to disable the Timer1.
 Inputs:
     Takes variable of enum type Timer_ID to select which Timer that user wants to de-initialize
*/
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID timerID);


#endif /* TIMER_H_ */
