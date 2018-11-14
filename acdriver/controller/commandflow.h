/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 01.11.2011 20:08:27
*/


#ifndef COMMANDFLOW_H_
#define COMMANDFLOW_H_

#define CTRL_COMMANDFLOW_QUEUE_LENGTH 4
#define CTRL_COMMAND_MAX_CUSTOM_DATA_LENGTH 24
#define CTRL_COMMANDFLOW_RESULT_QUEUE_LENGTH 8
#define CTRL_COMMAND_RESULT_MAX_CUSTOM_DATA_LENGTH 16

typedef enum
{
	CTRL_COMMAND_RESULT_SUCCESS = 0
} ctrlCommandResultCode;

/** Command for real time scheduled execution in command flow */
struct ctrlCommand
{
	// id
	uint32_t id;
	// previous command id
	uint32_t previous_command_id;
	// queue number (individual for modbus device)
	uint32_t queue_number;	
	// type
	uint8_t type;
	// start network time in milliseconds
	uint32_t start_network_time;
	// start local time in milliseconds
	uint32_t start_local_time;
	// planned execution time in milliseconds
	uint32_t planned_execution_time;
	// command custom data
	uint8_t custom_data[CTRL_COMMAND_MAX_CUSTOM_DATA_LENGTH];
};

/** Result of command execution */
struct ctrlCommandResult
{
	// id
	uint32_t id;
	// previous command id
	uint32_t previous_command_id;
	// queue number (individual for modbus device)
	uint32_t queue_number;
	// type
	uint8_t type;
	// start network time in milliseconds
	uint32_t start_network_time;
	// start local time in milliseconds
	uint32_t start_local_time;
	// planned execution time in milliseconds
	uint32_t planned_execution_time;
	// result execution time in milliseconds
	uint32_t result_execution_time;
	// command result code
	ctrlCommandResultCode command_result_code;
	// command custom result data
	uint8_t result_data[CTRL_COMMAND_RESULT_MAX_CUSTOM_DATA_LENGTH];
};

#endif /* COMMANDFLOW_H_ */