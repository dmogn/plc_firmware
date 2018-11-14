/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:35:17
 */ 

#ifndef REALTIMER_H_
#define REALTIMER_H_

typedef struct{
	volatile uint32_t tick_counts;
} UpTimeCounter;

// Init Timer2 as asynchronous real time timer (for 32768 Gz external quartz)
void InitASS_Timer(void);

#endif /* REALTIMER_H_ */
