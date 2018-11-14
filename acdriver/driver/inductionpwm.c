/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 21:25:35
 *
 * Implementation of vector control algorithm for AC induction motor servo control.
 */ 

#include "os.h"

#ifdef ENABLE_AC_INDUCTION_SERVO

typedef struct 																
{
	// input parameters:
	int16_t phase1Current;
	int16_t phase2Current;
	int16_t phase3Current;
	int16_t rotorSpeed;
	int16_t statorFieldAngle;
	int16_t rotorFieldAngle;
	// two-way parameters
	int16_t rotorFieldSpeed;
	int16_t rotorFieldValue;
	int16_t torque;
	// output parameters
	int16_t pwmSector;
	int16_t pwmValue;
}  inductionServoMotorData;

inductionServoMotorData induction_servo_motor_data;

void UpdatePWMRates(void)
{
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
		return;// no activity in IDLE mode
		
	//TODO: UpdatePWMRates
}

#endif //ENABLE_AC_INDUCTION_SERVO