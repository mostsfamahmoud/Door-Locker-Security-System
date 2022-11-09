 /******************************************************************************
 *
 * Module: CONTROL_ECU
 *
 * File Name: Control_Application.c
 *
 * Description: Source file for the CONTROL_ECU driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include "Control_Application.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "twi.h"
#include "buzzer.h"
#include "timer.h"
#include "uart.h"
#include "Macros.h"


int main(void)
{
	SET_BIT(SREG, PIN7_ID);       /* Enable Global Interrupts (I-Bit) */

	/* Timer Configuration:
	 * Timer ID --> Timer 1
	 * Timer Mode --> CTC Mode
	 * Initial Value --> 0
	 * Timer_Prescaler --> FCPU/1024
	 * Compare Value --> 7813
	 * AS FCPU = 8MHz so Ftimer = 8MHz/1024 = 128us & To force timer to produce interrupt every 1 second
	 * SO Compare Value = 1/128us = 7813
	 */
	Timer_ConfigType timerConfig = { TIMER1, COMPARE_MODE, 0, 7813, FCPU_1024, DUMMY };
	Timer_setCallBack(CTRL_timerCallBack, TIMER1);
	Timer_init(&timerConfig);

	/* UART Configuration:
	 * BaudRate --> 9600 Bps
	 * CharacterSize --> 8 Data Bits
	 * ParityMode --> No Parity
	 * No. of Stop Bits --> One Stop Bit
	 */
	UART_ConfigType uartConfig = { 9600, DATA_EIGHT, NO_PARITY, ONE_STOP_BIT };
	UART_init(&uartConfig);

	/* TWI(I2C) Configuration:
	 * Bit Rate --> 2
	 * Slave Address --> 0x01
	 * TWI Prescaler --> FCPU/1 --> 8MHz
	 */
	TWI_ConfigType twiConfig = { 0x02, TWI_CONTROL_ECU_ADDRESS, PRESCALER_1 };
	TWI_init(&twiConfig);

	/* Initialize DC MOTOR & BUZZER */
	DcMotor_Init();
	BUZZER_init();

	CTRL_SystemPasswordInit(g_receivedPassword);

	uint8 receivedByte = 0;

	while (1)
	{
		if (UART_recieveByte() == READY_TO_SEND)
		{
			CTRL_receivePasswordByUART(g_receivedPassword);
			receivedByte = UART_recieveByte();

			if (receivedByte == OPEN_DOOR_OPTION)
			{
				if (CTRL_comparePasswords(g_storedPassword, g_receivedPassword) == PASSWORD_MATCHED)
				{
					UART_sendByte(UNLOCKING_DOOR); /* inform HMI ECU to display that door is unlocking */
					CTRL_OpenDoor();               /* start opening door process/task */
					g_wrongPasswordCounter = 0;    /* reset the counter */
				}
				else
				{
					UART_sendByte(WRONG_PASSWORD);

					/* Count number of wrong attempts, and turn on a buzzer of it exceeds the limit */
					g_wrongPasswordCounter++;
					if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						BUZZER_ON();
						g_sec = 0;
						while (g_sec < ALARM_ON_DELAY); /* turn on alarm for a certain period */
						BUZZER_OFF();
						g_wrongPasswordCounter = 0; /* reset the counter */
					}
				}
			}
			else if (receivedByte == CHANGE_PASSWORD_OPTION)
			{
				if (CTRL_comparePasswords(g_storedPassword, g_receivedPassword) == PASSWORD_MATCHED)
				{
					UART_sendByte(CHANGING_PASSWORD); /* inform HMI to process changing password */
					CTRL_SystemPasswordInit(g_receivedPassword);
					g_wrongPasswordCounter = 0;    /* reset the counter */
				}
				else
				{
					UART_sendByte(WRONG_PASSWORD);

					/* Count number of wrong attempts, and turn on a buzzer of it exceeds the limit */
					g_wrongPasswordCounter++;
					if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						BUZZER_ON();
						g_sec = 0;
						while (g_sec < ALARM_ON_DELAY); /* turn on alarm for a certain period */
						BUZZER_OFF();
						g_wrongPasswordCounter = 0; /* reset the counter */
					}
				}
			}
		}
	}
}


/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
 * Description: a function to compare the stored pass with the received one
 */
uint8 CTRL_comparePasswords(const uint8 *a_password1,const uint8 *a_password2)
{
	CTRL_updateStoredPassword();
	uint8 i;
	for (i = 0; i < PASSWORD_LENGTH; i++)
	{
		if (a_password1[i] != a_password2[i])
		{
			return PASSWORD_UNMATCHED;
		}
	}
	return PASSWORD_MATCHED;
}

/*
 * Description: a function to initialize the password in first-run OR to change the password
 */
void CTRL_SystemPasswordInit(uint8 *pass)
{
	/* Do not return from this function till Control ECU sends a PASSWORD_MATCH signal */
	uint8 confirmationPassword[PASSWORD_LENGTH];
	uint8 matchingFlag = 0;

	while (!matchingFlag)
	{
		while (UART_recieveByte() != READY_TO_SEND);   /* wait till HMI gets ready */
		UART_sendByte(READY_TO_RECEIVE);               /* inform HMI that Control ECU ready to receive the password */
		CTRL_receivePasswordByUART(pass);

		while (UART_recieveByte() != READY_TO_SEND);
		UART_sendByte(READY_TO_RECEIVE);               /* inform HMI to send the confirmation password */
		CTRL_receivePasswordByUART(confirmationPassword);

		if (CTRL_comparePasswords(pass, confirmationPassword) == PASSWORD_MATCHED)
		{
			UART_sendByte(READY_TO_SEND);
			UART_sendByte(PASSWORD_MATCHED);
			CTRL_storePassword();
			matchingFlag = 1;
		}

		else
		{
			UART_sendByte(READY_TO_SEND);
			UART_sendByte(PASSWORD_UNMATCHED);
		}
	}
}

/* Description:
 *    A function that 1) Rotates on the DC motor for 15 seconds clockwise,
 *                    2) Stops it for 3 seconds,
 *                    3) Rotates it anti-clockwise for 15 seconds.
 */
void CTRL_OpenDoor(void)
{
	/* run the DC motor clockwise for 15 seconds */
	g_sec = 0;
	DcMotor_Rotate(CLOCKWISE);
	while (g_sec < DOOR_UNLOCKING_PERIOD);

	/* let the door be open for 3 seconds */
	g_sec = 0;
	DcMotor_Rotate(STOP);
	while (g_sec < DOOR_LEFT_OPEN_PERIOD);

	/* hold the system for 15 seconds & display to user that door is locking */
	g_sec = 0;
	DcMotor_Rotate(Anti_CLOCKWISE);
	while (g_sec < DOOR_LOCKING_PERIOD);

	DcMotor_Rotate(STOP);
}

/*
 * Description: the call-back function called by the timer every 1 second
 */
void CTRL_timerCallBack(void)
{
	g_sec++;
}

/*
 * Description: A function to receive the password via UART by looping on receiveByte function
 */
void CTRL_receivePasswordByUART(uint8 * pass)
{
	uint8 i;
	for (i = 0; i < PASSWORD_LENGTH; i++)
	{
		pass[i] = UART_recieveByte();
		_delay_ms(100);
	}
}

/*
 * Description: A function to retrieve the stored password from EEPROM
 */
void CTRL_updateStoredPassword(void)
{
	uint8 i;
	for (i = 0; i < PASSWORD_LENGTH; i++)
	{
		EEPROM_readByte(EEPROM_STORE_ADDREESS + i, g_storedPassword + i);
	}
}

/*
 * Description: A function to store the password in EEPROM
 */
void CTRL_storePassword(void)
{
	uint8 i;
	for (i = 0; i < PASSWORD_LENGTH; i++)
	{
		EEPROM_writeByte(EEPROM_STORE_ADDREESS + i, g_receivedPassword[i]);
		_delay_ms(100);
	}
}
