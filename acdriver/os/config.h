/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 05.11.2011 19:01:55
 *
 * Project configuration file.
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

// CPU Clock Config (XTAL)
#define F_CPU 8000000L 

// timer ticks per seconds (Timer clock = system clock/64)
#define F_SYSTEM_TIMER_TICKS 125000


// UART Config
#define BAUD 500000
#define UART_BAUD_RATE BAUD
#define USE_2X

// asynchronous real time timer (for 32768 Gz external quartz on timer 2)
//#define ENABLE_QUARTZ_TUMER

// Modbus interface related constants

#define MODBUS_INTERFACE_DEFAULT_DEVICE_NUMBER 10
//#define MODBUS_INTERFACE_BUFFER_SIZE 10 

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

// Controller related constants

#define CONTROLLER_LOG_SIZE 10

#define CONTROLLER_TERMPERATURE_CONTROL_ENABLED
#define DEFAULT_MAXIMUM_RADIATOR_TEMPERATURE 100
#define DEFAULT_RADIATOR_TEMPERATURE 0


// motor type
//#define ENABLE_AC_INDUCTION_SERVO
#define ENABLE_AC_INDUCTION_SIMPLE
//#define ENABLE_AC_SYNCHRONOUS_SERVO
//#define ENABLE_STEP_SERVO

//#define STEP_MOTOR_MODE - obsolete
//#define SERVO_MOTOR_MODE - obsolete

// Common motor settings

// Current in milliamperes per ADC unit of measurement
#define MOTOR_PHASE_CURRENT_PER_ADC_UNIT 50

#define ENABLE_CONTROLLER_TERMOSENSOR
#define ENABLE_MOTOR_TERMOSENSOR

/** Maximum allowed motor temperature (in Celsius) */
#define DEFAULT_MAXIMUM_ALLOWED_MOTOR_TEMPERATURE 80
/** Maximum allowed motor total current in milliamperes */
#define DEFAULT_MAXIMUM_ALLOWED_MOTOR_CURRENT 500
/** Maximum allowed rotor angle speed in steps per second */
#define DEFAULT_MAXIMUM_ALLOWED_ANGLE_SPEED 5000

#define DEFAULT_MOTOR_TEMPERATURE 0

// Induction motor related settings

#define INDUCTION_MOTOR_PHASE_COUNT 1

// Step motor related settings 

// default work phase current in milliamperes
#define STEP_MOTOR_DEFAULT_PHASE_CURRENT 2000

// keep position phase current in milliamperes
#define STEP_MOTOR_KEEP_POSITION_PHASE_CURRENT 500

// PWM frequency settings, 8 = 25 KHz
#define STEP_MOTOR_PWM_PRESCALER 8

#define STEP_MOTOR_DEFAULT_FULL_LOAD_PWM_DURATION 2

#define STEP_MOTOR_DEFAULT_KEEP_POSITION_PWM_DURATION 1

// overload time while full step start in system ticks 
#define STEP_MOTOR_MAX_STEP_ACCELERATION_OVERLOAD_TIME 8

#endif /* CONFIG_H_ */