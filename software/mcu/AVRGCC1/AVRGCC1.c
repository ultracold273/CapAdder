/*
 * AVRGCC1.c
 *
 * Created: 2011/10/8 16:54:41
 *  Author: Wei Lingxiao
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C.h"
#include "SAADevice.h"
#include "SramSet.h"
#include "generic.h"
#include "ExtFlash.h"
#include "SPI_enc28j60.h"
#include "nwkStack.h"
#include "httpd.h"
#include "USART.h"

#define WHITE 0
#define YELLOW 1
#define MAGENTA 2
#define RED 3
#define CYAN 6
#define GREEN 7
#define BLUE 8
#define BLACK 10

void showScrollString();
void showStaticString(unsigned int row, unsigned int col);

int main(void)
{
	// Initial the setting in AVR MCU
	asm("WDR");
	// Watchdog setting
	WDTCR |= (1<<WDTOE) | (1<<WDE);
	WDTCR = 0x00;
	/*
	// Timer0 interrupt setting
	TCCR0 = 0x00;
	TCNT0 = 0x00;
	OCR0 = 144;
	//TIMSK &= ~0x02;
	//TIMSK |= 0x02;
	TCCR0 = 0x0D;
	while(!TIFR &(1<<OCF0));
	*/
	
	USART_init();
	USART_StringPut("Enter the Video");
	
	// Timer1 interrupt setting
	TCNT1 = 0;
	OCR1A = (unsigned int)46875;
	TCCR1A = 0x00;
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
	while(!(TIFR & (1<<OCF1A)));
	TIFR |= (1<<OCF1A);
	USART_StringPut("Timer1 Done");
	
	
	// Initialize the SPI
	SPI_MasterInit();
	enc28j60_init();
	USART_StringPut("Network Done");

	// INT2 interrupt setting
	GICR &= ~(1<<INT2);
	GICR |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2);
	USART_StringPut("INT2 Done");
	
	// SPI interface setting
	// Preset SS_n to output!! Or the SPI mode cannot be set as Master Mode.
	//DDRB |= (1<<PB5) | (1<<PB7);
	//DDRB |= (1<<PB4);
	//SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	
	
	// I2C interface setting
	I2C_init();
	//USART_StringPut(TWBR);
	//DDRB &= ~SDA;
	//PORTB |= SDA;
	SAA7121_init();
	SAA7113_init();
	//SAA7121_init();
	USART_StringPut("Video Done");
	// Static ram initialize
	SramClear();
	//SramScrollInit(BLUE);
	//sramASC16Set(90,GREEN,MAGENTA,10);
	//sramASC16SetWithoutBorder(71,YELLOW,CYAN,12);
	// External Flash initialize
	PORTC_init();
	ExtFlashReset();
	//ExtFlashPageProgram(8432, ASC16, 40);
	STR = 5;
	timeAccumulate = 0;
	info.CharacLength = 21;
	info.BackgdColor = BLACK;
	info.CharacColor = MAGENTA;
	info.DisplayMode = SCROLL;
	info.FontSize = DOT16;
	info.isBorder = BORDER;
	showStaticString(155, 10);
	USART_StringPut("Configuration Down");
	sei();
	//showString();
	//sramASC16Set('E', GREEN, MAGENTA, 10);
	//sramASC16Set(77, GREEN, MAGENTA, 12);
	//for(int i=88;i<95;i++)
	//{
	//	ExtFlashPageProgram(i,ASC16+(i-88)*40,40);
	//}
	//SramClear();
	DDRD |= 0x20;
	PORTD |= 0x20;
	// Setting SAA7113 & SAA7121 through I2C Bus

    while(1)
    {
        //TODO: Please write your application code 
		//Wait for the UART transmission to wake up;
		
		asm("WDR");
    }
}

void setInterruptTimer()
{
	// INT1 interrupt setting
	GICR &= ~0x80;
	GICR |= 0x80;
	MCUCR = 0x08;	
}

void stopInterruptTimer()
{
	// INT1 interrupt disable
	GICR &= ~0x80;
}

/* Interrupt Handler - VREF falling edge trigger */
ISR(INT1_vect)
{
	cli();
	if(info.DisplayMode == SCROLL)
	{
		timeAccumulate++;
		if (timeAccumulate >= 16)
		{
			timeAccumulate = 0;
			STR--;
			if(info.FontSize == DOT16)
			{
				if((STR + info.CharacLength*2) <=0)
				{
					STR = 88;
				}
			}else if(info.FontSize == DOT24)
			{
				if((STR + info.CharacLength*3) <=0)
				{
					STR = 88;
				}
			}
			showScrollString();
		}
	}	
	sei();
}

ISR(INT2_vect)
{
	cli();
	server();
	if(info.isChanged){
		if(info.DisplayMode == FULLSCR)
		{
			SramClear();
			showStaticString(info.StartRow, info.StartCol);
			stopInterruptTimer();
		}else{
			SramClear();
			setInterruptTimer();
		}
		info.isChanged = 0;
	}	
	sei();
}

void showScrollString()
{
	STRINGDISPOFFSET = 360000L;
	if(info.FontSize == DOT16)
	{
		for(int i = 0;i<info.CharacLength;i++)
		{
			if ((STR + i*2) <= 0 || (STR + i*2) >= 88)
			{
				if(characBuffer[i] > 0xA0)
				{
					i++;
				}
			}else
			{
				if(info.isBorder == BORDER)
				{
					if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
						sramASC16Set(characBuffer[i], info.BackgdColor, info.CharacColor, i*2 + STR);
					else if(characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
					{
						SramHZ16Set(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, i*2 + STR);
						i++;
					}						
					else
						SramSpace16Set(i*2 + STR);
				}					
				if(info.isBorder == NOBORDER)
				{
					if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
						sramASC16SetWithoutBorder(characBuffer[i], info.BackgdColor, info.CharacColor, i*2 + STR);
					else if(characBuffer[i] > 0xA0 && characBuffer[i + 1] > 0xA0)
					{
						SramHZ16SetWithoutBorder(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, i*2 + STR);
						i++;
					}
					else
						SramSpace16SetWithoutBorder(info.BackgdColor, i*2 + STR);
				}
			}
		}
		// Clear the last character
		if ((STR + info.CharacLength*2) <= 88)
		{
			if(info.isBorder == BORDER)
				SramSpace16Set(STR + info.CharacLength*2);
			if(info.isBorder == NOBORDER)
				SramSpace16SetWithoutBorder(info.BackgdColor, STR + info.CharacLength*2);			
		}
	}else if(info.FontSize == DOT24)
	{
		for(int i = 0;i<info.CharacLength;i++)
		{
			if ((STR + i*3) <= 0 || (STR + i*3) >= 88)
			{
				if(characBuffer[i] > 0xA0)
				{
					i++;
				}
			}else
			{
				if(info.isBorder == BORDER)
				{
					if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
						sramASC16Set(characBuffer[i], info.BackgdColor, info.CharacColor, i*3 + STR);
					else if(characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
					{
						SramHZ24Set(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, i*3 + STR);
						i++;
					}						
					else
						SramSpace24Set(i*3 + STR);
				}					
				if(info.isBorder == NOBORDER)
				{
					if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
						sramASC16SetWithoutBorder(characBuffer[i], info.BackgdColor, info.CharacColor, i*3 + STR);
					else if(characBuffer[i] > 0xA0 && characBuffer[i + 1] > 0xA0)
					{
						SramHZ24SetWithoutBorder(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, i*3 + STR);
						i++;
					}
					else
						SramSpace24SetWithoutBorder(info.BackgdColor, i*3 + STR);
				}
			}
		}
		
		// Clear the last character
		if ((STR + info.CharacLength*3) <= 88)
		{
			if(info.isBorder == BORDER)
				SramSpace24Set(STR + info.CharacLength*3);
			if(info.isBorder == NOBORDER)
				SramSpace24SetWithoutBorder(info.BackgdColor, STR + info.CharacLength*3);			
		}
	}
}

void showStaticString(unsigned int row, unsigned int col)
{
	STRINGDISPOFFSET = row * 1440L;
	unsigned int column = col;
	if(info.FontSize == DOT16)
	{	
		for(int i = 0;i<info.CharacLength;i++)
		{
			if(info.isBorder == BORDER)
			{
				if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
				{
					sramASC16Set(characBuffer[i], info.BackgdColor, info.CharacColor, column);
					column += 2;
				}					
				else if (characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
				{
					SramHZ16Set(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, column);
					i++;
					column += 4;
				}else{
					SramSpace16Set(column);
					column += 2;
				}
				if(column >= 82)
				{
					column = 4;
					STRINGDISPOFFSET += 18*1440L;
				}
			}					
			if(info.isBorder == NOBORDER)
			{
				if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
				{
					sramASC16SetWithoutBorder(characBuffer[i], info.BackgdColor, info.CharacColor, column);
					column += 2;
				}					
				else if (characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
				{
					SramHZ16SetWithoutBorder(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, column);
					i++;
					column += 4;
				}else{
					SramSpace16SetWithoutBorder(info.BackgdColor, column);
					column += 2;
				}
				if(column >= 82)
				{
					column = 4;
					STRINGDISPOFFSET += 18*1440L;
				}					
			}
		}
	}else if(info.FontSize == DOT24)
	{
		for(int i = 0;i<info.CharacLength;i++)
		{
			if(info.isBorder == BORDER)
			{
				if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
					sramASC16Set(characBuffer[i], info.BackgdColor, info.CharacColor, i*3 + col);//deprecated
				else if (characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
				{
					SramHZ24Set(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, column);
					i++;
					column += 6;
				}else{
					SramSpace24Set(column);
					column += 3;
				}
				if(column >=84)
				{
					column = 4;
					STRINGDISPOFFSET += 26*1440L;
				}
			}					
			if(info.isBorder == NOBORDER)
			{
				if(characBuffer[i] > 0x20 && characBuffer[i] < 0x7F)
					//deprecated
					sramASC16SetWithoutBorder(characBuffer[i], info.BackgdColor, info.CharacColor, i*3 + col);
				else if (characBuffer[i] > 0xA0 && characBuffer[i+1] > 0xA0)
				{
					SramHZ24SetWithoutBorder(characBuffer[i], characBuffer[i+1], info.BackgdColor, info.CharacColor, column);
					i++;
					column += 6;
				}else{
					SramSpace24SetWithoutBorder(info.BackgdColor, column);
					column += 3;
				}
				if (column >= 84)
				{
					column = 4;
					STRINGDISPOFFSET += 26*1440L;
				}
			}
		}
	}		
}