/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.11.2011 20:58:13
 */ 

#include "os.h"

#define ANGLE_CHANNELS_MASK 0x0F
#define ANGLE_CHANNELS_UNKNOWN_STATE 0xFF

uint8_t angleConvertChannelsToPhase(uint8_t channels);

typedef struct 																
{
	uint8_t current_state;
	uint8_t last_state;
	uint32_t last_state_time;
	uint8_t last_last_state;
	uint32_t last_last_state_time;
}  anglepositionDataInput;	

anglepositionDataInput angle_position_data_input;

void InitAngleInput(void)
{
	angle_position_data_input.current_state = ANGLE_CHANNELS_UNKNOWN_STATE;
	angle_position_data_input.last_state = ANGLE_CHANNELS_UNKNOWN_STATE;
	angle_position_data_input.last_state_time = 0;
	angle_position_data_input.last_last_state = ANGLE_CHANNELS_UNKNOWN_STATE;
	angle_position_data_input.last_last_state_time = 0;
}

/** 
Calculate current absolute position for noise reduction
*/
void UpdateAngleInputValues(void)
{
	// update current position channels
	angle_position_data_input.current_state = GPIO_CHANNEL_0_IN_PORT & ANGLE_CHANNELS_MASK;
	
	
	if (angle_position_data_input.last_state == ANGLE_CHANNELS_UNKNOWN_STATE)
	{
		// first run, set relative position only 
		angle_position_data_input.last_state = angle_position_data_input.current_state;
		return;
	}
	
	if (angle_position_data_input.last_state != angle_position_data_input.current_state)
	{
		// step detected, update position
		//TODO: 
		// detect current phase and direction
		uint8_t phase = angleConvertChannelsToPhase(angle_position_data_input.current_state);
		uint8_t last_phase = angleConvertChannelsToPhase(angle_position_data_input.last_state);
		
		if (angle_position_data_input.last_last_state != ANGLE_CHANNELS_UNKNOWN_STATE)
		{
			//TODO: ignore with very long last state duration, because motion started
			
			// mix result with last last step result for noise protection
			//TODO: 
		}
		
		// update last last result
		angle_position_data_input.last_last_state = angle_position_data_input.last_state;
		angle_position_data_input.last_last_state_time = angle_position_data_input.last_state_time;
		angle_position_data_input.last_state = angle_position_data_input.current_state;
		angle_position_data_input.last_state_time = GetSystemTime();
	}
	else
	{
		//update speed, if long step time (> last step duration)
		//TODO: 
	}
}

uint8_t angleConvertChannelsToPhase(uint8_t channels)
{
	channels &= 3;// 2 channels mask
	
	if (channels == 0)
		return 0;
	else if (channels == 1)
		return 1;
	else if (channels == 3)
		return 2;
	else // channels == 2
		return 3;
}