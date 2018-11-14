/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:35:17
 */ 

#ifndef PWM_H_
#define PWM_H_

#define PWM_CHANNELS_COUNT 8

#define PWM_CHANNEL_0_BIT 1
#define PWM_CHANNEL_1_BIT 6
#define PWM_CHANNEL_2_BIT 7
#define PWM_CHANNEL_3_BIT 3
#define PWM_CHANNEL_4_BIT 4
#define PWM_CHANNEL_5_BIT 5
#define PWM_CHANNEL_6_BIT 6
#define PWM_CHANNEL_7_BIT 7

#define PWM_CHANNEL_0_PORT PORTC
#define PWM_CHANNEL_1_PORT PORTC
#define PWM_CHANNEL_2_PORT PORTC
#define PWM_CHANNEL_3_PORT PORTD
#define PWM_CHANNEL_4_PORT PORTD
#define PWM_CHANNEL_5_PORT PORTD
#define PWM_CHANNEL_6_PORT PORTD
#define PWM_CHANNEL_7_PORT PORTD

typedef struct {
  // frequency
  uint8_t prescaler; 
  uint8_t duration; 
} pwmChannel;

void InitPWMPins(void);

/**
* Init timer0 as PWM hard real-time timer.
*/
void InitPWMTimer(void);

void DisablePWMTimer(void);

void enablePWMChannel(uint8_t number);

void disablePWMChannel(uint8_t number);

void disableAllPWMChannels(void);

void setPWMChannelParameters(uint8_t channel_number, uint8_t enabled, uint8_t prescaler, uint8_t duration);

/**
* Calculate time interval between two time values.
*/
uint16_t calculatePWMTimeInterval(uint16_t current_clock, uint16_t last_clock);

#endif /* PWM_H_ */