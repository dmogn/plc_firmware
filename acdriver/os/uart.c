/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:25:00
 */ 

#include "os.h"

#define UART_RS485_PIN_INDEX 2

//volatile usart_rs485_mode = UART_RS485_MODE_RECEIVE;

void USART_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRRH = (uint8_t)(baud>>8);
	UBRRL = (uint8_t)baud;
	
	/* Set RS485 transmit pin to output mode and low signal (receive mode) */
	USART_SetRS485Mode(UART_RS485_MODE_RECEIVE);
	
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<URSEL)|(1<<USBS)|(1<<UCSZ0);
}

void USART_SetMaxRate( void )
{
	// set maximum rate (0.5 MBit for 8MGz)
	/* Set baud rate */
	UBRRH = (uint8_t)0;
	UBRRL = (uint8_t)0;

	// Set double rate (1 MBit for 8MGz)
#if USE_2X
    UCSRA |= (1 << U2X);
#else
    UCSRA &= ~(1 << U2X);
#endif	
}

void USART_SetDefaultRate( void )
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
#if USE_2X
    UCSRA |= (1 << U2X);
#else
    UCSRA &= ~(1 << U2X);
#endif	
}

void USART_EnableInterrupts( void ) 
{
	// Receive Complete Interrupt Enable (RXCIE) - enabled by default
	// Transmit Compete Interrupt Enable (TXCIE) - enabled by default
	// Data Register empty Interrupt Enable (UDRIE) - disabled by default
	UCSRB |= (1<<RXCIE)|(1<<TXCIE);
}

void USART_SetRS485Mode(uint8_t mode) 
{
	if (mode == UART_RS485_MODE_RECEIVE)
	{
		// Set RS485 device pin to receiver mode
		setPin(PORTD_INDEX, UART_RS485_PIN_INDEX, UART_RS485_MODE_RECEIVE);	
		
		// Enable receiver
		UCSRB |= (1<<RXEN);
		// Disable transmitter
		UCSRB &= ~(1<<TXEN);
		// Disable Data Register empty Interrupt (UDRIE)
		UCSRB &= ~(1<<UDRIE);	
	} 
	else if (mode == UART_RS485_MODE_TRANSMIT)
	{
		// Set RS485 device pin to transmitter mode
		setPin(PORTD_INDEX, UART_RS485_PIN_INDEX, UART_RS485_MODE_TRANSMIT);
		
		// Disable receiver
		UCSRB &= ~(1<<RXEN);
		// Enable transmitter
		UCSRB |= (1<<TXEN);
		// Enable Data Register empty Interrupt (UDRIE)
		UCSRB |= (1<<UDRIE);
	}	
	//usart_rs485_mode = mode;
}

void USART_DisableDataRegisterEmptyInterrupt( void )
{
	UCSRB &= ~(1<<UDRIE);
}

void USART_Transmit( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
		;
	/* Put data into buffer, sends the data */
	UDR = data;
}

uint8_t USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) )
		;
	/* Get and return received data from buffer */
	return UDR;
}

void USART_Flush( void )
{
	uint8_t dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}

// USART, Rx Complete interrupt handler
ISR(USART_RXC_vect)
{
	// must read received byte in any way
	uint8_t receivedByte = UDR;
	modbusUARTByteReceived(receivedByte);
	//pxMBFrameCBByteReceived();
}

// USART Data Register Empty interrupt handler
ISR(USART_UDRE_vect)
{
	//pxMBFrameCBTransmitterEmpty();
	modbusUARTTransmitterBufferEmpty();
}

// USART, Tx Complete interrupt handler
ISR(USART_TXC_vect)
{
	modbusUARTByteTransmitted();
//#ifdef RTS_ENABLE
    //RTS_LOW;
//#endif
}
