/* 
 * PLC firmware is available under the MIT License. See http://opensource.org/licenses/MIT for full text.
 *
 * Copyright (C) Dmitry Ognyannikov, 2011
 * Created: 29.10.2011 3:23:03
 */ 

/* ----------------------- System includes ----------------------------------*/
#include "os.h"

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT* usRegInputBuf = (USHORT*)&modbus_variables;// switch usRegInputBuf to modbus_variables

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


void InitModbusInterface()
{
	// fill modbus_variables with zero
	uint8_t *pointer = (uint8_t *)&modbus_variables;
	for (int i=0; i<sizeof(modbus_variables); i++)
		pointer[i] = 0;
	
	//TODO: set defaults to variables	
	//TODO: read settings from storage
	
	// init modbus
	eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x0A, 0, BAUD, MB_PAR_EVEN );
	
	/* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable();
}

// 50 ms MODBUS timer call
uint8_t ModbusTimerCycle(msg_par message_parameter)
{
	pxMBPortCBTimerExpired();
	
	return 0;
}