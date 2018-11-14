/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:28:45
 */ 

#include "os.h"

iTimer    lTimer[maxTimers]; // список таймеров
iHandler  lHandler[maxHandlers]; // список обработчиков

iMessage  lMessage[maxMessages]; // буфер сообщений
uint16_t lMesPointer=0, hMesPointer=0; // указатели на начало и конец буфера

// установка обработчика события
// вызывается: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd) {
  uint8_t i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    if (lHandler[i].msg==0) { // ищем свободное место 
      lHandler[i].hnd = hnd; // и регистрирем обработчик
      lHandler[i].msg = msg;
      break;
    }
    i++;
  }
}

// снятие обработчика события
// вызывается: killHandler(MSG_KEY_PRESS, &checkKey);
void killHandler(msg_num msg, handler hnd) {
  uint8_t i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    
    if ((lHandler[i].msg==msg) && (lHandler[i].hnd==hnd)) {
      lHandler[i].msg = 0; // если нашли нужный, очищаем
    }
    
    if (lHandler[i].msg != 0) {
      if (i != j) { // сдвигаем все записи к началу списка, чтобы дырок не было
        lHandler[j].msg = lHandler[i].msg;
        lHandler[j].hnd = lHandler[i].hnd;
        lHandler[i].msg = 0;
      }
      j++;
    }
    i++;
  }
}

// занести событие в очередь
// пример вызова: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par) {
  hMesPointer = (hMesPointer+1) & (maxMessages-1); // сдвигаем указатель головы
  
  lMessage[hMesPointer].msg = msg; // заносим событие и параметр
  lMessage[hMesPointer].par = par;
  if (hMesPointer == lMesPointer) { // догнали начало очереди, убиваем необработанное сообытие
    lMesPointer = (lMesPointer+1) & (maxMessages-1);
  }
};

// обработка событий
void dispatchMessage() {
  int8_t i;
  uint8_t res;
  
  if (hMesPointer == lMesPointer) { // если пустая очередь - возврат
    return;
  }
  
  lMesPointer = (lMesPointer+1) & (maxMessages-1); // сдвинем указатель
  
  msg_num msg = lMessage[lMesPointer].msg;
  msg_par par = lMessage[lMesPointer].par;
  
  if (msg==0)
    return;
  
  for(i=maxHandlers-1; i>=0; i--) { // просматриваем обработчики с конца
    if (lHandler[i].msg==msg) { // последний занесенный имеет приоритет
      res = lHandler[i].hnd(par); // вызываем обработчик
      if (res) { // если боработчик вернул 1, перываем обработку события
        break;
      }
    }
  }
}

// установить таймер
// пример вызова: setTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time) {
  uint8_t i,firstFree;
  firstFree = maxTimers+1;
  if (time == 0) {
    sendMessage(msg, par);
  } else {
  
    for (i=0; i<maxTimers; i++) { // ищем установленный таймер
      if (lTimer[i].msg == 0) {
        firstFree = i;
      } else { // если нашли - обновляем время
        if ((lTimer[i].msg == msg) && (lTimer[i].par == par)) {
          lTimer[i].time = time;
          return;
        }  
      }
    }
    if (firstFree <= maxTimers) { // иначе - просто добавляем новый
      lTimer[firstFree].msg = msg;
      lTimer[firstFree].par = par;
      lTimer[firstFree].time = time;
    }
  }
}

// убить таймер
// особенность - убивает все установленные таймеры на данное событие, 
// не зависимо от параметра события
void killTimer(msg_num msg) {
  uint8_t i;
  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == msg) {
      lTimer[i].msg = 0;
    }
  }
}

// диспетчер таймеров
// Generate timer messages (jobs)
void dispatchTimer() {
  uint8_t i;
  msg_num msg;
  msg_par par;
  
  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == 0)
      continue;
    
    if (lTimer[i].time == 0) { // если пришло время
      msg = lTimer[i].msg;
      par =lTimer[i].par;
      lTimer[i].msg = 0;
      sendMessage(msg, par); // создаем событие
    } else {
      lTimer[i].time--; // иначе просто уменьшаем время
    }
  }
}
