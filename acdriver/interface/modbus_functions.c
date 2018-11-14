/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.01.2012 1:50:27
 *
 * Custom embedded hardware-optimized fully-asynchronous slave-mode Modbus protocol implementation with hight noise immunity.
 */ 

#include "os.h"

extern volatile UCHAR ucRTUBuf[MB_SER_PDU_SIZE_MAX];

uint16_t modbusResponseException(uint8_t start_buffer_index, uint8_t exception_code)
{
	uint8_t function_code = ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT];
	uint8_t response_exception_function_code = function_code + MODBUS_FUNC_EXCEPTION_SHIFT;
	
	// response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = response_exception_function_code;
	ucRTUBuf[2] = exception_code;
	
	// return data length
	return 3;
}

uint16_t modbusHandlerReadHoldingRegisters(uint8_t start_buffer_index)
{
	// read arguments
	uint16_t* input_arg = (uint16_t*)(ucRTUBuf+start_buffer_index);
	input_arg++;
	uint16_t starting_address = *input_arg;
	input_arg++;
	uint16_t register_count = *input_arg;
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_READ_HOLDING_REGISTERS;
	
	uint16_t* holding_registers = (uint16_t*)&modbus_variables;
	
	if (starting_address < MODBUS_HOLDING_REGISTERS_SHIFT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	const uint16_t max_register_count = sizeof(modbus_variables)/2;
	if (starting_address+register_count > MODBUS_HOLDING_REGISTERS_SHIFT+max_register_count)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	if (register_count > MODBUS_MAX_IO_RESISTERS_COUNT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);

	holding_registers += starting_address-MODBUS_HOLDING_REGISTERS_SHIFT;
	uint16_t* modbus_write_registers = (uint16_t*)(ucRTUBuf+3);
	
	// copy register data
	for (uint8_t index = 0; index < register_count; index++)
	{
		*modbus_write_registers = *holding_registers;
		modbus_write_registers++;
		holding_registers++;
	}
	
	// write registers data length 
	ucRTUBuf[2] = register_count*2;
	
	// calculate and return data length
	return 3 + register_count*2;
}

uint16_t modbusHandlerReadInputRegisters(uint8_t start_buffer_index)
{
	// read arguments
	uint16_t* input_arg = (uint16_t*)(ucRTUBuf+start_buffer_index);
	input_arg++;
	uint16_t starting_address = *input_arg;
	input_arg++;
	uint16_t register_count = *input_arg;
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_READ_INPUT_REGISTERS;
	
	uint16_t* input_registers = (uint16_t*)&modbus_variables;
	
	if (starting_address < MODBUS_INPUT_REGISTERS_SHIFT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	const uint16_t max_register_count = sizeof(modbus_variables)/2;
	if (starting_address+register_count > MODBUS_INPUT_REGISTERS_SHIFT+max_register_count)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	if (register_count > MODBUS_MAX_IO_RESISTERS_COUNT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
		
	input_registers += starting_address-MODBUS_INPUT_REGISTERS_SHIFT;
	uint16_t* modbus_write_registers = (uint16_t*)(ucRTUBuf+3);
	
	// copy register data
	for (uint8_t index = 0; index < register_count; index++)
	{
		*modbus_write_registers = *input_registers;
		modbus_write_registers++;
		input_registers++;
	}
	
	// write registers data length 
	ucRTUBuf[2] = register_count*2;
	
	// calculate and return data length
	return 3 + register_count*2;
}

uint16_t modbusHandlerWriteSingleRegister(uint8_t start_buffer_index)
{
	// read arguments
	uint16_t* input_arg = (uint16_t*)(ucRTUBuf+start_buffer_index);
	input_arg++;
	uint16_t register_address = *input_arg;
	input_arg++;
	uint16_t register_value = *input_arg;
	
	uint16_t* holding_registers = (uint16_t*)&modbus_variables;
	uint16_t max_register_ingex = sizeof(modbus_variables)/2 - 1;
	
	if (register_address < MODBUS_HOLDING_REGISTERS_SHIFT ||
		register_address > MODBUS_HOLDING_REGISTERS_SHIFT + max_register_ingex)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	// write register
	holding_registers[register_address] = register_value;
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_WRITE_SINGLE_REGISTER;

	uint16_t* output_arg = (uint16_t*)(ucRTUBuf);
	output_arg++;
	*output_arg = register_address;
	output_arg++;
	*output_arg = register_value;
	
	// return data length
	return 6;
}

uint16_t modbusHandlerDiagnostics(uint8_t start_buffer_index)
{
	// read arguments
	//TODO:
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_DIAGNOSTICS;
	//TODO:
	
	// calculate and return data length
	return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE);
}

uint16_t modbusHandlerWriteMultipleRegisters(uint8_t start_buffer_index)
{
	// read arguments
	uint16_t* input_arg = (uint16_t*)(ucRTUBuf+start_buffer_index);
	input_arg++;
	uint16_t starting_address = *input_arg;
	input_arg++;
	uint16_t register_count = *input_arg;
	uint8_t data_bytes = ucRTUBuf[start_buffer_index+6];
	uint16_t* input_registes = (uint16_t*)(ucRTUBuf+start_buffer_index+7);
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS;
	
	uint16_t* holding_registers = (uint16_t*)&modbus_variables;
	
	if (starting_address < MODBUS_HOLDING_REGISTERS_SHIFT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	const uint16_t max_register_count = sizeof(modbus_variables)/2;
	if (starting_address+register_count > MODBUS_HOLDING_REGISTERS_SHIFT+max_register_count)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);
	
	if (register_count > MODBUS_MAX_IO_RESISTERS_COUNT)
		return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS);

	holding_registers += starting_address-MODBUS_HOLDING_REGISTERS_SHIFT;
	
	// copy register data
	for (uint8_t index = 0; index < register_count; index++)
	{
		*holding_registers = *input_registes;
		input_registes++;
		holding_registers++;
	}
	
	// response arguments
	uint16_t* output_arg = (uint16_t*)(ucRTUBuf+start_buffer_index);
	output_arg++;
	*output_arg = starting_address;
	output_arg++;
	*output_arg = register_count;
	
	// calculate and return data length
	return 6;
}

uint16_t modbusHandlerReportSlaveID(uint8_t start_buffer_index)
{
	// read arguments
	//TODO:
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_REPORT_SLAVE_ID;
	//TODO:
	
	// calculate and return data length
	return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE);
}

uint16_t modbusHandlerReadDeviceIdentification(uint8_t start_buffer_index)
{
	// read arguments
	//TODO:
	
	// write response
	ucRTUBuf[MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT] = MODBUS_FUNC_UTILITE;
	//TODO:
	
	// calculate and return data length
	return modbusResponseException(start_buffer_index, MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE);
}
