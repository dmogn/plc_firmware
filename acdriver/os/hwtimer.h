/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:27:34
 */ 


#ifndef TIMER_H_
#define TIMER_H_

/**
* Init timer1 as main system schedule timer.
*/
void InitScheduleTimer(void);

/**
* Return system time (uptime) in milliseconds.
*/
uint32_t GetSystemTime(void);

/**
* Calculate time interval between two time values.
*/
uint32_t CalculateTimeInterval(uint32_t current_clock, uint32_t last_clock);

#endif /* TIMER_H_ */
