/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 21:12:06
*/

#include "os.h"

interfaceLEDInternalData interface_led_internal_data;

/** Indicate controller mode with single LED signals */
uint8_t LEDUpdateCycle(msg_par message_parameter)
{
	// check for controller mode update
	if (interface_led_internal_data.currentControllerDisplayedMode != modbus_variables.controller_data.mode
		|| interface_led_internal_data.currentControllerDisplayedErrorMode != modbus_variables.controller_data.error_code)
		LEDInterfaceChangeControllerMode(modbus_variables.controller_data.mode, modbus_variables.controller_data.error_code);
	
	// check for device mode update
	if (interface_led_internal_data.currentDeviceDisplayedMode != modbus_variables.driver_data.mode
		|| interface_led_internal_data.currentDeviceDisplayedErrorMode != modbus_variables.driver_data.hardware_error_code)
		LEDInterfaceChangeDeviceMode(modbus_variables.driver_data.mode, modbus_variables.driver_data.hardware_error_code);
	
	LEDInterfaceUpdateControllerCycle();

	LEDInterfaceUpdateDeviceCycle();
	
	return 0;
}

void LEDInterfaceChangeControllerMode(uint8_t new_mode, uint8_t new_error_mode)
{
	interface_led_internal_data.currentControllerDisplayedMode = new_mode;
	interface_led_internal_data.currentControllerDisplayedErrorMode = new_error_mode;
	// set current time
	interface_led_internal_data.currentControllerIndicationCycleBeginTime = GetSystemTime();
}

void LEDInterfaceChangeDeviceMode(uint8_t new_mode, uint8_t new_error_mode)
{
	interface_led_internal_data.currentDeviceDisplayedMode = new_mode;
	interface_led_internal_data.currentDeviceDisplayedErrorMode = new_error_mode;
	// set current time
	interface_led_internal_data.currentDeviceIndicationCycleBeginTime = GetSystemTime();
}

void LEDInterfaceUpdateControllerCycle(void)
{
	uint16_t time_shift = CalculateTimeInterval(GetSystemTime(), interface_led_internal_data.currentControllerIndicationCycleBeginTime);
	
	if (interface_led_internal_data.currentControllerDisplayedErrorMode != CTRLR_NO_ERRORS)
	{	
		// show only error
		// light with 0.3 second frequency
		if (time_shift == 0)
			LEDInterfaceSetControllerLED(TRUE);// enable LED
		else if (time_shift < 300)
			;// LED enabled
		else if (time_shift == 300)
			LEDInterfaceSetControllerLED(FALSE);// disable LED
		else if (time_shift < 600)
			;// LED disabled
		else if (time_shift >= 600)
			interface_led_internal_data.currentControllerIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	if (interface_led_internal_data.currentControllerDisplayedMode == CTRLR_MODE_IDLE)
	{
		// light with 0.5 second Hi 4.5 seconds Low frequency
		if (time_shift == 0)
			LEDInterfaceSetControllerLED(TRUE);// enable LED
		else if (time_shift < 500)
			;// LED enabled
		else if (time_shift == 500)
			LEDInterfaceSetControllerLED(FALSE);// disable LED
		else if (time_shift < 5000)
			;// LED disabled
		else if (time_shift >= 5000)
			interface_led_internal_data.currentControllerIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	// other modes
	{
		// light with 1.5 seconds  frequency
		if (time_shift == 0)
			LEDInterfaceSetControllerLED(TRUE);// enable LED
		else if (time_shift < 750)
			;// LED enabled
		else if (time_shift == 750)
			LEDInterfaceSetControllerLED(FALSE);// disable LED
		else if (time_shift < 1500)
			;// LED disabled
		else if (time_shift >= 1500)
			interface_led_internal_data.currentControllerIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	// clear timer for other modes
	//if (time_shift >= UINT16_MAX - 100)
	//	interface_led_internal_data.currentControllerIndicationCycleBeginTime = GetSystemTime();
}

void LEDInterfaceUpdateDeviceCycle(void)
{
	uint16_t time_shift = CalculateTimeInterval(GetSystemTime(), interface_led_internal_data.currentDeviceIndicationCycleBeginTime);	
	
	if (interface_led_internal_data.currentDeviceDisplayedErrorMode != DRIVER_HARDWARE_NO_ERRORS)
	{	
		// show only error
		// light with 0.3 second frequency
		if (time_shift == 0)
			LEDInterfaceSetDeviceLED(TRUE);// enable LED
		else if (time_shift < 300)
			;// LED enabled
		else if (time_shift == 300)
			LEDInterfaceSetDeviceLED(FALSE);// disable LED
		else if (time_shift < 600)
			;// LED disabled
		else if (time_shift >= 600)
			interface_led_internal_data.currentDeviceIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	if (interface_led_internal_data.currentDeviceDisplayedMode == DRIVER_SERVO_IDLE)
	{	
		// show only error
		// light with 0.5 second Hi 4.5 seconds Low frequency
		if (time_shift == 0)
			LEDInterfaceSetDeviceLED(TRUE);// enable LED
		else if (time_shift < 500)
			;// LED enabled
		else if (time_shift == 500)
			LEDInterfaceSetDeviceLED(FALSE);// disable LED
		else if (time_shift < 5000)
			;// LED disabled
		else if (time_shift >= 5000)
			interface_led_internal_data.currentDeviceIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	// other modes
	{
		// light with 1.5 seconds  frequency
		if (time_shift == 0)
			LEDInterfaceSetDeviceLED(TRUE);// enable LED
		else if (time_shift < 750)
			;// LED enabled
		else if (time_shift == 750)
			LEDInterfaceSetDeviceLED(FALSE);// disable LED
		else if (time_shift < 1500)
			;// LED disabled
		else if (time_shift >= 1500)
			interface_led_internal_data.currentDeviceIndicationCycleBeginTime = GetSystemTime();// end cycle
			
		return;
	}
	
	// clear timer for other modes
	//if (time_shift >= UINT16_MAX - 100)
	//	interface_led_internal_data.currentDeviceIndicationCycleBeginTime = GetSystemTime();
}

void LEDInterfaceSetControllerLED(uint8_t enabled)
{
	//TODO: 
}

void LEDInterfaceSetDeviceLED(uint8_t enabled)
{
	//TODO: 
}