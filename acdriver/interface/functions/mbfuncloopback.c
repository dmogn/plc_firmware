/*
 * mbfuncloopback.c
 *
 * Created: 01.11.2011 6:58:15
 *  Author: Dmitry Ognyannikov
 */ 

#include "os.h"

eMBException
eMBFuncLoopback( UCHAR * pucFrame, USHORT * usLen )
{
	// Do nothing, sent frame data back
	return MB_EX_NONE;
}