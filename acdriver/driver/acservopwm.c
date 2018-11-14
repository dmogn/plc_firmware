/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 21:27:48
 *
 * Implementation of control algorithm for AC synchronous motor servo control.
 */ 

#include "os.h"

#ifdef ENABLE_AC_SYNCHRONOUS_SERVO

void UpdatePWMRates(void)
{
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
		return;// no activity in IDLE mode
	
	//TODO: UpdatePWMRates
}

#endif //ENABLE_AC_SYNCHRONOUS_SERVO