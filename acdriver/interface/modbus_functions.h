/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 07.01.2012 1:50:46
 *
 * Custom embedded hardware-optimized fully-asynchronous slave-mode Modbus protocol implementation with hight noise immunity.
 */ 


#ifndef MODBUS_FUNCTIONS_H_
#define MODBUS_FUNCTIONS_H_

#define MODBUS_ZERO_RESPONSE 0

uint16_t modbusResponseException(uint8_t start_buffer_index, uint8_t exception_code);

uint16_t modbusHandlerReadHoldingRegisters(uint8_t start_buffer_index);
uint16_t modbusHandlerReadInputRegisters(uint8_t start_buffer_index);
uint16_t modbusHandlerWriteSingleRegister(uint8_t start_buffer_index);
uint16_t modbusHandlerDiagnostics(uint8_t start_buffer_index);
uint16_t modbusHandlerWriteMultipleRegisters(uint8_t start_buffer_index);
uint16_t modbusHandlerReportSlaveID(uint8_t start_buffer_index);
uint16_t modbusHandlerReadDeviceIdentification(uint8_t start_buffer_index);

#endif /* MODBUS_FUNCTIONS_H_ */