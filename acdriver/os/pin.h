/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:26:31
 */ 

#ifndef PIN_H_
#define PIN_H_

#define PIN_INPUT_MODE 0
#define PIN_OUTPUT_MODE 1

#define PORTA_INDEX 0
#define PORTB_INDEX 1
#define PORTC_INDEX 2
#define PORTD_INDEX 3

void InitPins(void);

uint8_t readPin(uint8_t portNumber, uint8_t pinNumber);

void setPin(uint8_t portNumber, uint8_t pinNumber, uint8_t value);

/**
Mode: 0 = pin read mode (without pull-up), 1 = pin write mode 
*/
void setPinMode(uint8_t portNumber, uint8_t pinNumber, uint8_t mode);

#endif /* PIN_H_ */