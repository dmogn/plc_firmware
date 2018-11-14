/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 0:02:38
 */ 

#include "os.h"

int main() {
  init();
  
  for(;;) {
	// process task handlers
    dispatchMessage();
	
	// Motor Control calculations
	UpdatePWMRates();
	
	// process modbus events
	//eMBPoll();
	
	// reset WatchDog Timer
	wdt_reset();
	
	// sleep upon next timer tick
	//sleep(); //TODO: only in IDLE mode
  }
}



void init() {
	InitPins();
	
	MemoryStorageInit();
	
	//Init Timers
	InitScheduleTimer();
	InitPWMTimer();
	//InitASS_Timer(); // 32768 Gz - disabled by default
	
  // регистрация обработчиков событий
  // set timers and handlers  
  setHandler(MSG_ADC_CYCLE, &ADCUpdateCycle);
  setHandler(MSG_MODBUS_PROCESS_FRAME, &modbusProcessFrameHandler);

  // set MODBUS 50ms timer - call ( void )pxMBPortCBTimerExpired(  );
  setHandler(MSG_MODBUS_TIMER, &ModbusTimerCycle);
  setTimer(MSG_MODBUS_TIMER, 1, 50);
  
  // set controller update timer (1 ms)
  setHandler(MSG_CONTROLLER_TIMER, &ctrlUpdateStep);
  setTimer(MSG_CONTROLLER_TIMER, 0, 1);

  // 100ms - update timer
  setHandler(MSG_LED_TIMER, &LEDUpdateCycle); 
  setTimer(MSG_LED_TIMER, 0, 100); 
	
	//Init ADC
	ADCInit(ADC_REFERENCE_INTERNAL);// with internal reference source (ATMega32=2.56V)
	
	//Init Modbus interface
	InitModbusInterface();
	
	initController();
	
	InitAngleInput();
	
	InitDriver();
	
	//InitUSART
	USART_Init(9600);
	USART_SetDefaultRate();	
	USART_EnableInterrupts();
	
	//Init Watch Dog Timer
	//wdt_enable(WDTO_1S); // disabled by default
}
