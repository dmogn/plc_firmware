/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 29.10.2011 3:23:48
*/


#ifndef DRIVER_H_
#define DRIVER_H_


typedef enum
{
	DRIVER_SERVO_IDLE = 0,
	DRIVER_SERVO_KEEP_POSITION,
	DRIVER_SERVO_CONTINUOUS_SPEED,
	DRIVER_SERVO_SPEED_AND_ACCELERATION,
	DRIVER_SERVO_SPEED_AND_ACCELERATION_TO_END_SPEED,
	DRIVER_SERVO_SPEED_TO_END_POSITION,
	DRIVER_SERVO_CONTINUOUS_FORCE,
	DRIVER_STEP_DIRECT
} driverMode;

typedef enum
{
	DRIVER_HARDWARE_NO_ERRORS = 100,
	DRIVER_HARDWARE_MOTOR_TEMPERATURE_OVERHEAT,
	DRIVER_HARDWARE_POWER_LINE_OVERCURRENCY,
	DRIVER_HARDWARE_ANGLE_OVERSPEED,
	DRIVER_HARDWARE_ROTOR_BREAKED,
	/** Plan angle speed overspeed maximum motor speed */ 
	DRIVER_SOFRWARE_COMMAND_ANGLE_OVERSPEED,
	DRIVER_SOFRWARE_COMMAND_ANGLE_ACCELERATION_OPPOSITE_TO_ENDSPEED
} driverHardwareErrorType;

typedef struct
{
	/** Step is calculation unit of angle measure */
	uint16_t steps_per_revolution;
	/** Rotor Inertia in rotor speed change (in steps per seconds) per torque unit */
	uint16_t rotor_intertia;
	/** Moment elasticity - speed acceleration for angle error fix, in 0.1 % of speed error */
	uint16_t moment_elasticity;
	
#ifdef ENABLE_AC_INDUCTION_SERVO
#endif

#ifdef ENABLE_AC_INDUCTION_SIMPLE
	/** skolzenie */
	uint16_t default_s_in_angle_steps;
#endif

#ifdef ENABLE_AC_SYNCHRONOUS_SERVO
#endif

#ifdef ENABLE_STEP_SERVO
#endif
} driverCalculationConstants;

/** Data for servo driver state monitoring throught Modbus input registers */
typedef struct 																
{
	/** rotor angle speed in steps per second */
	int16_t rotor_angle_speed;
	/** rotor angle acceleration in steps per second */
	int16_t rotor_angle_acceleration;
	/** rotor angle position in steps */
	int32_t rotor_absolute_angle_position;
	/** rotor calculated (planned for execution) current angle speed in steps per second */
	int16_t rotor_planned_angle_speed;
	/** rotor calculated (planned for execution) current torque. Not applicable for step motors. */
	int16_t rotor_planned_angle_torque;
	/** Motor temperature (in Celsius) */
	int16_t motor_temperature;
} driverDataInput;

/** Data for servo driver state and control throught Modbus holding registers */
typedef struct 
{
	/** Motor control driver mode */
	uint16_t mode;
	/** Hardware error code */
	uint16_t hardware_error_code;
	/** Binary flags, also called "coils" */
	uint16_t flags;
	/** Start speed in acceleration mode in steps per second */
	int16_t rotor_angle_start_speed;
	/** acceleration start time (System Time) */
	int32_t rotor_angle_start_speed_time;
	/** acceleration in steps per second */
	int16_t rotor_angle_acceleration_speed;
	/** End continuous speed in acceleration mode and continuous speed in continuous speed mode in steps per second */
	int16_t rotor_angle_end_continuous_speed;
	/** Absolute angle stop position */
	int32_t rotor_angle_stop_position;
	int16_t rotor_angle_force;
	/** Make direct step (if value = +1 or -1) and set value to 0 */
	int16_t rotor_angle_direct_step;
	/** Maximum allowed motor temperature (in Celsius) */
	int16_t motor_maximum_allowed_temperature;
	/** Maximum allowed motor total current in milliamperes */
	int16_t motor_maximum_allowed_current;
	/** Maximum allowed rotor angle speed in steps per second */
	int16_t motor_maximum_allowed_angle_speed;
	/** Step motor microstep scaler, 1 = full step, 2 = halfstep, 4 = 1/4 step. */
	uint16_t motor_step_scale;
	
	//TODO: absolute position constraints
} driverData;

void InitDriver(void);

/** Disable all device-related activity. Could be used for emergency stop */
void StopAllDeviceActivity(void);

void UpdatePWMRates(void);

void CheckDriverModbusParameters(void);

/** Calculate real time position, position error and speed to drive step motor (position-oriented mode) */
void ServoCalculatePositionAndSpeed(void);

/** Calculate real time torque to drive servo motor (speed-oriented mode) */
void ServoCalculateSpeedAndTorque(void);

#endif /* DRIVER_H_ */