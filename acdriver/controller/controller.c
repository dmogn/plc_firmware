/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 01.11.2011 9:20:19
*/

#include "os.h"

void ctrlSetMode(uint16_t new_mode, uint16_t new_error_code) 
{
	uint16_t old_mode = modbus_variables.controller_data.mode;
	uint16_t old_error_code = modbus_variables.controller_data.error_code;
	
	modbus_variables.controller_data.mode = new_mode;
	ctrlReportError(new_error_code);
	
	if (new_mode == CTRLR_MODE_IDLE)
	{
		StopAllDeviceActivity();
	}
	else if (new_mode == CTRLR_MODE_RESET)
	{
		StopAllDeviceActivity();
		ResetHardware();
	}
			
	//TODO: ctrlSetMode() - other modes
}

void initController(void)
{
	modbus_variables.controller_data.mode = CTRLR_MODE_IDLE;
	modbus_variables.controller_data.error_code = CTRLR_NO_ERRORS;
	modbus_variables.controller_data.maximum_radiator_temperature = DEFAULT_MAXIMUM_RADIATOR_TEMPERATURE;
	modbus_variables.controller_data.radiator_temperature = DEFAULT_RADIATOR_TEMPERATURE;
	modbus_variables.controller_data.device_time = 0;
	modbus_variables.controller_data.network_time_shift = 0;
	modbus_variables.controller_data.maximum_system_step_timeout = 0;
	
	ctrlInitLog();
	
	//TODO: read settings from memory storage
}

uint8_t ctrlUpdateStep(msg_par message_parameter)
{
	// update maximum_system_step_timeout indication parameter
	uint32_t step_time_shift = CalculateTimeInterval(GetSystemTime(), modbus_variables.controller_data.device_time);
	if (step_time_shift > UINT16_MAX)
		step_time_shift = UINT16_MAX;
	modbus_variables.controller_data.maximum_system_step_timeout = step_time_shift;
	
	// update device_time indication parameter
	modbus_variables.controller_data.device_time = GetSystemTime();
	
	return 0;
}

void ctrlInitLog(void)
{
	// clear log report structures
	for (uint8_t index=0; index<CONTROLLER_LOG_SIZE; index++) 
	{
		modbus_variables.system_log[index].error_code = CTRLR_NO_ERRORS;
		modbus_variables.system_log[index].count = 0;
		modbus_variables.system_log[index].last_parameter = 0;
		modbus_variables.system_log[index].last_time = 0;
	}
	
	// first structure reserved for "other error" reports
	modbus_variables.system_log[0].error_code = CTRLR_OTHER_ERROR;
}
void ctrlReportError(uint16_t error_code)
{
	ctrlReportErrorWithParameter(error_code, ZERO_ERROR_LOG_PARAMETER);
}

void ctrlReportErrorWithParameter(uint16_t error_code, uint16_t parameter)
{
	if (error_code == CTRLR_NO_ERRORS)
		return;// nothing
		
	modbus_variables.controller_data.error_code = error_code;
	
	// clear log report structures
	for (uint8_t index=0; index<CONTROLLER_LOG_SIZE; index++) 
	{
		if (modbus_variables.system_log[index].error_code == CTRLR_NO_ERRORS) 
		{
			// empty report structure - initialize for this error type
			modbus_variables.system_log[index].error_code = error_code;
		}
		
		if (modbus_variables.system_log[index].error_code == error_code)
		{
			// same error structure - report error
			if (modbus_variables.system_log[index].count < UINT16_MAX)
				modbus_variables.system_log[index].count++;
				
			modbus_variables.system_log[index].last_parameter = parameter;
			modbus_variables.system_log[index].last_time = GetSystemTime();
			return;
		}
	}
	
	// no structures for this type reports and no empty structures -> report to zero structure reserved for "other error" reports
	if (modbus_variables.system_log[0].count < UINT16_MAX)
		modbus_variables.system_log[0].count++;
				
	modbus_variables.system_log[0].last_parameter = parameter;
	modbus_variables.system_log[0].last_time = GetSystemTime();
}