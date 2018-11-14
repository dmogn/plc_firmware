/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:28:22
 */ 


#ifndef TASK_H_
#define TASK_H_

#define maxHandlers 8
#define maxMessages 8
#define maxTimers  8

#define MSG_ADC_CYCLE 1
#define MSG_MODBUS_TIMER 2
#define MSG_CONTROLLER_TIMER 3
#define MSG_LED_TIMER 4
#define MSG_MODBUS_PROCESS_FRAME 5

typedef uint8_t msg_num; // тип сообытия - мне пока хватает одного байта
typedef int16_t msg_par; // тип параметра события
typedef uint8_t (*handler)(msg_par); // описание функции-обработчика

// структура записи из списка обработчиков
typedef struct{
  msg_num msg; // обрабатываемое событие
  handler hnd; // собственно сам обработчик
} iHandler;

// структура события из буфера событий
typedef struct{
  msg_num msg; // номер события
  msg_par par; // параметр
} iMessage;

// структура таймера
typedef struct{
  msg_num msg; // номер генерируемого сообщения
  msg_par par; // его параметр
  uint16_t  time; // таймер в условных тиках (сейчас 10 мсек)
} iTimer;

void init();

// обработка событий
// process messages (jobs)
void dispatchMessage();

// установка обработчика события
// вызывается: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd);

// снятие обработчика события
// вызывается: killHandler(MSG_KEY_PRESS, &checkKey);
void killHandler(msg_num msg, handler hnd);

// занести событие в очередь
// пример вызова: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par);

// установить таймер
// пример вызова: setTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time);

// убить таймер
// особенность - убивает все установленные таймеры на данное событие, 
// не зависимо от параметра события
void killTimer(msg_num msg);

// диспетчер таймеров
// Generate timer messages (jobs)
void dispatchTimer();


#endif /* TASK_H_ */
