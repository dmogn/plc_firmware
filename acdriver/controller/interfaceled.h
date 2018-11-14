/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 21:13:24
*/


#ifndef INTERFACELED_H_
#define INTERFACELED_H_

/** internal data for single LED state indication */
typedef struct 																
{
	uint8_t currentControllerDisplayedMode;
	uint8_t currentControllerDisplayedErrorMode;
	uint32_t currentControllerIndicationCycleBeginTime;
	
	uint8_t currentDeviceDisplayedMode;
	uint8_t currentDeviceDisplayedErrorMode;
	uint32_t currentDeviceIndicationCycleBeginTime;
} interfaceLEDInternalData;

/** Indicate controller mode with single LED signals */
uint8_t LEDUpdateCycle(msg_par message_parameter);
 
void LEDInterfaceChangeControllerMode(uint8_t new_mode, uint8_t new_error_mode);

void LEDInterfaceChangeDeviceMode(uint8_t new_mode, uint8_t new_error_mode);

void LEDInterfaceUpdateControllerCycle(void);

void LEDInterfaceUpdateDeviceCycle(void);

void LEDInterfaceSetControllerLED(uint8_t enabled);

void LEDInterfaceSetDeviceLED(uint8_t enabled);

#endif /* INTERFACELED_H_ */