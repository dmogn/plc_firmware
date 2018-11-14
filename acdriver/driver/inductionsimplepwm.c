/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 21.12.2011 15:23:08
 *
 * Implementation of frequency/voltage control algorithm for AC Induction motor servo control.
 * 
 */  

#include "os.h"

#ifdef ENABLE_AC_INDUCTION_SIMPLE

/** Internal data for PWM calculations */
typedef struct 																
{
	// calculation constants
	uint8_t phaseCount;// phase count of induction motor per revolution, by default 1
	
	// input parameters
	int16_t phase1Current;
	int16_t phase2Current;
	int16_t phase3Current;
	//int16_t rotorSpeed;
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

void initServoDrive()
{
	induction_servo_motor_data.phaseCount = INDUCTION_MOTOR_PHASE_COUNT;
	
	induction_servo_motor_data.phase1Current = 0;
	induction_servo_motor_data.phase2Current = 0;
	induction_servo_motor_data.phase3Current = 0;
	
	induction_servo_motor_data.statorFieldAngle = 0;
	induction_servo_motor_data.rotorFieldAngle = 0;
	induction_servo_motor_data.torque = 0;
	
	induction_servo_motor_data.pwmSector = 0;
	induction_servo_motor_data.pwmValue = 0;
	
	modbus_variables.driver_calculation_constants.default_s_in_angle_steps = 10; // 0.05 (s) * 200 (steps per revolution) = 10
}

/**
Steps:
1. Calculate requirement moment and direction.
2. Calculate requirement frequency (angular stator field speed).
3. Calculate requirement stator current.
4. Set PWM rates.
*/
void UpdatePWMRates(void)
{
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
	{
		disableAllPWMChannels();
		
		return;// no activity in IDLE mode
	}
	
	// Calculate requirement torque and direction.
	ServoCalculateSpeedAndTorque();
	
	//TODO: Calculate requirement frequency (angular stator field speed).
	
	
	//TODO:	Calculate requirement stator current.
	
	
	//TODO: UpdatePWMRates
	
}

#endif //ENABLE_AC_INDUCTION_SIMPLE