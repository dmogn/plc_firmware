/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:35:38
 */ 

#include "os.h"

volatile uint16_t pwm_timer_counter = 0;
volatile uint8_t enabled_channels = 0;
volatile pwmChannel pwm_channels[PWM_CHANNELS_COUNT];

void InitPWMPins(void)
{
	// Set PWM pins to low levels
	disableAllPWMChannels();
	
	// Set PWM pins as output pins
	DDRC |= (1<<PWM_CHANNEL_0_BIT)|(1<<PWM_CHANNEL_1_BIT)|(1<<PWM_CHANNEL_2_BIT);
	DDRD |= (1<<PWM_CHANNEL_3_BIT)|(1<<PWM_CHANNEL_4_BIT)|(1<<PWM_CHANNEL_5_BIT)|(1<<PWM_CHANNEL_6_BIT)|(1<<PWM_CHANNEL_7_BIT);
}

void InitPWMTimer(void) 
{
	// Output Compare Flag - OCF0
	// Output Compare Register - OCR0
	// Timer/Counter Control Register - TCCR0
	// Timer/Counter Register-  TCNT0
	// Timer/Counter Interrupt Mask Register - TIMSK
	// Timer/Counter Interrupt Flag Register - TIFR
	// CTC mode = Clear Timer on Compare
	
	// disable timer0
	TCCR0 = 0;
	
	// clear counter
	TCNT0 = 0;
	
	// set compate match to 40 (200 KGz)
	OCR0 = 40;
	
	// init timer 0
	// set prescaler
	TCCR0 = 1; // Timer clock = system clock (no prescaling)
			
	// enable compare match interrupt
	TIMSK = (1<<1);
	
	//TODO: enable CTC mode
}

void DisablePWMTimer(void)
{
	// disable timer0
	TCCR0 = 0;
}

void enablePWMPin(uint8_t number)
{
	if (number < 4)
	{
		if (number < 2)
		{
			if (number == 0)
				PWM_CHANNEL_0_PORT |= (1<<PWM_CHANNEL_0_BIT);
			else
				PWM_CHANNEL_1_PORT |= (1<<PWM_CHANNEL_1_BIT);
		}
		else
		{
			if (number == 2)
				PWM_CHANNEL_2_PORT |= (1<<PWM_CHANNEL_2_BIT);
			else
				PWM_CHANNEL_3_PORT |= (1<<PWM_CHANNEL_3_BIT);
		}
	}
	else
	{
		if (number < 6)
		{
			if (number == 4)
				PWM_CHANNEL_4_PORT |= (1<<PWM_CHANNEL_4_BIT);
			else
				PWM_CHANNEL_5_PORT |= (1<<PWM_CHANNEL_5_BIT);
		}
		else
		{
			if (number == 6)
				PWM_CHANNEL_6_PORT |= (1<<PWM_CHANNEL_6_BIT);
			else
				PWM_CHANNEL_7_PORT |= (1<<PWM_CHANNEL_7_BIT);
		}
	}
}

void disablePWMPin(uint8_t number)
{
	if (number < 4)
	{
		if (number < 2)
		{
			if (number == 0)
				PWM_CHANNEL_0_PORT &= ~(1<<PWM_CHANNEL_0_BIT);
			else
				PWM_CHANNEL_1_PORT &= ~(1<<PWM_CHANNEL_1_BIT);
		}
		else
		{
			if (number == 2)
				PWM_CHANNEL_2_PORT &= ~(1<<PWM_CHANNEL_2_BIT);
			else
				PWM_CHANNEL_3_PORT &= ~(1<<PWM_CHANNEL_3_BIT);
		}
	}
	else
	{
		if (number < 6)
		{
			if (number == 4)
				PWM_CHANNEL_4_PORT &= ~(1<<PWM_CHANNEL_4_BIT);
			else
				PWM_CHANNEL_5_PORT &= ~(1<<PWM_CHANNEL_5_BIT);
		}
		else
		{
			if (number == 6)
				PWM_CHANNEL_6_PORT &= ~(1<<PWM_CHANNEL_6_BIT);
			else
				PWM_CHANNEL_7_PORT &= ~(1<<PWM_CHANNEL_7_BIT);
		}
	}
}

void enablePWMChannel(uint8_t number)
{
	enabled_channels |= (1<<number);
}

void disablePWMChannel(uint8_t number) 
{
	// disable PIN
	enabled_channels &= ~(1<<number);
	
	// disablePWMChannel
	disablePWMPin(number);
}

void disableAllPWMChannels(void)
{
	// disable internal state
	enabled_channels = 0;
	
	// disable pins
	PORTC &= ~((1<<PWM_CHANNEL_0_BIT)|(1<<PWM_CHANNEL_1_BIT)|(1<<PWM_CHANNEL_2_BIT));
	PORTD &= ~((1<<PWM_CHANNEL_3_BIT)|(1<<PWM_CHANNEL_4_BIT)|(1<<PWM_CHANNEL_5_BIT)|(1<<PWM_CHANNEL_6_BIT)|(1<<PWM_CHANNEL_7_BIT));
}

void setPWMChannelParameters(uint8_t channel_number, uint8_t enabled, uint8_t prescaler, uint8_t duration)
{
	if (channel_number >= PWM_CHANNELS_COUNT)
	{
		// report software failure
		ctrlReportError(PWM_SOFTWARE_CHANNEL_NUMBER_OUT_OF_RANGE);
	}		
		
	if (duration > prescaler)
		duration = prescaler;
	
	pwm_channels[channel_number].prescaler = prescaler;
	pwm_channels[channel_number].duration = duration;
	
	if (enabled)
		enabled_channels = enabled_channels | (1<<channel_number);
	else
		enabled_channels = enabled_channels & ~(1<<channel_number); 
}

uint16_t calculatePWMTimeInterval(uint16_t current_clock, uint16_t last_clock)
{
	if (current_clock >= last_clock)
		return current_clock - last_clock;
	else
		return current_clock + (UINT16_MAX - last_clock);
}

ISR(TIMER0_OVF_vect)
{
    // none, disabled
}

ISR(TIMER0_COMP_vect)
{
	// clear hardware counter
	TCNT0 = 0;
	
	// update counter
	if (pwm_timer_counter == UINT16_MAX)
		pwm_timer_counter = 0;
	else
		pwm_timer_counter++;
	
	// PWM hard real-time feature
	for (uint8_t index=0; index<PWM_CHANNELS_COUNT; index++)
	{
		if (enabled_channels & (1<<index))
		{
			uint8_t channel_specific_ticks = pwm_timer_counter % (pwm_channels[index].prescaler);
			if (channel_specific_ticks == 0 && pwm_channels[index].duration > 0)
				enablePWMPin(index);
			
			else if (channel_specific_ticks >= pwm_channels[index].duration)
				disablePWMPin(index);
		}
		else
			disablePWMPin(index);
	}	
}