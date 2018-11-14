/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 14:10:40
 */ 


#ifndef OS_H_
#define OS_H_

#include "config.h"

#include <inttypes.h>
#include <stdint.h>
#include <avr/common.h>
#include <avr/pgmspace.h>
// microcontroller resister and port definitions
#include <avr/io.h>
// microcontroller hardware interrupts
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/portpins.h>
#include <avr/power.h>
#include <avr/sleep.h>
// Watch Dog Timer
#include <avr/wdt.h>
#include <inttypes.h>
#include <util/setbaud.h>

#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END    SREG = _sreg

#define INTERRUPTABLE_SECTION_START	unsigned char _sreg = SREG; sei()
#define INTERRUPTABLE_SECTION_END    SREG = _sreg

#define	INLINE inline

// --------------- avrlibdefs.h ------------------------
// Code compatibility to new AVR-libc
// outb(), inb(), inw(), outw(), BV(), sbi(), cbi(), sei(), cli()
#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif
#ifndef inb
	#define	inb(addr)			(addr)
#endif
#ifndef outw
	#define	outw(addr, data)	addr = (data)
#endif
#ifndef inw
	#define	inw(addr)			(addr)
#endif

#ifndef BV
	#define BV(bit)			(1<<(bit))
#endif
#ifndef cbi
	#define cbi(reg,bit)	reg &= ~(BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)	reg |= (BV(bit))
#endif
#ifndef cli
	#define cli()			__asm__ __volatile__ ("cli" ::)
#endif
#ifndef sei
	#define sei()			__asm__ __volatile__ ("sei" ::)
#endif
#ifndef nop
	#define nop()			__asm__ __volatile__ ("nop" ::)
#endif
#ifndef sleep
	#define sleep()			__asm__ __volatile__ ("sleep" ::)
#endif

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

// Clamp a value to be in a given range
#define CLAMP(val, min, max) MAX(MIN(val,max),min)

// maximum value that can be held
// by unsigned data types (8,16,32bits)
#define MAX_U08	255U
#define MAX_U16	65535UL
#define MAX_U32	4294967295UL

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_S08	-128
#define MAX_S08	127
#define MIN_S16	-32768
#define MAX_S16	32767
#define MIN_S32	-2147483648
#define MAX_S32	2147483647

#define ENTER_CRITICAL_SECTION( )   cli()
#define EXIT_CRITICAL_SECTION( )    sei()

#define assert( x )

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

/* ----------------------- OS & Hardware includes ----------------------------------*/
#include "task.h"
#include "adc.h"
#include "gpio.h"
#include "hal.h"
#include "hwtimer.h"
#include "memorystorage.h"
#include "pin.h"
#include "pwm.h"
#include "realtimer.h"
#include "uart.h"
#include "watchdog.h"


/* ----------------------- Modbus interface includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"

#if MB_RTU_ENABLED == 1
#include "../interface/rtu/mbcrc.h"
#include "../interface/rtu/mbrtu.h"
#endif

#include "../interface/interface.h"
#include "../interface/modbus.h"
#include "../interface/modbus_functions.h"

/* ----------------------- controller includes ----------------------------------*/
#include "../controller/controller.h"
#include "../controller/commandflow.h"
#include "../controller/interfaceled.h"
#include "../controller/time.h"

/* ----------------------- motor control driver includes ----------------------------------*/
#include "../driver/driver.h"

/* ----------------------- sensor input includes ----------------------------------*/
#include "../sensorinput/angleposition.h"
#include "../sensorinput/temperature.h"
#include "../sensorinput/pwmchannelcurrent.h"

typedef struct {
	controllerData controller_data;
	driverCalculationConstants driver_calculation_constants;
	driverData driver_data;
	driverDataInput driver_data_input;
	controllerLogReport system_log[CONTROLLER_LOG_SIZE];
} ModbusVariables;

extern volatile ModbusVariables modbus_variables;

#endif /* OS_H_ */
