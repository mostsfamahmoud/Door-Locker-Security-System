 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "Macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *configPtr)
{
	uint16 ubrr_value = 0;

	UCSRA = (1 << U2X);       /* U2X = 1 for double transmission speed */

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For (5,6,7,8) bit data mode, This bit is set only for 9-bit data mode
	 * RXB8 & TXB8 used for 9-bit data mode only
	 ***********************************************************************/

	UCSRB = (1 << RXEN) | (1 << TXEN);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UCPOL   = 0 when Asynchronous mode is used
	 * UPM1:0  = Dynamically Configured according to user requirements
	 * USBS    = Dynamically Configured according to user requirements
	 * UCSZ1:0 = Dynamically Configured according to user requirements
	 ***********************************************************************/

	UCSRC = 0;
	UCSRC |= (1 << URSEL);
	UCSRC |= (UCSRC & 0xCF) | (configPtr->parity << UPM0);
	UCSRC |= (UCSRC & 0xF7) | (configPtr->stopBits << USBS);
	UCSRC |= (UCSRC & 0xF9) | (configPtr->dataBits << UCSZ0);

	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU/(8UL * configPtr->BaudRate)) - 1));

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRL = ubrr_value;
	UBRRH = ubrr_value >> 8;
}


/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for transmitting a new byte
	 * So wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE));

	/* This Code:
	 * Puts the required data in the UDR register
	 * Clears the UDRE flag as the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)); // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC);             // Clear the TXC flag
	*******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC));

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
    return UDR;		
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}


