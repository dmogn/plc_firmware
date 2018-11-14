/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 10.01.2012 16:52:11
 *
 * General Purpose Input-Output PINs
 */ 

#include "os.h"

void gpioInitPins(void)
{
	// Set GPIO pins as input pins with enabled pull-up resistors
	DDRB &= ~(1<<GPIO_CHANNEL_0_BIT | 1<<GPIO_CHANNEL_1_BIT | 1<<GPIO_CHANNEL_2_BIT | 1<<GPIO_CHANNEL_3_BIT | 1<<GPIO_CHANNEL_4_BIT);
	DDRC &= ~(1<<GPIO_CHANNEL_5_BIT);
	
	PORTB |= (1<<GPIO_CHANNEL_0_BIT | 1<<GPIO_CHANNEL_1_BIT | 1<<GPIO_CHANNEL_2_BIT | 1<<GPIO_CHANNEL_3_BIT | 1<<GPIO_CHANNEL_4_BIT);
	PORTC |= (1<<GPIO_CHANNEL_5_BIT);
	
	PINB |= (1<<GPIO_CHANNEL_0_BIT | 1<<GPIO_CHANNEL_1_BIT | 1<<GPIO_CHANNEL_2_BIT | 1<<GPIO_CHANNEL_3_BIT | 1<<GPIO_CHANNEL_4_BIT);
	PINC |= (1<<GPIO_CHANNEL_5_BIT);
}
