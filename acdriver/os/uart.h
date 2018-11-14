/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:25:29
 */ 


#ifndef UART_H_
#define UART_H_

typedef enum
{
	UART_RS485_MODE_RECEIVE = 0,
	UART_RS485_MODE_TRANSMIT
} uartRS485ModeEnum;

void USART_Init( unsigned int baud );

// set maximum available rate & double rate flag
void USART_SetMaxRate( void );

// set default rate with definitions BAUD, USE_2X
void USART_SetDefaultRate( void );

/** enable receive and transmit interrupts */
void USART_EnableInterrupts( void );

void USART_SetRS485Mode(uint8_t mode);

void USART_DisableDataRegisterEmptyInterrupt( void );

// synchronous byte transmit 
void USART_Transmit( unsigned char data );

// synchronous byte receive 
unsigned char USART_Receive( void );

void USART_Flush( void );

#endif /* UART_H_ */
