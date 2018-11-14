/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:35:17
 */ 

#include "os.h"

#ifdef ENABLE_QUARTZ_TUMER

UpTimeCounter global_up_time_counter;

// Asynchronous Timer2 interrupt
ISR(TIMER2_OVF_vect)
{
	global_up_time_counter.tick_counts++;

	//TODO: call timer tasks
}


//============================================================================
// Init Timer2 as asynchronous real time timer (for 32768 Gz external quartz)
//============================================================================
void InitASS_Timer(void)
{
	// clear timer structures
	global_up_time_counter.tick_counts = 0;
	
	// enable hardware
//if(ASSR & (1<<AS2))	//Если это второй вход то
	//{	
	//if (ASSR & (1<<TCN2UB | 1<<OCR2UB | TCR2UB) ) 	// проверяем есть ли хоть один бит флаговый
	//{
		//SetTask(InitASS_Timer);						// Если есть, то отправляем на повторный цикл ожидания
	//}
	//else											// Если все чисто, то можно запускать прерывания
	//{
		//TIFR  |= 1<<OCF2 | 1<< TOV2;				// Сбрасываем флаги прерываний, на всякий случай.
		//TIMSK |= 1<< TOIE2;							// Разрешаем прерывание по переполнению
		//return;
	//}
//
	//}
//
	//TIMSK &=~(1<<OCIE2 | 1<< TOIE2);		// Запрещаем прерывания таймера 2
	//ASSR  = 1<<AS2;							// Включаем асинхронный режим
	//TCNT2 = 0;
	//TCCR2 = 5<<CS20; 						// Предделитель на 128 на 32768 даст 256 тиков в секунду
											//// Что даст 1 прерывание в секунду.				
	//SetTask(InitASS_Timer);					// Прогоняем через диспетчер, чтобы зайти снова. 
}

#endif