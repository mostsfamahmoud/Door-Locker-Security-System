 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the BUZZER driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include "buzzer.h"
#include <avr/io.h>


/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
   Description:
     1) Setup the direction for the buzzer pin as output pin through the GPIO driver.
     2) Turn off the buzzer through the GPIO.
*/
void BUZZER_init()
{
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}

/*
   Description: Function to enable the Buzzer through the GPIO.
*/
void BUZZER_ON(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

/*
   Description: Function to disable the Buzzer through the GPIO.
*/
void BUZZER_OFF(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
