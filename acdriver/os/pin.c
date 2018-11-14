/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:26:59
 */ 

#include "os.h"

void InitPins(void)
{
	// Registers: DDRA (mode, direction), PORTA (out data), PINA (input)
	
	// set port A as input pins (ADC)
	DDRA = 0;
	// set port B output pins to low level (disable port pull-ups)
	PORTA = 0;
	
	// set port B output pins to low level (disable port  pull-ups)
	PORTB = 0;
	// set port B as out pins (PWM)
	DDRB = 0xFF;
	
	// set port C output pins to low level (disable port pull-ups)
	PORTC = 0;
	
	// set port D output pins to low level (disable port  pull-ups)
	PORTD = 0;
	
	InitPWMPins();
	gpioInitPins();
}

void InitGPIOPins(void)
{
	;
}

uint8_t readPin(uint8_t portNumber, uint8_t pinNumber)
{
	uint8_t portValue = 0;
	
	if (portNumber == PORTA_INDEX)
		portValue = PINA;
	else if (portNumber == PORTB_INDEX)
		portValue = PINB;
	else if (portNumber == PORTC_INDEX)
		portValue = PINC;
	else if (portNumber == PORTD_INDEX)
		portValue = PIND;
	else
		ctrlReportErrorWithParameter(CTRLR_SOFTWARE_ILLEGAL_PIN_PORT_NUMER, portNumber);
	
	return (portValue>>pinNumber) & 1;
}

void setPin(uint8_t portNumber, uint8_t pinNumber, uint8_t pinValue)
{
	uint8_t portValue = 0;
	
	if (portNumber == PORTA_INDEX)
		portValue = PORTA;
	else if (portNumber == PORTB_INDEX)
		portValue = PORTB;
	else if (portNumber == PORTC_INDEX)
		portValue = PORTC;
	else if (portNumber == PORTD_INDEX)
		portValue = PORTD;
	else
		ctrlReportErrorWithParameter(CTRLR_SOFTWARE_ILLEGAL_PIN_PORT_NUMER, portNumber);
		
	if (pinValue > 0)
		portValue |= (1<<pinNumber);
	else 
		portValue &= ~(1<<pinNumber);
		
	if (portNumber == PORTA_INDEX)
		PORTA = portValue;
	else if (portNumber == PORTB_INDEX)
		PORTB = portValue;
	else if (portNumber == PORTC_INDEX)
		PORTC = portValue;
	else if (portNumber == PORTD_INDEX)
		PORTD = portValue;
}

void setPinMode(uint8_t portNumber, uint8_t pinNumber, uint8_t mode)
{
	uint8_t portValue = 0;
	
	if (portNumber == 0)
		portValue = DDRA;
	else if (portNumber == 1)
		portValue = DDRB;
	else if (portNumber == 2)
		portValue = DDRC;
	else if (portNumber == 3)
		portValue = DDRD;
	else
		ctrlReportErrorWithParameter(CTRLR_SOFTWARE_ILLEGAL_PIN_PORT_NUMER, portNumber);
		
	if (mode > 0)
		portValue |= (1<<pinNumber);
	else 
		portValue &= ~(1<<pinNumber);
		
	if (portNumber == 0)
		DDRA = portValue;
	else if (portNumber == 1)
		DDRB = portValue;
	else if (portNumber == 2)
		DDRC = portValue;
	else if (portNumber == 3)
		DDRD = portValue;
}
