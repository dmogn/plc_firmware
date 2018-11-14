/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:27:58
 */ 

#include "os.h"


volatile uint8_t global_steps_count = 0;
volatile uint32_t system_uptime = 0;

// Asynchronous Timer1 interrupt
ISR(TIMER1_OVF_vect)
{
    ++global_steps_count;
	
	if (global_steps_count >= 125)
	{// millisecond tick
		global_steps_count = 0;
		system_uptime++;
		
		// cycle time where overflow
		if (system_uptime == UINT32_MAX)
			system_uptime = 0;
	}

    // Generate timer messages (jobs)
    dispatchTimer();
}

/**
* Init timer1 as main system timer.
*/
void InitScheduleTimer(void) 
{
// set prescaler
TCCR1B = (1<<CS11)|(1<<CS10); // Timer clock = system clock/64

// enable timer
TIFR = 1<<ICF1;

// Clear ICF1/
// clear pending
// interrupts

TIMSK = 1<<TICIE1; // Enable Timer1
                  // Capture Even
}

uint32_t GetSystemTime(void)
{
	return system_uptime;
}

uint32_t CalculateTimeInterval(uint32_t current_clock, uint32_t last_clock)
{
	if (current_clock >= last_clock)
		return current_clock - last_clock;
	else
		return current_clock + (UINT32_MAX - last_clock);
}