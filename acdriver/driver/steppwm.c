/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 20:54:17
 */ 

#include "os.h"

#ifdef ENABLE_STEP_SERVO

typedef struct 																
{
	// calculation constants
	
	// input parameters
	//int16_t phase1Current;// phase current in milliamperes
	//int16_t phase2Current;// phase current in milliamperes
	//int16_t phase3Current;// phase current in milliamperes
	//int16_t phase4Current;// phase current in milliamperes
	
	// ADC value for phase current zero point (in ADC measurement units)
	uint8_t phaseCurrentZeroPoint;
	
	// two-way parameters
	
	/** collected rotor angle error in steps */
	int32_t rotor_collected_angle_error;
	/** collected rotor angle error in steps */
	int32_t rotor_angle_error_last_calculation_time;
	
	// output parameters
	//uint8_t pwmMode;// 0, 1, 2, 3  step mode for 2-phase bipolar and 4-phase unipolar step motor
	int8_t pwmMicroStepMode;// 0-3 sub-steps (microsteps) mode and 4-15 full step + microstep mode
	uint32_t currentStepStartTime;
}  stepMotorData;

stepMotorData step_motor_data;

void StepMotorSetNextStep(int8_t direction);
void StepMotorKeepPositionUpdatePWMRates(void);

void initServoDrive()
{
	//step_motor_data.phase1Current = 0;
	//step_motor_data.phase2Current = 0;
	//step_motor_data.phase3Current = 0;
	//step_motor_data.phase4Current = 0;
	
	//TODO: ADC zero point adjustment
	step_motor_data.phaseCurrentZeroPoint = 0;
	
	//step_motor_data.pwmMode = 0;
	step_motor_data.pwmMicroStepMode = 0;
	step_motor_data.currentStepStartTime = GetSystemTime();
}

void UpdatePWMRates(void)
{
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
		return;// no activity in IDLE mode
	
	// update phase currents
	//step_motor_data.phase1Current = ADCGetChannelValue(0) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;
	//step_motor_data.phase2Current = ADCGetChannelValue(1) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;
	//step_motor_data.phase3Current = ADCGetChannelValue(3) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;
	//step_motor_data.phase4Current = ADCGetChannelValue(4) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;
	
	// calculate required position
	ServoCalculatePositionAndSpeed();
	
	if (modbus_variables.driver_data.mode == DRIVER_STEP_DIRECT)
	{
		if (abs(modbus_variables.driver_data.rotor_angle_direct_step) == 1)
		{
			StepMotorSetNextStep((int8_t)modbus_variables.driver_data.rotor_angle_direct_step);
			modbus_variables.driver_data.rotor_angle_direct_step = 0;
		}
		
		if (modbus_variables.driver_data.rotor_angle_direct_step != 0)
		{
			//TODO: report illegal parameters: direct step must be +-1 or 0. 
		}
		
		// no other speed or position-derived calculations in direct step mode
		return;
	}
	
	// find current step time derived from speed
	if (modbus_variables.driver_data_input.rotor_planned_angle_speed != 0)
	{
		// next step time interval in system ticks
		uint32_t next_step_time_interval = F_SYSTEM_TIMER_TICKS / abs(modbus_variables.driver_data_input.rotor_planned_angle_speed);
		uint32_t current_step_time_interval = CalculateTimeInterval(GetSystemTime(), step_motor_data.currentStepStartTime);
		if (current_step_time_interval >= next_step_time_interval)
		{
			if (modbus_variables.driver_data_input.rotor_planned_angle_speed > 0)
				StepMotorSetNextStep(1);
			else
				StepMotorSetNextStep(-1);
		}
		else
			StepMotorKeepPositionUpdatePWMRates();
	}
	else
	{
		StepMotorKeepPositionUpdatePWMRates();
	}	
}

void StepMotorKeepPositionUpdatePWMRates(void)
{
	// find current mode
	uint8_t pwmStepMode = step_motor_data.pwmMicroStepMode / 4;
	uint8_t pwmSubStepMode = step_motor_data.pwmMicroStepMode % 4;
	
	uint8_t pwmEnabledChannel1 = 0;
	uint8_t pwmEnabledChannel2 = 0;
	
	uint8_t currentChanne1Index = 0;
	uint8_t currentChanne2Index = 0;
	
	if (pwmStepMode == 0)
	{
		pwmEnabledChannel1 = 0;
		pwmEnabledChannel2 = 2;
		currentChanne1Index = 1;
		currentChanne2Index = 3;
	}
	else if (pwmStepMode == 1)
	{
		pwmEnabledChannel1 = 2;
		pwmEnabledChannel2 = 1;
		currentChanne1Index = 3;
		currentChanne2Index = 0;
	}
	else if (pwmStepMode == 2)
	{
		pwmEnabledChannel1 = 1;
		pwmEnabledChannel2 = 3;
		currentChanne1Index = 0;
		currentChanne2Index = 2;
	}
	else if (pwmStepMode == 3)
	{
		pwmEnabledChannel1 = 3;
		pwmEnabledChannel2 = 0;
		currentChanne1Index = 2;
		currentChanne2Index = 1;
	}
	
	// disable unused channels
	for (uint8_t ui=0; ui<4; ui++)
	{
		if (ui != pwmEnabledChannel1 && ui != pwmEnabledChannel2)
			setPWMChannelParameters(ui, FALSE, STEP_MOTOR_PWM_PRESCALER, 0);
	}
	
	// calculate current data
	int16_t channel1Current = (ADCGetChannelValue(currentChanne1Index) - step_motor_data.phaseCurrentZeroPoint) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;// phase current in milliamperes
	int16_t channel2Current = (ADCGetChannelValue(currentChanne2Index) - step_motor_data.phaseCurrentZeroPoint) * MOTOR_PHASE_CURRENT_PER_ADC_UNIT;// phase current in milliamperes 
	
	// calculate microstep coefficients
	uint8_t channel1MicrostepCoefficient = 4-pwmSubStepMode;
	uint8_t channel2MicrostepCoefficient = pwmSubStepMode;
	
	//TODO: calculate overload acceleration rates
	uint8_t overloadCoefficient = 1;
	
	// update used channels PWM rates
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_KEEP_POSITION)
	{// keep position mode low current
		setPWMChannelParameters(pwmEnabledChannel1, TRUE, STEP_MOTOR_PWM_PRESCALER, STEP_MOTOR_DEFAULT_KEEP_POSITION_PWM_DURATION * channel1MicrostepCoefficient / 4);
		setPWMChannelParameters(pwmEnabledChannel2, TRUE, STEP_MOTOR_PWM_PRESCALER, STEP_MOTOR_DEFAULT_KEEP_POSITION_PWM_DURATION * channel2MicrostepCoefficient / 4);		
	}
	else
	{// moving mode with full current
		uint16_t channel1Duration = STEP_MOTOR_DEFAULT_FULL_LOAD_PWM_DURATION * overloadCoefficient * channel1MicrostepCoefficient / 4;
		if (channel1Duration > STEP_MOTOR_PWM_PRESCALER)
			channel1Duration = STEP_MOTOR_PWM_PRESCALER;// 100% duration
			
		uint16_t channel2Duration = STEP_MOTOR_DEFAULT_FULL_LOAD_PWM_DURATION * overloadCoefficient * channel2MicrostepCoefficient / 4;
		if (channel2Duration > STEP_MOTOR_PWM_PRESCALER)
			channel2Duration = STEP_MOTOR_PWM_PRESCALER;// 100% duration
			
		setPWMChannelParameters(pwmEnabledChannel1, TRUE, STEP_MOTOR_PWM_PRESCALER, channel1Duration);
		setPWMChannelParameters(pwmEnabledChannel2, TRUE, STEP_MOTOR_PWM_PRESCALER, channel2Duration);
	}	
}

/** direction must be -1 or +1 */
void StepMotorSetNextStep(int8_t direction)
{
	// calculate shift
	int8_t microstep_scaler = 4;// full step
	if (modbus_variables.driver_data.motor_step_scale == 2)
		microstep_scaler = 2;
	else if (modbus_variables.driver_data.motor_step_scale == 4)
		microstep_scaler = 1;
	
	// update step mode data
	step_motor_data.pwmMicroStepMode += microstep_scaler * direction;
	if (step_motor_data.pwmMicroStepMode < 0)
		step_motor_data.pwmMicroStepMode += 64;
	if (step_motor_data.pwmMicroStepMode > 15)
		step_motor_data.pwmMicroStepMode = step_motor_data.pwmMicroStepMode % 16;
	
	step_motor_data.currentStepStartTime = GetSystemTime();
	
	// set PWM rates for new step position
	StepMotorKeepPositionUpdatePWMRates();
}

#endif //ENABLE_STEP_SERVO