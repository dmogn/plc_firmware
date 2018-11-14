/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 02.01.2012 0:01:32
 *
 * Custom embedded hardware-optimized fully-asynchronous slave-mode Modbus protocol implementation with hight noise immunity.
 */

#include "os.h"

extern volatile UCHAR ucRTUBuf[MB_SER_PDU_SIZE_MAX];

typedef enum
{
	MODBUS_INTERFACE_DISABLED = 0,
	MODBUS_INTERFACE_READ_MODE,
	MODBUS_INTERFACE_WRITE_MODE,
	MODBUS_INTERFACE_FRAME_PROCESSING_MODE
} modbusInterfaceMode; 

typedef struct {
	uint8_t predicted_frame_start_index;// byte index in ucRTUBuf
	uint8_t predicted_frame_size;// if value > 0 then structure is active, minimum frame size is MODBUS_INTERFACE_FRAME_CHECK_HEADER_SIZE
} modbusInterfaceFrameCheck;

/** Data for controller modbus state */
typedef struct
{
	uint8_t device_number;
	uint8_t currentMode;
	
	// read mode variables
	uint16_t read_mode_buffer_readed_size;	
	uint32_t last_byte_received_time;
	modbusInterfaceFrameCheck modbus_interface_frame_check[MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE];

	// write mode variables
	uint8_t current_write_mode_buffer_index;
	uint8_t current_write_mode_buffer_length;
	
} modbusInterfaceData;	

modbusInterfaceData modbus_interface_data;

static const PROGMEM uint8_t modbus_supported_functions[] = 
{
	MODBUS_FUNC_READ_HOLDING_REGISTERS, // 0x03 Read Holding Registers
	MODBUS_FUNC_READ_INPUT_REGISTERS, // 0x04 Read Input Registers
	MODBUS_FUNC_WRITE_SINGLE_REGISTER, // 0x06 Write Single Register
	MODBUS_FUNC_DIAGNOSTICS, // 0x08 Diagnostics
	MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS, // 0x10 Write Multiple registers
	MODBUS_FUNC_REPORT_SLAVE_ID, // 0x11 Report Slave ID
	MODBUS_FUNC_UTILITE, // 0x2B, + 0x0E Read Device Identification
	0// array end
};

void modbusInitInterface( void )
{
	modbus_interface_data.device_number = MODBUS_INTERFACE_DEFAULT_DEVICE_NUMBER;
	modbus_interface_data.currentMode = MODBUS_INTERFACE_READ_MODE;
	
	modbusClearReadStructures();	

	modbus_interface_data.current_write_mode_buffer_index = 0;
	modbus_interface_data.current_write_mode_buffer_length = 0;
}

void clearFrameCheckStructure(uint8_t index)
{// clear frame check structure
	modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size = 0;
	modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index = 0;
}

void modbusClearReadStructures( void )
{
	modbus_interface_data.last_byte_received_time = GetSystemTime();
	modbus_interface_data.read_mode_buffer_readed_size = 0;
	
	for (uint8_t index=0; index<MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE; index++)
	{
		clearFrameCheckStructure(index);
	}
}

void modbusUARTByteReceived(uint8_t receivedByte)
{
	if (modbus_interface_data.currentMode != MODBUS_INTERFACE_READ_MODE)
		return;
	
	// check for maximum timeout
	uint32_t timeout = CalculateTimeInterval(GetSystemTime(), modbus_interface_data.last_byte_received_time);
	if (timeout > MODBUS_INTERFACE_MAX_UART_TIMEOUT)
	{
		// timeout detected, clear read buffer
		modbusClearReadStructures();
	}
	modbus_interface_data.last_byte_received_time = GetSystemTime();
	
	// process received byte
	if (modbus_interface_data.read_mode_buffer_readed_size == 0)
	{// watch for device address or broadband address
		if (receivedByte == MODBUS_INTERFACE_BROADBAND_DEVICE_NUMBER || receivedByte == modbus_interface_data.device_number)
		{// possibly the packet start, addressed ti this device
			ucRTUBuf[0] = receivedByte;
			modbus_interface_data.read_mode_buffer_readed_size++;
			
			modbus_interface_data.modbus_interface_frame_check[0].predicted_frame_start_index = 0;
			modbus_interface_data.modbus_interface_frame_check[0].predicted_frame_size = 4;// minimum frame size
			
			return;
		}
	}
	else
	{
		ucRTUBuf[modbus_interface_data.read_mode_buffer_readed_size] = receivedByte;
		modbus_interface_data.read_mode_buffer_readed_size++;
		
		// predict size, check predicted frames for completion
		for (uint8_t index=0; index<MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE; index++)
		{
			// predict frame size with default size for function number
			if (modbus_interface_data.read_mode_buffer_readed_size == MODBUS_INTERFACE_FRAME_CHECK_HEADER_SIZE)
			{
				if (modbusCheckPacketHeader(modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index) == 0)
				{
					// not allowed header, clear frame check structure
					clearFrameCheckStructure(index);
				}
				
				// check for allowed modbus function
				uint8_t modbus_function = ucRTUBuf[modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index];
				
				BOOL check_func_result = FALSE;
				for (uint8_t func_index=0; modbus_supported_functions[func_index] != 0; func_index++)
				{
					if (modbus_supported_functions[func_index] == modbus_function)
					{
						check_func_result = TRUE;
						break;
					}
				}
				
				// not allowed function, ignore
				if (check_func_result == FALSE)
				{
					clearFrameCheckStructure(index);
				}
				
				// find frame size
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size = modbusGetFrameSize(modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index);
			}
			
			// check frame for completion
			if (modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size > 0 && modbus_interface_data.read_mode_buffer_readed_size == 
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index +
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size)
			{
				// check for frame size (could be corrected for functions with variable frame size)
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size = modbusGetFrameSize(modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index);
			
				if (modbus_interface_data.read_mode_buffer_readed_size == 
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index +
				modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size)
				{
					//frame completed
					if (modbusProcessFrame(modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_start_index, modbus_interface_data.modbus_interface_frame_check[index].predicted_frame_size))
						return;// buffer and read structures already cleaned in modbusProcessFrame()
					else
					{// clear frame check structure
						clearFrameCheckStructure(index);
					}
				}
			}
		}
		
		// check for new frame
		if (receivedByte == MODBUS_INTERFACE_BROADBAND_DEVICE_NUMBER || receivedByte == modbus_interface_data.device_number)
		{// possibly the packet start, addressed this device or broadband
		
			// find empty frame structure
			for (uint8_t frame_index = 0; frame_index < MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE; frame_index++)
			{
				if (modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_size == 0)
				{// empty structure found
					// fill frame structure
					modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_start_index = 
						modbus_interface_data.read_mode_buffer_readed_size - 1;
					modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_size = 4;// minimum frame size
					break;
				}
			}
		}
		
		// check for buffer overflow
		if (modbus_interface_data.read_mode_buffer_readed_size == MB_SER_PDU_SIZE_MAX)
		{
			// buffer overflow prevention
			// find first actual frame structure 
			uint16_t first_frame_start_index = MB_SER_PDU_SIZE_MAX;
			uint8_t first_frame_index = MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE;
			
			// find first frame structure by buffer start index
			for (uint8_t frame_index = 0; frame_index < MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE; frame_index++)
			{
				if (modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_size > 0)
				{// not empty structure found
					if (modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_start_index < first_frame_start_index)
					{
						first_frame_start_index = modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_start_index;
						first_frame_index = frame_index;
					}
				}
			}
			
			// shift data
			uint16_t datalength = MB_SER_PDU_SIZE_MAX - first_frame_index;
			
			for (uint16_t index = 0; index < datalength; index++)
			{
				ucRTUBuf[index] = ucRTUBuf[index + first_frame_start_index];
			}
			
			// shift frame structures start indexes
			for (uint8_t frame_index = 0; frame_index < MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE; frame_index++)
			{
				if (modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_size > 0)
				{// not empty structure found
					modbus_interface_data.modbus_interface_frame_check[frame_index].predicted_frame_start_index -= first_frame_start_index;
				}
			}
			
			modbus_interface_data.read_mode_buffer_readed_size -= first_frame_start_index;
		}
	}
}

void modbusUARTTransmitterBufferEmpty( void )
{
	if (modbus_interface_data.currentMode != MODBUS_INTERFACE_WRITE_MODE)
		return;
	
	if (modbus_interface_data.current_write_mode_buffer_index >= modbus_interface_data.current_write_mode_buffer_length)
	{
		// transmission finished
		modbus_interface_data.currentMode = MODBUS_INTERFACE_READ_MODE;
		modbus_interface_data.current_write_mode_buffer_index = 0;
		modbus_interface_data.current_write_mode_buffer_length = 0;
		USART_DisableDataRegisterEmptyInterrupt();
	}
	else
	{
		// transmit next byte from buffer
		USART_Transmit(ucRTUBuf[modbus_interface_data.current_write_mode_buffer_index]);
		modbus_interface_data.current_write_mode_buffer_index++;
	}
}

void modbusUARTByteTransmitted( void )
{
	if (modbus_interface_data.currentMode != MODBUS_INTERFACE_WRITE_MODE)
	{
		// switch UART to read mode
		USART_SetRS485Mode(UART_RS485_MODE_RECEIVE);
		return;
	}
}

/** check first 3 readed bytes for allowed header. Result is boolean value. */
uint8_t modbusCheckPacketHeader(uint8_t start_buffer_index) 
{
	// check for device address or broadband address
	if (ucRTUBuf[start_buffer_index] != MODBUS_INTERFACE_BROADBAND_DEVICE_NUMBER && ucRTUBuf[start_buffer_index] != modbus_interface_data.device_number)
		return FALSE;// not frame with device address or broadband address
		
	// check for allowed function code
	uint8_t function_code = ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT];
	BOOL allowed_function = FALSE;
	for (uint8_t index; modbus_supported_functions[index]!=0; index++)
	{
		if (function_code == modbus_supported_functions[index])
		{
			allowed_function = TRUE;
			break;
		}
	}
	
	// check some functions subcodes
	if (function_code == MODBUS_FUNC_UTILITE && ucRTUBuf[start_buffer_index+2] != MODBUS_FUNC_UTILITE_SUBCODE_READ_DEVICE_IDENTIFICATION)
		return FALSE;// 0x2B + 0x0E Read Device Identification
	
	return allowed_function;
}

/** 
Check first 3 readed bytes for allowed header. Result is full predicted frame size (with device address & CRC data). 
Return 0 if error has been occured. 
*/
uint8_t modbusGetPredictedFrameSize(uint8_t start_buffer_index)
{
	uint8_t frame_size = 0;
	
	// check function code and return frame length
	uint8_t function_code = ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT];
	if (function_code == 0x03)
		frame_size = 5;//Read Holding Registers
	else if (function_code == 0x04)
		frame_size = 5;//Read Input Registers
	else if (function_code == 0x06)
		frame_size = 5;//Write Single Register
	else if (function_code == 0x08)
		frame_size = 5;//Diagnostics - only 2 bytes data supported
	else if (function_code == 0x10)
	// need 6 bytes to find data length, minimum size with data - 8 bytes
		frame_size = 8;//Write Multiple registers 
	else if (function_code == 0x11)
		frame_size = 1;//Report Slave ID
	else if (function_code == 0x2B && ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_SUBCODE_SHIFT] == 0x0E)
		frame_size = 4;//Read Device Identification
	
	if (frame_size > 0)
		return frame_size + 1 /* device address byte */ + 2 /* control sum bytes */;
	else
		return 0;// error - function not found
}

uint8_t modbusGetFrameSize(uint8_t start_buffer_index)
{
	uint8_t frame_size = 0;
	
	// check function code and return frame length
	uint8_t function_code = ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT];
	if (function_code == 0x10)
		frame_size = ucRTUBuf[start_buffer_index+6];//Write Multiple registers 
	
	if (frame_size > 0)
		return frame_size + 1 /* device address byte */ + 2 /* control sum bytes */;
	else
		return modbusGetPredictedFrameSize(start_buffer_index);// frame size same as predicted
}

uint8_t modbusProcessFrame(uint8_t start_buffer_index, uint16_t frame_length)
{
	uint16_t crc = usMBCRC16(ucRTUBuf+start_buffer_index+MODBUS_ADU_DEVICE_PDU_BEGIN_SHIFT, frame_length-3);
	uint16_t *frame_crc = (uint16_t *)(ucRTUBuf+start_buffer_index+frame_length-2);
	
	// check CRC
	if (crc != *frame_crc)
		return FALSE;
	
	// add frame process task
	sendMessage(MSG_MODBUS_PROCESS_FRAME, start_buffer_index);
	
	// set MODBUS_INTERFACE_FRAME_PROCESSING_MODE mode
	modbus_interface_data.currentMode = MODBUS_INTERFACE_FRAME_PROCESSING_MODE;
	
	modbusClearReadStructures();
	
	return TRUE;
}

uint8_t modbusProcessFrameHandler(msg_par parameter)
{
	uint8_t start_buffer_index = (uint8_t)parameter;
	uint8_t device_address = ucRTUBuf[start_buffer_index];
	uint8_t function_code = ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT];
	BOOL unknown_function = FALSE;
	
	// process frame
	if (function_code == MODBUS_FUNC_READ_HOLDING_REGISTERS)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerReadHoldingRegisters(start_buffer_index);
	else if (function_code == MODBUS_FUNC_READ_INPUT_REGISTERS)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerReadInputRegisters(start_buffer_index);
	else if (function_code == MODBUS_FUNC_WRITE_SINGLE_REGISTER)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerWriteSingleRegister(start_buffer_index);
	else if (function_code == MODBUS_FUNC_DIAGNOSTICS)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerDiagnostics(start_buffer_index);
	else if (function_code == MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerWriteMultipleRegisters(start_buffer_index);
	else if (function_code == MODBUS_FUNC_REPORT_SLAVE_ID)
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerReportSlaveID(start_buffer_index);
	else if (function_code == MODBUS_FUNC_UTILITE && ucRTUBuf[start_buffer_index+MODBUS_ADU_DEVICE_FUNCTION_SUBCODE_SHIFT] == MODBUS_FUNC_UTILITE_SUBCODE_READ_DEVICE_IDENTIFICATION)// check for second byte
		modbus_interface_data.current_write_mode_buffer_length = modbusHandlerReadDeviceIdentification(start_buffer_index);
	else
	{
		// report unknown function
		unknown_function = TRUE;
	}
	
	// check for broadband address and do not send answer
	if (device_address == MODBUS_INTERFACE_BROADBAND_DEVICE_NUMBER)
	{
		// do not send, switch to read mode
		modbus_interface_data.currentMode = MODBUS_INTERFACE_READ_MODE;
		USART_SetRS485Mode(UART_RS485_MODE_RECEIVE);
		return 0;
	}
	
	if (modbus_interface_data.current_write_mode_buffer_length <= 0)
	{
		// report error
		ctrlReportErrorWithParameter(MODBUS_INTERFACE_FRAME_PROCESSING_ERROR, function_code);
		
		// modbus report exception
		uint8_t modbus_exception_code = MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE;
		
		if (unknown_function)
			modbus_exception_code = MODBUS_EXCEPTION_CODE_ILLEGAL_FUNCTION;
		
		modbus_interface_data.current_write_mode_buffer_length = modbusResponseException(start_buffer_index, modbus_exception_code);
	}	
	
	// set write mode and write size
	modbus_interface_data.currentMode = MODBUS_INTERFACE_WRITE_MODE;
	modbus_interface_data.current_write_mode_buffer_index = 0;
	
	// set device address	
	ucRTUBuf[MODBUS_ADU_DEVICE_ADDRESS_SHIFT] = device_address;
	
	// calculate CRC
	uint16_t *frame_crc = (uint16_t *)(ucRTUBuf+modbus_interface_data.current_write_mode_buffer_length);
	*frame_crc = usMBCRC16(ucRTUBuf+MODBUS_ADU_DEVICE_PDU_BEGIN_SHIFT, modbus_interface_data.current_write_mode_buffer_length-MODBUS_ADU_DEVICE_PDU_BEGIN_SHIFT);
	
	modbus_interface_data.current_write_mode_buffer_length += 2;
		
	// set UART to transmit mode
	// transmission should start automatic by Data Register empty Interrupt
	USART_SetRS485Mode(UART_RS485_MODE_TRANSMIT);

	return 0;
}

