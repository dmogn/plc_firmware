/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:28:35
 */ 

#include "os.h"

void WatchDogInit2Seconds(void)
{
	WDTCR = (1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);
}

void WatchDogDisable(void)
{
	/* reset WDT */
	wdt_reset();
	
	/* Write logical one to WDTOE and WDE */
	WDTCR |= (1<<WDTOE) | (1<<WDE);
	/* Turn off WDT */
	WDTCR = 0x00;
}


void ResetHardware(void)
{
	// Init watch dog with maximum frequency - 16 ms wait and 17 ms timeout:
	WDTCR = (1<<WDE)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0);
	
	wdt_reset();
	
	// wait 16 ms for reset in infinity cicle
	while (TRUE)
	{
		;
	}	
}