/*
 * SramSet.c
 *
 * Created: 2011/10/26 14:05:03
 *  Author: ultracold
 */ 

// PAY ATTENTION ALL ADDRESS TO BE SET MUST BE LONG TYPE!!!

#include "generic.h"
#include "ExtFlash.h"
#include "SramSet.h"
#include <avr/io.h>

void SramAddrSet(unsigned long addr)
{
	DDRA = 0xFF;
	DDRD |= (CSRAM + CSCPLD + WRRAM);
	PORTD &= ~CSCPLD;
	PORTD |= CSRAM;					// Disable SRAM, input data only to CPLD
	DDRB |= (RAMA0 + RAMA1);
	
	PORTB |= (RAMA0 + RAMA1);		// Transmit MSBs of addr
	PORTD &= ~WRRAM;	
	PORTA = (unsigned char)(addr >> 16);
	PORTD |= WRRAM;
	
	PORTB &= ~RAMA0;				// Transmit Middle bits of addr
	PORTD &= ~WRRAM;
	PORTA = (unsigned char)((addr & 0x00FF00) >> 8);
	PORTD |= WRRAM;
	
	PORTB &= ~RAMA1;				// Transmit LSBs of addr
	PORTB |= RAMA0;
	PORTD &= ~WRRAM;
	PORTA = (unsigned char)(addr);
	PORTD |= WRRAM;
	
	PORTB &= ~(RAMA0 + RAMA1);		// Set to data transmission mode
	PORTD &= ~CSRAM;
}

void SramInit(unsigned char backColor)
{
	unsigned char backColorOffset = backColor << 2;
	unsigned char bColorY, bColorB, bColorR;
	unsigned long addrOffset;
	
	bColorY = colorTable[backColorOffset];backColorOffset++;
	bColorB = colorTable[backColorOffset];backColorOffset++;
	bColorR = colorTable[backColorOffset];
	
	for (unsigned char col=0;col<45;col++)
	{
		addrOffset = col * 32;
		SramAddrSet(addrOffset);
		DDRD |= CSRAM;
		PORTD &= ~CSRAM;
		PORTB &= ~(RAMA0 + RAMA1);
		asm("WDR");
		for (int row = 0;row<3200;row++)
		{
			PORTA = bColorB;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorR;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramClear(void)
{
	unsigned long addrOffset;
		
	for (unsigned char col=0;col<45;col++)
	{
		addrOffset = col * 32;
		SramAddrSet(addrOffset);
		DDRD |= CSRAM;
		PORTD &= ~CSRAM;
		PORTB &= ~(RAMA0 + RAMA1);
		DDRA = 0xFF;
		PORTA = 0;
		asm("WDR");
		for (int row = 0;row<3200;row++)
		{
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramScrollInit(unsigned char backColor)
{
	unsigned char backColorOffset = backColor << 2;
	unsigned char bColorY, bColorB, bColorR;
	unsigned long addrOffset;
	
	bColorY = colorTable[backColorOffset];backColorOffset++;
	bColorB = colorTable[backColorOffset];backColorOffset++;
	bColorR = colorTable[backColorOffset];
// NOTICE!!!! THIS PART SHOULD BE CAREFULLY CONSIDERED!!! Problem solved on Nov. 24
// SramAddrSet() function the RAMWR is not correctly set.
	for(unsigned char col = 0;col<45;col++)
	{
		addrOffset = col;
		addrOffset *= 32;
		addrOffset += 360000L;
		SramAddrSet(addrOffset);
		DDRD |= CSRAM;
		PORTD &= ~CSRAM;
		PORTB &= ~(RAMA0 + RAMA1);
		asm("WDR");
		for(unsigned char row = 0;row < 192;row++)
		{
			PORTA = bColorB;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorR;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramScrollClear(void)
{
	unsigned long addrOffset;

	for(unsigned char col = 0;col<45;col++)
	{
		addrOffset = col * 32 + 360000L;
		SramAddrSet(addrOffset);
		DDRD |= CSRAM;
		PORTD &= ~CSRAM;
		PORTB &= ~(RAMA0 + RAMA1);
		PORTA = 0;
		asm("WDR");
		for(unsigned char row = 0;row < 240;row++)
		{
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
}


/***************************************************************************
 * Function: SramHZ16Set
 * Parameter: chHchL is the dual byte to represent a Chinese character
 *				charColor is the color of the character or the border
 *				backColor is the background color of the character
 *				col is the display start column.
 *
 ***************************************************************************/
void SramHZ16Set(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	// Search for charColor
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;	//STRINGIDSPOFFSET is not defined.
	unsigned long charOffset = 94*(chH - 0xA1) + (chL - 0xA1);
	//charOffset += Layer16 * 10000;		// Layer16 is not defined.
	charOffset += HZK16_OFFSET;
	
	ExtFlashPageDataRead(charOffset, buffer, 228);	// buffer is not allocated.//now is allocated.
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<2;i++)
	{
		SramAddrSet(colOffset + 32*i);
		for (unsigned char j=i;j<18*2;j+=2)
		{
			charBody = buffer[j];
			charBorder = buffer[j+36];
			for (unsigned char k=0;k<8;k++)
			{
				if((charBody & 0x80) == 0)
				{
					if((charBorder & 0x80) == 0)
					{
						PORTA = 0;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					} 
					else
					{
						PORTA = fColorB;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorR;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					}
				}
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				charBody <<= 1;
				charBorder <<= 1;
			}
			
		}
	}
	PORTD |= CSCPLD;
}

void SramHZ24Set(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	// Search for charColor
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;	//STRINGIDSPOFFSET is not defined.
	unsigned long charOffset = 94*(chH - 0xA0) + (chL - 0xA1);
	//charOffset += Layer24 * 10000;		// Layer24 is not defined.
	charOffset += HZK24_OFFSET;
	
	ExtFlashPageDataRead(charOffset, buffer, 228);	// buffer is not allocated.//now is allocated.
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<3;i++)
	{
		SramAddrSet(colOffset + 32*i);
		for (unsigned char j=i;j<26*3;j+=3)
		{
			charBody = buffer[j];
			charBorder = buffer[j+78];
			for (unsigned char k=0;k<8;k++)
			{
				if((charBody & 0x80) == 0)
				{
					if((charBorder & 0x80) == 0)
					{
						PORTA = 0;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					} 
					else
					{
						PORTA = fColorB;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorR;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					}
				}
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				charBody <<= 1;
				charBorder <<= 1;
			}
			
		}
	}
	PORTD |= CSCPLD;
}

void SramHZ16SetWithoutBorder(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	// Search for charColor
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;	//STRINGIDSPOFFSET is not defined.
	unsigned long charOffset = 94*(chH - 0xA1) + (chL - 0xA1);
	//charOffset += Layer16 * 10000;		// Layer16 is not defined.
	charOffset += HZK16_OFFSET;
	
	ExtFlashPageDataRead(charOffset, buffer, 228);	// buffer is not allocated.//now is allocated.
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<2;i++)
	{
		SramAddrSet(colOffset + 32*i);
		for (unsigned char j=i;j<18*2;j+=2)
		{
			charBody = buffer[j];
			charBorder = buffer[j+36];
			for (unsigned char k=0;k<8;k++)
			{
				if((charBody & 0x80) == 0)
				{
					PORTA = fColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				charBody <<= 1;
				charBorder <<= 1;
			}
			
		}
	}
	PORTD |= CSCPLD;
}

void SramHZ24SetWithoutBorder(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	// Search for charColor
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;	//STRINGIDSPOFFSET is not defined.
	unsigned long charOffset = 94*(chH - 0xA0) + (chL - 0xA1);
	//charOffset += Layer24 * 10000;		// Layer24 is not defined.
	charOffset += HZK24_OFFSET;
	
	ExtFlashPageDataRead(charOffset, buffer, 228);	// buffer is not allocated.//now is allocated.
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<3;i++)
	{
		SramAddrSet(colOffset + 32*i);
		for (unsigned char j=i;j<26*3;j+=3)
		{
			charBody = buffer[j];
			charBorder = buffer[j+78];
			for (unsigned char k=0;k<8;k++)
			{
				if((charBody & 0x80) == 0)
				{
					PORTA = fColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				charBody <<= 1;
				charBorder <<= 1;
			}
			
		}
	}
	PORTD |= CSCPLD;
}

void sramASC16Set(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	// STRINGDISPOFFSET is preset to 360000L
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;	// REMEMBER TO CHANGE COLOFFSET TO LONG
	unsigned long charOffset = ch - 33 + ASC16_OFFSET;
	//unsigned int charOffset = ch*40;
	//testing
	ExtFlashPageDataRead(charOffset, buffer, 40);
	//for(int i = 0;i<40;i++)
	//{
	//	buffer[i] = ASC16[charOffset + i];
	//}
	
	SramAddrSet(colOffset);
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char j=0;j<18;j++)
	{
		charBody = buffer[j];
		charBorder = buffer[j+20];
		for (unsigned char k=0;k<8;k++)
		{
			if ((charBody & 0x80) == 0)
			{
				if ((charBorder & 0x80) == 0)
				{
					PORTA = 0;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				} 
				else
				{
					PORTA = fColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
			} 
			else
			{
				PORTA = bColorB;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorR;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
			}
			charBody <<= 1;
			charBorder <<= 1;
		}
	}
	PORTD |= CSCPLD;
}

void sramASC16SetWithoutBorder(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;
	unsigned long charOffset = ch - 33 + ASC16_OFFSET;
	//unsigned int charOffset = ch*40;
	// NEED TO BE CHANGED!!!
	ExtFlashPageDataRead(charOffset, buffer, 40);
	//for(int i = 0;i<40;i++)
	//{
	//	buffer[i] = ASC16[charOffset + i];
	//}
	
	SramAddrSet(colOffset);
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char j=0;j<18;j++)
	{
		charBody = buffer[j];
		charBorder = buffer[j+20];//?? 18 or 20
		for (unsigned char k=0;k<8;k++)
		{
			if ((charBody & 0x80) == 0)
			{
				PORTA = fColorB;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = fColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = fColorR;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = fColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
			} 
			else
			{
				PORTA = bColorB;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorR;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
				PORTA = bColorY;
				PORTD &= ~WRRAM;
				PORTD |= WRRAM;
			}
			charBody <<= 1;
			charBorder <<= 1;
		}
	}
	PORTD |= CSCPLD;
}
//@deprecated
void sramASC24Set(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned int colOffset = col * 16 + STRINGDISPOFFSET;
	// The following lines need to be changed.
	unsigned int charOffset = ch*40;
	for(int i = 0;i<40;i++)
	{
		buffer[i] = ASC16[charOffset + i];
	}
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<2;i++)
	{
		SramAddrSet(colOffset + 32 * i);
	
		for(unsigned char j=i;j<18;j++) // 18 need to be examined.
		{
			charBody = buffer[j];
			charBorder = buffer[j+18];//?? 18 or 20
			for (unsigned char k=0;k<8;k++)
			{
				if ((charBody & 0x80) == 0)
				{
					if ((charBorder & 0x80) == 0)
					{
						PORTA = 0;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					} 
					else
					{
						PORTA = fColorB;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorR;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
						PORTA = fColorY;
						PORTD &= ~WRRAM;
						PORTD |= WRRAM;
					}
				} 
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
				charBody <<= 1;
				charBorder <<= 1;
			}
		}
	}	
	PORTD |= CSCPLD;
}

//@deprecated
void sramASC24SetWithoutBorder(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char fColorOffset = charColor << 2;
	unsigned char bColorY, bColorR, bColorB;
	unsigned char fColorY, fColorR, fColorB;
	
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	fColorY = colorTable[fColorOffset];fColorOffset++;
	fColorB = colorTable[fColorOffset];fColorOffset++;
	fColorR = colorTable[fColorOffset];
	
	unsigned int colOffset = col * 16 + STRINGDISPOFFSET;
	// The following lines need to be changed.
	unsigned int charOffset = ch*40;
	for(int i = 0;i<40;i++)
	{
		buffer[i] = ASC16[charOffset + i];
	}
	
	unsigned char charBody;
	unsigned char charBorder;
	
	for(unsigned char i=0;i<2;i++)
	{
		SramAddrSet(colOffset + 32 * i);
	
		for(unsigned char j=i;j<18;j++) // 18 need to be examined.
		{
			charBody = buffer[j];
			charBorder = buffer[j+18];//?? 18 or 20
			for (unsigned char k=0;k<8;k++)
			{
				if ((charBody & 0x80) == 0)
				{
					PORTA = fColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = fColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				} 
				else
				{
					PORTA = bColorB;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorR;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
					PORTA = bColorY;
					PORTD &= ~WRRAM;
					PORTD |= WRRAM;
				}
			}
		}
	}	
	PORTD |= CSCPLD;
}

void SramSpace16Set(unsigned int col)
{
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;
	SramAddrSet(colOffset);
	DDRD |= CSRAM;
	PORTD &= ~CSRAM;
	PORTB &= ~(RAMA0 + RAMA1);
	PORTA = 0;
	asm("WDR");
	
	for(unsigned char j=0;j<18;j++)
	{
		for (unsigned char k=0;k<8;k++)
		{
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramSpace24Set(unsigned int col)
{
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;
	SramAddrSet(colOffset);
	DDRD |= CSRAM;
	PORTD &= ~CSRAM;
	PORTB &= ~(RAMA0 + RAMA1);
	PORTA = 0;
	asm("WDR");
	
	for(unsigned char j=0;j<26;j++)
	{
		for (unsigned char k=0;k<8;k++)
		{
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramSpace16SetWithoutBorder(unsigned char backColor, unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char bColorY, bColorB, bColorR;
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;
	SramAddrSet(colOffset);
	DDRD |= CSRAM;
	PORTD &= ~CSRAM;
	PORTB &= ~(RAMA0 + RAMA1);
	PORTA = 0;
	asm("WDR");
	
	for(unsigned char j=0;j<18;j++)
	{
		for (unsigned char k=0;k<8;k++)
		{
			PORTA = bColorB;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorR;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}

void SramSpace24SetWithoutBorder(unsigned char backColor, unsigned int col)
{
	unsigned char bColorOffset = backColor << 2;
	unsigned char bColorY, bColorB, bColorR;
	bColorY = colorTable[bColorOffset];bColorOffset++;
	bColorB = colorTable[bColorOffset];bColorOffset++;
	bColorR = colorTable[bColorOffset];
	
	unsigned long colOffset = col * 16 + STRINGDISPOFFSET;
	SramAddrSet(colOffset);
	DDRD |= CSRAM;
	PORTD &= ~CSRAM;
	PORTB &= ~(RAMA0 + RAMA1);
	PORTA = 0;
	asm("WDR");
	
	for(unsigned char j=0;j<26;j++)
	{
		for (unsigned char k=0;k<8;k++)
		{
			PORTA = bColorB;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorR;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
			PORTA = bColorY;
			PORTD &= ~WRRAM;
			PORTD |= WRRAM;
		}
	}
	PORTD |= CSCPLD;
}