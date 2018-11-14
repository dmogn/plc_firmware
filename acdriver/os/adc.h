/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 19.06.2011 19:28:16
 */ 


#ifndef ADC_H_
#define ADC_H_

// A2D voltage reference select
#define ADC_REFERENCE_AREF		0x00	///< 0x00 -> AREF pin, internal VREF turned off
#define ADC_REFERENCE_AVCC		0x01	///< 0x01 -> AVCC pin,  with external capacitor at AREF pin
#define ADC_REFERENCE_RSVD		0x02	///< 0x02 -> Reserved.   ATMega640=1.1
#define ADC_REFERENCE_INTERNAL	0x03	///< 0x03 -> Internal VREF ATMega8=2.56V, ATMega168=1.1V, ATMega32=2.56V, ATMega640=2.56V Voltage Reference with external capacitor at AREF pin

// default value
#ifndef ADC_DEFAULT_REFERENCE
#define ADC_DEFAULT_REFERENCE			ADC_REFERENCE_AVCC
#endif

#ifdef ADATE
	#define ADFR	ADATE
#endif

// channel select
#define ADC0_CHANNEL 0b00000 
#define ADC1_CHANNEL 0b00001  
#define ADC2_CHANNEL 0b00010  
#define ADC3_CHANNEL 0b00011  
#define ADC4_CHANNEL 0b00100  
#define ADC5_CHANNEL 0b00101  
#define ADC6_CHANNEL 0b00110  
#define ADC7_CHANNEL 0b00111  
#define REFERENCE_112_CHANNEL 0b11110  
#define GROUND_CHANNEL 0b11111  

#define CHANNELS_COUNT 8

/**
* ucReferenceVoltageMode - must be ADC_REFERENCE_AREF or ADC_REFERENCE_AVCC or ADC_REFERENCE_INTERNAL
*/
void ADCInit(unsigned char ucReferenceVoltageMode);

uint8_t ADCUpdateCycle(msg_par message_parameter);

uint8_t ADCGetChannelValue(uint8_t channel_number);

#endif /* ADC_H_ */
