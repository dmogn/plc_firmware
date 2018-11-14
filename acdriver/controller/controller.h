/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 01.11.2011 9:20:59
*/


#ifndef CONTROLLER_H_
#define CONTROLLER_H_


typedef enum
{
    CTRLR_MODE_IDLE = 0,                /* Idle (minimal power consumption, all power circuits disabled). Idle is default mode. */
    CTRLR_MODE_EMERGENCY_STOP,                /* Emergency stop (brake and stop all motion, disable power circuits). Caused by master command. The mode can be leaved only with reset. */
	CTRLR_MODE_STOP_KEEP_STATE,                /* Stop and keep state (force keep current state or servodrive position, wait for commands). Default mode after mode DIRECT_CONTROL (4) and NC_COMMANDS_FLOW (5). */
	CTRLR_MODE_DIRECT_CONTROL,                /* Direct control (direct realtime control with change force, speed, etc. control parameters). General register and coil read/write commands are used. Switch to mode STOP_KEEP_STATE (3) with timeout without any register read or write commands. */
	CTRLR_MODE_NC_COMMANDS_FLOW,                /* NC commands flow execution. Switch to mode STOP_KEEP_STATE (3) after command queue execution end. */
	CTRLR_MODE_RESET,                /* Reset (stop all motion, disable power circuits, hardware reset). */
	CTRLR_MODE_HARDWARE_ERROR,                /* Hardware error (mechanical faults, etc.). The mode can be leaved only with reset. */
	CTRLR_MODE_HEAT_MOTOR                /* Heat motor (heat motor with 1% of nominal power) for prevention of wet condensation and corrosion inside motor and driver. */
} cntrMode;

typedef enum
{
	CTRLR_NO_ERRORS = 0,
	CTRLR_OTHER_ERROR,
	CTRLR_HARDWARE_SOFTWARE_FAILURE_DETECTED,
	CTRLR_HARDWARE_FIRMWARE_CHECKSUM_FAILURE,
	CTRLR_HARDWARE_RADIATOR_TEMPERATURE_OVERHEAT,
	CTRLR_HARDWARE_POWER_LINE_OVERVOLTAGE,
	CTRLR_HARDWARE_LOW_VOLTAGE_LINE_UNDERVOLTAGE,
	CTRLR_HARDWARE_DIRECT_CONTROL_INTERFACE_TIMEOUT,
	CTRLR_HARDWARE_DRIVER_SPECIFIC_ERROR,
	CTRLR_SOFTWARE_ILLEGAL_ARGUMENT_DETECTED,
	PWM_SOFTWARE_CHANNEL_NUMBER_OUT_OF_RANGE,
	CTRLR_SOFTWARE_ILLEGAL_PIN_PORT_NUMER,
	MODBUS_INTERFACE_FRAME_PROCESSING_ERROR
} cntrHardwareErrorType;	

/** Data for controller state and control throught Modbus holding registers */
typedef struct
{
	/** Controller mode */
	uint16_t mode;
	/** Error code */
	uint16_t error_code;
	/** Current device time (millisecond resolution) = uptime */
	uint32_t device_time;
	/** Device time to network time shift (millisecond resolution) */
	int32_t network_time_shift;
	/** Maximum system step timeout, in milliseconds. The parameter for monitoring for logic errors */
	uint16_t maximum_system_step_timeout;
	/** Driver cooling radiator temperature (in Celsius) */
	int16_t radiator_temperature;
	/** Maximum allowed radiator temperature (in Celsius) */
	int16_t maximum_radiator_temperature;
} controllerData;

typedef struct
{
	// error type
	uint16_t error_code;
	// count of this error type reports
	uint16_t count;
	// system time of this error type last report
	uint32_t last_time;
	// custom parameter value of last report
	uint16_t last_parameter;
} controllerLogReport;
	 
void initController(void);

void ctrlSetMode(uint16_t newMode, uint16_t new_error_code);

/** Update controller parameters */
uint8_t ctrlUpdateStep(msg_par message_parameter);

/** Initialize log report data structures */
void ctrlInitLog(void);

/** Report error to log */
void ctrlReportError(uint16_t error_code);

/** Report error to log with parameter */
void ctrlReportErrorWithParameter(uint16_t error_code, uint16_t parameter);

#define ZERO_ERROR_LOG_PARAMETER 0

#endif /* CONTROLLER_H_ */