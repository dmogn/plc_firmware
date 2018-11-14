/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 01.01.2012 23:59:57
 *
 * Custom embedded hardware-optimized fully-asynchronous slave-mode Modbus protocol implementation with hight noise immunity.
 */ 


#ifndef MODBUS_H_
#define MODBUS_H_

// Modbus functions:
#define MODBUS_FUNC_READ_HOLDING_REGISTERS	0x03 //Read Holding Registers
#define MODBUS_FUNC_READ_INPUT_REGISTERS	0x04 //Read Input Registers
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER	0x06 //Write Single Register
#define MODBUS_FUNC_DIAGNOSTICS	0x08 //Diagnostics
#define MODBUS_FUNC_WRITE_MULTIPLE_REGISTERS	0x10 //Write Multiple Registers
#define MODBUS_FUNC_REPORT_SLAVE_ID	0x11 //Report Slave ID
#define MODBUS_FUNC_UTILITE	0x2B  // + 0x0E Read Device Identification

#define MODBUS_FUNC_UTILITE_SUBCODE_READ_DEVICE_IDENTIFICATION 0x0E	// Read Device Identification

#define MODBUS_FUNC_EXCEPTION_SHIFT 0x80

// application data unit (ADU) structure
// application data unit (ADU) include device address, protocol data unit (PDU), checksum 
#define MODBUS_ADU_DEVICE_ADDRESS_SHIFT 0
#define MODBUS_ADU_DEVICE_FUNCTION_CODE_SHIFT 1
#define MODBUS_ADU_DEVICE_FUNCTION_SUBCODE_SHIFT 2
#define MODBUS_ADU_DEVICE_PDU_BEGIN_SHIFT 1

#define MODBUS_INTERFACE_BROADBAND_DEVICE_NUMBER 0
#define MODBUS_INTERFACE_FRAME_CHECK_HEADER_SIZE 3
#define MODBUS_INTERFACE_FRAME_CHECK_ARRAY_SIZE 8

// maximum UART timeout in milliseconds
#define MODBUS_INTERFACE_MAX_UART_TIMEOUT 100

// modbus exception codes
#define MODBUS_EXCEPTION_CODE_ILLEGAL_FUNCTION 1
#define MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS 2
#define MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_VALUE 3
#define MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE 4
#define MODBUS_EXCEPTION_CODE_ACKNOWLEDGE 5
#define MODBUS_EXCEPTION_CODE_MEMORY_PARITY_ERROR 8
#define MODBUS_EXCEPTION_CODE_GATEWAY_PATH_UNAVAILABLE 0xA
#define MODBUS_EXCEPTION_CODE_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND 0xB

// modbus registers
#define MODBUS_HOLDING_REGISTERS_SHIFT 0x1000
#define MODBUS_INPUT_REGISTERS_SHIFT 0x1000

#define MODBUS_MAX_IO_RESISTERS_COUNT 0x007D


void modbusInitInterface( void );

void modbusClearReadStructures( void );

void modbusUARTByteReceived(uint8_t receivedByte);

void modbusUARTTransmitterBufferEmpty( void );

void modbusUARTByteTransmitted( void );

uint8_t modbusCheckPacketHeader(uint8_t start_buffer_index);

uint8_t modbusGetPredictedFrameSize(uint8_t start_buffer_index);

uint8_t modbusGetFrameSize(uint8_t start_buffer_index);

uint8_t modbusProcessFrame(uint8_t start_buffer_index, uint16_t frame_length);

uint8_t modbusProcessFrameHandler(msg_par parameter);

#endif /* MODBUS_H_ */