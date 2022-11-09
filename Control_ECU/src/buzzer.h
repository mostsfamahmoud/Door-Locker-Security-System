 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the BUZZER driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "gpio.h"

#define BUZZER_PORT_ID        PORTC_ID
#define BUZZER_PIN_ID         PIN7_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
   Description:
     1) Setup the direction for the buzzer pin as output pin through the GPIO driver.
     2) Turn off the buzzer through the GPIO.
*/
void BUZZER_init();

/*
   Description: Function to enable the Buzzer through the GPIO.
*/
void BUZZER_ON(void);

/*
   Description: Function to disable the Buzzer through the GPIO.
*/
void BUZZER_OFF(void);


#endif /* BUZZER_H_ */
