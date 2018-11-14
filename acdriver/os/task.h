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

typedef uint8_t msg_num; // ��� �������� - ��� ���� ������� ������ �����
typedef int16_t msg_par; // ��� ��������� �������
typedef uint8_t (*handler)(msg_par); // �������� �������-�����������

// ��������� ������ �� ������ ������������
typedef struct{
  msg_num msg; // �������������� �������
  handler hnd; // ���������� ��� ����������
} iHandler;

// ��������� ������� �� ������ �������
typedef struct{
  msg_num msg; // ����� �������
  msg_par par; // ��������
} iMessage;

// ��������� �������
typedef struct{
  msg_num msg; // ����� ������������� ���������
  msg_par par; // ��� ��������
  uint16_t  time; // ������ � �������� ����� (������ 10 ����)
} iTimer;

void init();

// ��������� �������
// process messages (jobs)
void dispatchMessage();

// ��������� ����������� �������
// ����������: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg, handler hnd);

// ������ ����������� �������
// ����������: killHandler(MSG_KEY_PRESS, &checkKey);
void killHandler(msg_num msg, handler hnd);

// ������� ������� � �������
// ������ ������: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg, msg_par par);

// ���������� ������
// ������ ������: setTimer(MSG_LCD_REFRESH, 0, 50);
void setTimer(msg_num msg, msg_par par, uint16_t time);

// ����� ������
// ����������� - ������� ��� ������������� ������� �� ������ �������, 
// �� �������� �� ��������� �������
void killTimer(msg_num msg);

// ��������� ��������
// Generate timer messages (jobs)
void dispatchTimer();


#endif /* TASK_H_ */
