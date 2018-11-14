/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 26.06.2011 13:14:38
 */ 


#ifndef HAL_H_
#define HAL_H_

/// write byte to EEPROM
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);

/// read byte from EEPROM
unsigned char EEPROM_read(unsigned int uiAddress);

void InitInterrupts(void);

void DefaultInterupt(void);

#endif /* HAL_H_ */
