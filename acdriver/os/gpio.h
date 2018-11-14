/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 10.01.2012 16:51:51
 *
 * General Purpose Input-Output PINs
 */ 

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_CHANNEL_0_BIT 0
#define GPIO_CHANNEL_1_BIT 1
#define GPIO_CHANNEL_2_BIT 2
#define GPIO_CHANNEL_3_BIT 3
#define GPIO_CHANNEL_4_BIT 4
#define GPIO_CHANNEL_5_BIT 0

#define GPIO_CHANNEL_0_OUT_PORT PORTB
#define GPIO_CHANNEL_1_OUT_PORT PORTB
#define GPIO_CHANNEL_2_OUT_PORT PORTB
#define GPIO_CHANNEL_3_OUT_PORT PORTB
#define GPIO_CHANNEL_4_OUT_PORT PORTB
#define GPIO_CHANNEL_5_OUT_PORT PORTC

#define GPIO_CHANNEL_0_IN_PORT PINB
#define GPIO_CHANNEL_1_IN_PORT PINB
#define GPIO_CHANNEL_2_IN_PORT PINB
#define GPIO_CHANNEL_3_IN_PORT PINB
#define GPIO_CHANNEL_4_IN_PORT PINB
#define GPIO_CHANNEL_5_IN_PORT PINC

/** Set GPIO pins as input pins with enabled pull-up resistors */
void gpioInitPins(void);

#endif /* GPIO_H_ */