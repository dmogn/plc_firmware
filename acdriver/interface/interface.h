/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 29.10.2011 3:22:37
 */ 


#ifndef INTERFACE_H_
#define INTERFACE_H_

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

void InitModbusInterface();

uint8_t ModbusTimerCycle(msg_par message_parameter);

#endif /* INTERFACE_H_ */