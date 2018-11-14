/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:28:16
 */ 


#ifndef WATCHDOG_H_
#define WATCHDOG_H_

void WatchDogInit2Seconds(void); 
void WatchDogDisable(void);

/** 
Hardware reset entire microcontroller.
2 steps:
- Enable WatchDog with maximum frequency (16ms timeout)
- Wait until reset (infinity cycle);
 */
void ResetHardware(void);

#endif /* WATCHDOG_H_ */
