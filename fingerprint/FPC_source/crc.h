// The information contained herein is confidential property of
// Fingerprint Cards AB. The use, copying, transfer or disclosure of such 
// information is prohibited exceptby express written agreement with 
// Fingerprint Cards AB.
//
//-----------------------------------------------------------------------------
// File: crc.h
// Description:	Functions for calculating a checksum for a block of
// memory.
//
// History:
// 2002-06-05 Mattias Hallberg Created.
//--------------------------------------------------------------------

#ifndef __CRC__H
#define __CRC__H

#define POLY	0x04C11DB7   

//#include "stdafx.h"

typedef unsigned int u_int;
typedef unsigned char u_char;

u_int crcCalculate(u_char *buffer, u_int length);
bool crcCheck(u_char *buffer, int length, u_int receivedCrc);

#endif /* __CRC__H */
