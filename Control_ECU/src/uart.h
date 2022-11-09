 /*==============================================================================
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mostafa Mahmoud
 *
 ===============================================================================*/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef enum {
	NO_PARITY, EVEN_PARITY = 2, ODD_PARITY
} UART_ParityMode;

typedef enum {
	ONE_STOP_BIT, TWO_STOP_BITS
} UART_StopBitSelect;

typedef enum {
	DATA_FIVE, DATA_SIX, DATA_SEVEN, DATA_EIGHT
} UART_CharacterSize;

typedef struct {
	uint32 BaudRate;
	UART_CharacterSize dataBits;
	UART_ParityMode parity;
	UART_StopBitSelect stopBits;
} UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *configPtr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
