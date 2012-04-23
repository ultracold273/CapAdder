/*
 * generic.c
 *
 * Created: 2011/10/28 9:50:13
 *  Author: ultracold
 */ 

#include "generic.h"

const BYTE colorTable[ ] = 
{
	235,128,128,0,210, 16,146,0,106,202,222,0, 81, 90,240,0,
	121,240,110,0,159, 75,197,0,170,166, 16,0,145, 54, 34,0,
	 41,240,110,0, 48,109,104,0, 16,128,128,0
};
BYTE networkBuffer[NETWORKBUF_MAX_SIZE];
BYTE buffer[256];
BYTE characBuffer[64] =
{ 0xBB, 0xB6, 0xD3, 0xAD, 0xCA, 0xB9, 0xD3, 0xC3, 0xD7, 0xD6, 0xB7,0xFB,0xB5,0xFE,0xBC,0xD3,0xC6,0xF7,'o','l','d'
//	'0','8','1','1','8','0','0','9','5'
};
BYTE bufferLength;
BYTE ASC16[] =
{
};//NEED TO BE MODIFIED
DWORD STRINGDISPOFFSET;
BYTE length;
int STR;
BYTE timeAccumulate;
INFO info;