/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:28:35
 */ 

#include "os.h"

uint8_t adc_last_results[CHANNELS_COUNT];

	// ADC Init - Инициализурем АЦП.
void ADCInit(unsigned char ucReferenceVoltageMode) {
	// Итак что тут у нас:
	// ADEN = 1 - разрешаем АЦП
	// ADIE = 1Разрешаем прерывания. 
	// ADSC = 1 Запускаем преобразование (первое, дальше автоматом)
	// ADATE = 1 Непрерывные последовательные преобразования, одно за другим.
	// ADPS2..0 = 3 Делитель частоты на 8 - так у меня получается оптимальная частота. 
//OUTI	ADCSRA,(1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<ADATE)|(3<<ADPS0) 
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<ADATE)|(3<<ADPS0);
	
	//А тут выбираем откуда брать будем сигнал
	//REFS -- 0b[01]100101 первые два бита - напряжение с входа AVCC
    //ADLAR --0b01[1]00101следующий бит =1 - выравнивание по левому краю
	//MUX -- 0b011[00000]Сигнал на вход идет с нулевого канала АЦП.
	//ADMUX = 0b01100000

	// ADLAR = 1 - Left adjustment, read 8 bit result from ADCH
	ADMUX = (ucReferenceVoltageMode<<REFS0) | (1<<ADLAR) | ADC0_CHANNEL; 

	// init adc_last_results
	for (int i = 0; i<CHANNELS_COUNT; i++)
		adc_last_results[i] = 0;
}

void setChannel(unsigned char ucChannel)
{
	ADMUX = (ADMUX & 0b11100000) | ucChannel;
}

unsigned char readChannel()
{
	return ADCH;
}

/* ADC Conversion Complete */
ISR(ADC_vect)
{
	unsigned char channel = ADMUX & 0b00011111;

	if (channel < CHANNELS_COUNT)
	{
		adc_last_results[channel] = ADCH;
	}

	ADMUX = (ADMUX & 0b11100000)// keep ADMUX settings
        | ((channel+1)%8); // choose next channel 0-7
		
	// add message 
	sendMessage(MSG_ADC_CYCLE, 1);
}

// Should be called after ADC input sample processed by hardware
uint8_t ADCUpdateCycle(msg_par message_parameter)
{
	//UpdatePWNChannelsCurrentInputValues();
	UpdateAngleInputValues();
	UpdateTemperatureInputValues();
	
	return 1;
}

uint8_t ADCGetChannelValue(uint8_t channel_number)
{
	return adc_last_results[channel_number];
}