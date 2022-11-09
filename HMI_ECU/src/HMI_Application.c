 /******************************************************************************
 *
 * Module: HMI_ECU (HUMAN MACHINE INTERFACE)
 *
 * File Name: HMI_Application.c
 *
 * Description: Source file for the HMI_ECU driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include "HMI_Application.h"
#include "keypad.h"
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "Macros.h"

int main(void)
{
	uint8 key = 0, receivedByte = 0;

	SET_BIT(SREG, PIN7_ID);     /* Enable Global Interrupt (I-bit) */

	LCD_init();

	/* UART Configuration:
	 * BaudRate --> 9600 Bps
	 * CharacterSize --> 8 Data Bits
	 * ParityMode --> No Parity
	 * No. of Stop Bits --> One Stop Bit
	 */
	UART_ConfigType configPtr = { 9600, DATA_EIGHT, NO_PARITY, ONE_STOP_BIT };
	UART_init(&configPtr);

	/* Timer Configuration:
	 * Timer ID --> Timer 1
	 * Timer Mode --> CTC Mode
	 * Initial Value --> 0
	 * Timer_Prescaler --> FCPU/1024
	 * Compare Value --> 7813
	 * AS FCPU = 8MHz so Ftimer = 8MHz/1024 = 128us & To force timer to produce interrupt every 1 second
	 * SO Compare Value = 1/128us = 7813
	 */
	Timer_ConfigType config = { TIMER1, COMPARE_MODE, 0, 7813, FCPU_1024, DUMMY };

	/* Set the call back function of Timer1 as HMI_timerCallBack */
	Timer_setCallBack(HMI_timerCallBack, TIMER1);
	Timer_init(&config);

	g_Password_Match_Status = PASSWORD_UNMATCHED;      /* Initial value of the password status as UNMATCHED */

	HMI_SystemPasswordInit(g_InputPassword);           /* Create System password for the first time */

	while(1)
	{
		HMI_AppMainOptions();           /* Displaying the main options to the user */

		key = KEYPAD_getPressedKey();   /* Taking input from the keypad */

		if (key == '+')
		{
			LCD_clearScreen();
			LCD_displayString("Enter the pass: ");
			LCD_moveCursor(1, 0);
			HMI_getPassword(g_InputPassword);          /* Get password from user and store it in global array */
			UART_sendByte(READY_TO_SEND);              /* inform Control ECU to start sending */
			HMI_sendPasswordByUART(g_InputPassword);   /* Sending that password to Control ECU */

			UART_sendByte(OPEN_DOOR_OPTION);           /* Inform Control ECU that User chose Open Door Option */

			/* Control ECU responses [either the password is correct or wrong] */
			receivedByte = UART_recieveByte();
			if (receivedByte == UNLOCKING_DOOR)
			{
				HMI_OpenDoor();                /* Start displaying door status on LCD */
				g_wrongPasswordCounter = 0;    /* Reset the counter */
			}
			else if (receivedByte == WRONG_PASSWORD)
			{
				LCD_clearScreen();
				LCD_displayString("Wrong Password");

				g_wrongPasswordCounter++;       /* Increment the counter */

				if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
				{
					/* System should be locked no inputs from Keypad will be accepted during this time period */
					g_sec = 0;
					while (g_sec < KEYPAD_LOCKED_PERIOD);
				}
				else
				{
					/* Small delay to see "Wrong Password" message on the LCD */
					_delay_ms(MESSAGE_DISPLAY_DELAY);
				}
			}
		}

		else if(key == '-')
		{
			LCD_clearScreen();
			LCD_displayString("Enter the pass: ");
			LCD_moveCursor(1, 0);
			HMI_getPassword(g_InputPassword);          /* Get password from user and store it in global array */
			UART_sendByte(READY_TO_SEND);              /* inform Control ECU to start sending */
			HMI_sendPasswordByUART(g_InputPassword);   /* Sending that password to Control ECU */

			UART_sendByte(CHANGE_PASSWORD_OPTION);     /* inform Control ECU that user chose Change Password Option */

			/* Control ECU responses [either the password is correct or wrong] */
			receivedByte = UART_recieveByte();

			/* If user enters the old password right, then let user create a new system password*/
			if (receivedByte == CHANGING_PASSWORD)
			{
				HMI_SystemPasswordInit(g_InputPassword);
				LCD_clearScreen();
				g_wrongPasswordCounter = 0;      /* reset the counter */
			}
			else if (receivedByte == WRONG_PASSWORD)
			{
				LCD_clearScreen();
				LCD_displayString("Wrong Password");

				g_wrongPasswordCounter++;       /* Increment the counter */

				if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
				{
					/* System should be locked no inputs from Keypad will be accepted during this time period */
					g_sec = 0;
					while (g_sec < KEYPAD_LOCKED_PERIOD);
				}
				else
				{
					/* Small delay to see "Wrong Password" message on the LCD */
					_delay_ms(MESSAGE_DISPLAY_DELAY);
				}
			}
		}
	}
}

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
 * Description: Timer Call Back Function the is related to HMI Module
 */
void HMI_timerCallBack(void)
{
	g_sec++;
}

/*
 * Description: Function to display the application main options
 */
void HMI_AppMainOptions()
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+ : Open Door");
	LCD_displayStringRowColumn(1, 0, "- : Change Pass");
}

/*
 * Description: Function to Initialize System Password
 */
void HMI_SystemPasswordInit(uint8 *password)
{
	while(g_Password_Match_Status == PASSWORD_UNMATCHED)
	{
		/* Entering the password for the first time */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Enter a Password: ");
		LCD_moveCursor(1, 0);
		HMI_getPassword(password);

		/* Sending the password to the Control ECU By UART */
		UART_sendByte(READY_TO_SEND);
		while(UART_recieveByte() != READY_TO_RECEIVE);
		HMI_sendPasswordByUART(g_InputPassword);

		/* Entering the confirmation password */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Re-Enter the same");
		LCD_displayStringRowColumn(1, 0, "password: ");
		LCD_moveCursor(1, 10);
		HMI_getPassword(password);

		/* Sending the Confirmation password to the Control ECU By UART */
		UART_sendByte(READY_TO_SEND);
		while(UART_recieveByte() != READY_TO_RECEIVE);
		HMI_sendPasswordByUART(g_InputPassword);

		/* Receiving Passwords Match Status from Control ECU By UART */
		while(UART_recieveByte() != READY_TO_SEND);
		UART_sendByte(READY_TO_RECEIVE);
		g_Password_Match_Status = UART_recieveByte();

		/* Checking on the Matching Status */
		if (g_Password_Match_Status == PASSWORD_MATCHED)
			break;
		else
		{
			LCD_clearScreen();
			LCD_displayString("PASSWORD MISMATCH");
			_delay_ms(MESSAGE_DISPLAY_DELAY);
		}
	}
	g_Password_Match_Status = PASSWORD_UNMATCHED;
}

/*
 * Description: Function to get password as an input from the keypad and store it in a global array
 */
void HMI_getPassword(uint8 *password)
{
	uint8 i = 0, key = 0;

	/* Taking password of length = 5 from the keypad */
	while(i != PASSWORD_LENGTH)
	{
		key = KEYPAD_getPressedKey();

		if ((key >= 1) && (key <= 9))
		{
			password[i] = key;
			LCD_displayCharacter('*');        /* Display '*' on LCD for each number */
			i++;
		}
		/* Delay between each input taken from the keypad */
		_delay_ms(KEYPAD_INPUT_DELAY);
	}

	/* Loop until user presses the ENTER key from the keypad */
	while(KEYPAD_getPressedKey() != ENTER_KEY_PRESSED);
}

/*
 * Description: Function to send the entered password Byte by Byte to the Control ECU By UART
 */
void HMI_sendPasswordByUART(uint8 *password)
{
	uint8 count = 0;

	while(count != PASSWORD_LENGTH)
	{
		UART_sendByte(password[count]);
		count++;
		_delay_ms(100);
	}
}


/*
 * Description: A function that displays on LCD that door is opening or closing for a certain period of time
 */
void HMI_OpenDoor(void)
{
	/* Hold the system for 15 seconds & display to user that door is unlocking */
	g_sec = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is unlocking");
	while (g_sec < DOOR_UNLOCKING_PERIOD);

	/* Hold the door open for 3 seconds */
	g_sec = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is now open");
	while (g_sec < DOOR_LEFT_OPEN_PERIOD);

	/* Hold the system for 15 seconds & display to user that door is locking */
	g_sec = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is locking");
	while (g_sec < DOOR_LOCKING_PERIOD);
}
