/*
 * BufferDataProc.c
 *
 * Created: 2011/11/4 9:43:57
 *  Author: ultracold
 */ 
#if 0
#include "generic.h"
#include "SramSet.h"
#include "USART.h"

void DataReceive(void)
{
	characBuffer[bufferLength] = USART_Receive();
	bufferLength++;
	
	if(bufferLength >= 240)
	{
		for(int i=0;i<20;i++)
			characBuffer[i] = 0;
		bufferLength = 0;
	}
	
	if(bufferLength == 1)
	{
		if (characBuffer[0] != '[')
		{
			for (int i=0;i<20;i++)
			{
				characBuffer[i] = 0;
			}
			bufferLength = 0;
		}
	}
}

void CmdDeal(void)
{
	if(bufferLength >= 3)
	{
		if (characBuffer[0] == '[' && characBuffer[bufferLength - 1] == ']')
		{
			switch(characBuffer[1])
			{
				case 'D':
				case 'd':WholeScreen24Display();break;
				case 'H':
				case 'h':WholeScreen16Display();break;
				case 'I':
				case 'i':Scroll24Display();break;
				case 'J':
				case 'j':Scroll16Display();break;
				case 'E':break;
				case 'P':break;
				case 'X':break;
				case 'x':break;
			}
		}
	}
}

void WholeScreen24Display(void)
{
	unsigned char charColor, backColor;
	unsigned int startRow, startCol;
	if(bufferLength >= 10)
	{
		/* Set up character color */
		if(characBuffer[2] > '9' || characBuffer[2] < '0')
		{
			charColor = 10;
		}else{
			charColor = characBuffer[2] - '0';
		}
		
		/* Set up background color */
		if (characBuffer[3] > '9' || characBuffer[3] < '0')
		{
			backColor = 10;
		}else{
			backColor = characBuffer[3] - '0';
		}
		
		/* Set up start row of the character */
		if (characBuffer[4] < '9' && characBuffer[4] > '0'
			&& characBuffer[5] < '9' && characBuffer[5] > '0'
			&& characBuffer[6] < '9' && characBuffer[6] > '0')
		{
			startRow = 100 * (characBuffer[4] - '0') + 10 * (characBuffer[5] - '0') + (characBuffer[6] - '0');
			STRINGDISPOFFSET = startRow * 1440;
			if(startRow > 214)
			{
				
			}
		}
		else
		{
			USART_Transmit('e');
		}
		
		/* Set up start column of the character */
		if (characBuffer[7] < '9' && characBuffer[7] > '0'
		&& characBuffer[8] < '9' && characBuffer[8] > '0')
		{
			startCol = 10 * (characBuffer[7] - '0') + (characBuffer[8] - '0');
			STRINGDISPOFFSET += startCol * 16;
		}
		else
		{
			USART_Transmit('e');
		}
		
		/* Start writing characters into the static ram */
		unsigned int characIndex = 9;
		while(characBuffer[characIndex] != ']')
		{
			/* Judge what character is in the character buffer */
			// Chinese Character
			if(characBuffer[characIndex] > 0xA0 && characBuffer[characIndex + 1] > 0xA0)
			{
				if(characBuffer[1] == 'D') SramHZ24Set(characBuffer[characIndex], characBuffer[characIndex + 1],
														charColor, backColor, startRow);
				if(characBuffer[1] == 'd') SramHZ24SetWithoutBorder(characBuffer[characIndex], characBuffer[characIndex + 1],
														charColor, backColor, startRow);
				characIndex += 2;
				startRow += 6;
				
			}// ASC character
			else if(characBuffer[characIndex] > 0x20 && characBuffer[characIndex] < 0x7F)
			{
				if (characBuffer[1] == 'D') sramASC24Set(characBuffer[characIndex], charColor, backColor, startRow);
				if (characBuffer[1] == 'd') sramASC24SetWithoutBorder(characBuffer[characIndex], charColor, backColor, startRow);
				characIndex++;
				startRow += 3;
			}// Other character used as space
			else
			{
				if (characBuffer[1] == 'D') SramSpace24Set(startRow);
				if (characBuffer[1] == 'd') SramSpace24SetWithoutBorder(backColor, startRow);
				
				characIndex++;
				startRow += 3;
			}
		}
	}
}

void WholeScreen16Display(void)
{
	
}

void Scroll24Display(void)
{
	
}

void Scroll16Display(void)
{
	
}
#endif