/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 29.10.2011 3:23:27
 *
 * Motor Control Driver common functions.
 */ 

#include "os.h"

void InitDriver(void)
{
	modbus_variables.driver_data.mode = DRIVER_SERVO_IDLE;
	modbus_variables.driver_data.hardware_error_code = DRIVER_HARDWARE_NO_ERRORS;
	
	modbus_variables.driver_data.flags = 0;
	modbus_variables.driver_data.rotor_angle_start_speed = 0;
	modbus_variables.driver_data.rotor_angle_start_speed_time = GetSystemTime();
	modbus_variables.driver_data.rotor_angle_acceleration_speed = 0;
	modbus_variables.driver_data.rotor_angle_end_continuous_speed = 0;
	modbus_variables.driver_data.rotor_angle_stop_position = 0;
	modbus_variables.driver_data.rotor_angle_force = 0;
	modbus_variables.driver_data.rotor_angle_direct_step = 0;
	modbus_variables.driver_data.motor_step_scale = 1; // fullstep mode
	
	modbus_variables.driver_data.motor_maximum_allowed_temperature = DEFAULT_MAXIMUM_ALLOWED_MOTOR_TEMPERATURE;
	modbus_variables.driver_data.motor_maximum_allowed_current = DEFAULT_MAXIMUM_ALLOWED_MOTOR_CURRENT;
	modbus_variables.driver_data.motor_maximum_allowed_angle_speed = DEFAULT_MAXIMUM_ALLOWED_ANGLE_SPEED;
	
	modbus_variables.driver_data_input.rotor_planned_angle_speed = 0;
	modbus_variables.driver_data_input.motor_temperature = DEFAULT_MOTOR_TEMPERATURE;
	
	modbus_variables.driver_calculation_constants.steps_per_revolution = 200;
	modbus_variables.driver_calculation_constants.rotor_intertia = 100;
	modbus_variables.driver_calculation_constants.moment_elasticity = 1000;
}

void StopAllDeviceActivity(void)
{
	disableAllPWMChannels();
	
	modbus_variables.driver_data.mode = DRIVER_SERVO_IDLE;
}

// check modbus parameters for correct structure
void CheckDriverModbusParameters(void)
{
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_CONTINUOUS_SPEED)
	{
		if (abs(modbus_variables.driver_data.rotor_angle_end_continuous_speed) > modbus_variables.driver_data.motor_maximum_allowed_angle_speed)
		{
			// report system error
			modbus_variables.driver_data.hardware_error_code = DRIVER_SOFRWARE_COMMAND_ANGLE_OVERSPEED;
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_OVERSPEED);
		}
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_AND_ACCELERATION_TO_END_SPEED)
	{
		// check for system error where acceleration direction opposite to end speed:
		if ((modbus_variables.driver_data.rotor_angle_end_continuous_speed > 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed < 0) ||
			(modbus_variables.driver_data.rotor_angle_end_continuous_speed < 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed > 0))
		{
			// report system error
			modbus_variables.driver_data.hardware_error_code = DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED;
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED);
		}
	}
}

void ServoCalculatePositionAndSpeed(void)
{
	int16_t rotor_angle_speed_error = 0;
	
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
		;
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_KEEP_POSITION)
	{
		;
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_CONTINUOUS_SPEED)
	{
		rotor_angle_speed_error = modbus_variables.driver_data.rotor_angle_end_continuous_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		//modbus_variables.driver_data_input.rotor_planned_angle_speed = modbus_variables.driver_data.rotor_angle_end_speed;
		//TODO: position
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_AND_ACCELERATION)
	{
		uint32_t time_difference = CalculateTimeInterval(GetSystemTime(), modbus_variables.driver_data.rotor_angle_start_speed_time);
		
		int32_t current_planed_speed = modbus_variables.driver_data.rotor_angle_start_speed + (modbus_variables.driver_data.rotor_angle_acceleration_speed * time_difference) / F_SYSTEM_TIMER_TICKS;
		
		if (abs(current_planed_speed) > modbus_variables.driver_data.motor_maximum_allowed_angle_speed)
			if (current_planed_speed >= 0)
				current_planed_speed = modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			else
				current_planed_speed = -modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			
		rotor_angle_speed_error = current_planed_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		//modbus_variables.driver_data_input.rotor_planned_angle_speed = (uint16_t)current_planed_speed;
		
		//TODO: position and speed
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_AND_ACCELERATION_TO_END_SPEED)
	{
		uint32_t time_difference = CalculateTimeInterval(GetSystemTime(), modbus_variables.driver_data.rotor_angle_start_speed_time);
		
		int32_t current_planed_speed = modbus_variables.driver_data.rotor_angle_start_speed + (modbus_variables.driver_data.rotor_angle_acceleration_speed * time_difference) / F_SYSTEM_TIMER_TICKS;
		
		// check for system error where acceleration direction opposite to end speed:
		if ((modbus_variables.driver_data.rotor_angle_end_continuous_speed > 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed < 0) ||
			(modbus_variables.driver_data.rotor_angle_end_continuous_speed < 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed > 0))
		{
			// report system error
			modbus_variables.driver_data.hardware_error_code = DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED;
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED);
		}			
			
		// check for end speed
		if (current_planed_speed > 0 && modbus_variables.driver_data.rotor_angle_end_continuous_speed >= 0) 
			if  (current_planed_speed > modbus_variables.driver_data.rotor_angle_end_continuous_speed)
			{
				current_planed_speed = modbus_variables.driver_data.rotor_angle_end_continuous_speed;
				// switch mode to DRIVER_SERVO_CONTINUOUS_SPEED
				modbus_variables.driver_data.mode = DRIVER_SERVO_CONTINUOUS_SPEED;
			}				
				
		if (current_planed_speed < 0 && modbus_variables.driver_data.rotor_angle_end_continuous_speed <= 0)
			if  (current_planed_speed < modbus_variables.driver_data.rotor_angle_end_continuous_speed)
			{
				current_planed_speed = modbus_variables.driver_data.rotor_angle_end_continuous_speed;
				// switch mode to DRIVER_SERVO_CONTINUOUS_SPEED
				modbus_variables.driver_data.mode = DRIVER_SERVO_CONTINUOUS_SPEED;
			}				
		
		// check for maximum mechanic speed limit
		if (abs(current_planed_speed) > modbus_variables.driver_data.motor_maximum_allowed_angle_speed)
		{
			if (current_planed_speed >= 0)
				current_planed_speed = modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			else
				current_planed_speed = -modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
				
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_OVERSPEED);
		}
			
		rotor_angle_speed_error = current_planed_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		//modbus_variables.driver_data_input.rotor_planned_angle_speed = (uint16_t)current_planed_speed;
		
		//TODO: position and speed
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_TO_END_POSITION)
	{
		//TODO:
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_CONTINUOUS_FORCE)
	{
		//TODO: report system error, not applicable mode for step motor
	}
	else if (modbus_variables.driver_data.mode == DRIVER_STEP_DIRECT)
	{
		;// direct drive, no calculations
	}
}

void ServoCalculateSpeedAndTorque(void) 
{
	int16_t rotor_angle_speed_error = 0;
	
	if (modbus_variables.driver_data.mode == DRIVER_SERVO_IDLE)
	{	
		modbus_variables.driver_data_input.rotor_planned_angle_speed = 0;
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_KEEP_POSITION)
	{
		//TODO: calculate keep position error and feedback torque
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_CONTINUOUS_SPEED)
	{
		rotor_angle_speed_error = modbus_variables.driver_data.rotor_angle_end_continuous_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		modbus_variables.driver_data_input.rotor_planned_angle_speed = modbus_variables.driver_data.rotor_angle_end_continuous_speed;
		
		//TODO: torque
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_AND_ACCELERATION)
	{
		uint32_t time_difference = CalculateTimeInterval(GetSystemTime(), modbus_variables.driver_data.rotor_angle_start_speed_time);
		
		int32_t current_planed_speed = modbus_variables.driver_data.rotor_angle_start_speed + (modbus_variables.driver_data.rotor_angle_acceleration_speed * time_difference) / F_SYSTEM_TIMER_TICKS;
		
		if (abs(current_planed_speed) > modbus_variables.driver_data.motor_maximum_allowed_angle_speed)
			if (current_planed_speed >= 0)
				current_planed_speed = modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			else
				current_planed_speed = -modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			
		rotor_angle_speed_error = current_planed_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		modbus_variables.driver_data_input.rotor_planned_angle_speed = (uint16_t)current_planed_speed;
		
		//TODO: torque
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_AND_ACCELERATION_TO_END_SPEED)
	{
		uint32_t time_difference = CalculateTimeInterval(GetSystemTime(), modbus_variables.driver_data.rotor_angle_start_speed_time);
		
		int32_t current_planed_speed = modbus_variables.driver_data.rotor_angle_start_speed + (modbus_variables.driver_data.rotor_angle_acceleration_speed * time_difference) / F_SYSTEM_TIMER_TICKS;
		
		// check for system error where acceleration direction opposite to end speed:
		if ((modbus_variables.driver_data.rotor_angle_end_continuous_speed > 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed < 0) ||
			(modbus_variables.driver_data.rotor_angle_end_continuous_speed < 0 && modbus_variables.driver_data.rotor_angle_acceleration_speed > 0))
		{
			// report system error
			modbus_variables.driver_data.hardware_error_code = DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED;
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED);
		}			
			
		// check for end speed
		if (current_planed_speed > 0 && modbus_variables.driver_data.rotor_angle_end_continuous_speed >= 0) 
			if  (current_planed_speed > modbus_variables.driver_data.rotor_angle_end_continuous_speed)
				current_planed_speed = modbus_variables.driver_data.rotor_angle_end_continuous_speed;
				
		if (current_planed_speed < 0 && modbus_variables.driver_data.rotor_angle_end_continuous_speed <= 0)
			if  (current_planed_speed < modbus_variables.driver_data.rotor_angle_end_continuous_speed)
				current_planed_speed = modbus_variables.driver_data.rotor_angle_end_continuous_speed;
		
		// check for maximum mechanic speed limit
		if (abs(current_planed_speed) > modbus_variables.driver_data.motor_maximum_allowed_angle_speed)
		{
			if (current_planed_speed >= 0)
				current_planed_speed = modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
			else
				current_planed_speed = -modbus_variables.driver_data.motor_maximum_allowed_angle_speed;
				
			ctrlReportError(DRIVER_SOFRWARE_COMMAND_ANGLE_OVERSPEED);
		}
			
		rotor_angle_speed_error = current_planed_speed - modbus_variables.driver_data_input.rotor_angle_speed;
		
		modbus_variables.driver_data_input.rotor_planned_angle_speed = (uint16_t)current_planed_speed;
		
		//TODO: torque
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_SPEED_TO_END_POSITION)
	{
		//TODO: position-driven mode
	}
	else if (modbus_variables.driver_data.mode == DRIVER_SERVO_CONTINUOUS_FORCE)
	{
		modbus_variables.driver_data_input.rotor_planned_angle_speed = modbus_variables.driver_data_input.rotor_angle_speed;
		modbus_variables.driver_data_input.rotor_planned_angle_torque = modbus_variables.driver_data.rotor_angle_force;
	}
	else if (modbus_variables.driver_data.mode == DRIVER_STEP_DIRECT)
	{
		//TODO:
	}
}
