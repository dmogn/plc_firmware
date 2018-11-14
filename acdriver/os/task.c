/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 22:28:45
 */ 

#include "os.h"

iTimer    lTimer[maxTimers]; // ������ ��������
iHandler  lHandler[maxHandlers]; // ������ ������������

iMessage  lMessage[maxMessages]; // ����� ���������
uint16_t lMesPointer=0, hMesPointer=0; // ��������� �� ������ � ����� ������

// ��������� ����������� �������
// ����������: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd) {
  uint8_t i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    if (lHandler[i].msg==0) { // ���� ��������� ����� 
      lHandler[i].hnd = hnd; // � ����������� ����������
      lHandler[i].msg = msg;
      break;
    }
    i++;
  }
}

// ������ ����������� �������
// ����������: killHandler(MSG_KEY_PRESS, &checkKey);
void killHandler(msg_num msg, handler hnd) {
  uint8_t i,j;
  i=0; j=0;
  while (i<maxHandlers) {
    
    if ((lHandler[i].msg==msg) && (lHandler[i].hnd==hnd)) {
      lHandler[i].msg = 0; // ���� ����� ������, �������
    }
    
    if (lHandler[i].msg != 0) {
      if (i != j) { // �������� ��� ������ � ������ ������, ����� ����� �� ����
        lHandler[j].msg = lHandler[i].msg;
        lHandler[j].hnd = lHandler[i].hnd;
        lHandler[i].msg = 0;
      }
      j++;
    }
    i++;
  }
}

// ������� ������� � �������
// ������ ������: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par) {
  hMesPointer = (hMesPointer+1) & (maxMessages-1); // �������� ��������� ������
  
  lMessage[hMesPointer].msg = msg; // ������� ������� � ��������
  lMessage[hMesPointer].par = par;
  if (hMesPointer == lMesPointer) { // ������� ������ �������, ������� �������������� ��������
    lMesPointer = (lMesPointer+1) & (maxMessages-1);
  }
};

// ��������� �������
void dispatchMessage() {
  int8_t i;
  uint8_t res;
  
  if (hMesPointer == lMesPointer) { // ���� ������ ������� - �������
    return;
  }
  
  lMesPointer = (lMesPointer+1) & (maxMessages-1); // ������� ���������
  
  msg_num msg = lMessage[lMesPointer].msg;
  msg_par par = lMessage[lMesPointer].par;
  
  if (msg==0)
    return;
  
  for(i=maxHandlers-1; i>=0; i--) { // ������������� ����������� � �����
    if (lHandler[i].msg==msg) { // ��������� ���������� ����� ���������
      res = lHandler[i].hnd(par); // �������� ����������
      if (res) { // ���� ���������� ������ 1, �������� ��������� �������
        break;
      }
    }
  }
}

// ���������� ������
// ������ ������: setTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time) {
  uint8_t i,firstFree;
  firstFree = maxTimers+1;
  if (time == 0) {
    sendMessage(msg, par);
  } else {
  
    for (i=0; i<maxTimers; i++) { // ���� ������������� ������
      if (lTimer[i].msg == 0) {
        firstFree = i;
      } else { // ���� ����� - ��������� �����
        if ((lTimer[i].msg == msg) && (lTimer[i].par == par)) {
          lTimer[i].time = time;
          return;
        }  
      }
    }
    if (firstFree <= maxTimers) { // ����� - ������ ��������� �����
      lTimer[firstFree].msg = msg;
      lTimer[firstFree].par = par;
      lTimer[firstFree].time = time;
    }
  }
}

// ����� ������
// ����������� - ������� ��� ������������� ������� �� ������ �������, 
// �� �������� �� ��������� �������
void killTimer(msg_num msg) {
  uint8_t i;
  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == msg) {
      lTimer[i].msg = 0;
    }
  }
}

// ��������� ��������
// Generate timer messages (jobs)
void dispatchTimer() {
  uint8_t i;
  msg_num msg;
  msg_par par;
  
  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == 0)
      continue;
    
    if (lTimer[i].time == 0) { // ���� ������ �����
      msg = lTimer[i].msg;
      par =lTimer[i].par;
      lTimer[i].msg = 0;
      sendMessage(msg, par); // ������� �������
    } else {
      lTimer[i].time--; // ����� ������ ��������� �����
    }
  }
}
