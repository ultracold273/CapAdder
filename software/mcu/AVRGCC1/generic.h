/*
 * generic.h
 *
 * Created: 2011/10/19 13:59:26
 *  Author: ultracold
 */ 


#ifndef GENERIC_H_
#define GENERIC_H_

#define BYTE unsigned char
#define WORD unsigned int
#define DWORD unsigned long

#define SCROLL 0x01
#define FULLSCR 0x02

#define DOT16 0x01
#define DOT24 0x02

#define BORDER 0x01
#define NOBORDER 0x02

typedef struct {
	BYTE DisplayMode;
	BYTE FontSize;
	BYTE isBorder;
	BYTE CharacLength;
	BYTE BackgdColor;
	BYTE CharacColor;
	BYTE StartRow;
	BYTE StartCol;
	BYTE isChanged;
} INFO;

#define NETWORKBUF_MAX_SIZE 1024

extern BYTE *webPage;
extern INFO info;
extern BYTE buffer[256];
extern BYTE characBuffer[64];
extern BYTE bufferLength;
extern BYTE ASC16[];// NEED TO BE MODIFIED
extern const BYTE colorTable[];
extern DWORD STRINGDISPOFFSET;
extern BYTE length;
extern int STR;
extern BYTE timeAccumulate;
extern BYTE networkBuffer[NETWORKBUF_MAX_SIZE];

// Flash Offset Address Definition for HZK
#define ASC16_OFFSET 8400L;
#define ASC24_OFFSET 18400L;
#define HZK16_OFFSET 0L;
#define HZK24_OFFSET 10000L;

#endif /* GENERIC_H_ */