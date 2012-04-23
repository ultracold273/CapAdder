/*
 * USART.c
 *
 * Created:
 *  Author: ultracold
 */
#include <avr/io.h>
#include "USART.h"

void USART_init()
{
	/* Set the Baud rate */
	UBRRH = 0x00;
	UBRRL = 51;
	/* Enable Transmitter & Receiver */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set Frame Format: 8 data bits, 1 stop bits */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for the transmit buffer to be empty */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put the data into transmit buffer */
	UDR = data;
	while(!(UCSRA & (1<<TXC)));
	UCSRA |= (1<<TXC);
}

unsigned char USART_Receive( void )
{
	/* Wait to receive data */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get the data from the buffer and return the data */
	return UDR;
}

void USART_StringPut(char *str)
{
	while(*str) // Ignore the warning!!
	{
		USART_Transmit(*str++);
	}
	USART_Transmit(0x0d);
	USART_Transmit(0x0a);
}


/* Reference code derivated from PIC18F4620. 
BYTE ConsoleGetString(char *buffer, BYTE bufferLen)
{
    BYTE v;
    BYTE count;

    count = 0;
    do
    {
        if ( bufferLen-- == 0 )
            break;

        while( !ConsoleIsGetReady() );

        v = RCREG;

        if ( v == '\r' || v == '\n' )
            break;

        count++;
        *buffer++ = v;
        *buffer = '\0';
    } while(1);
    return count;
}*/
/*
unsigned char USART_StringGet(unsigned char *buffer, unsigned char bufferlen)
{
	unsigned char v, count;
	
	count = 0;
	do 
	{
		if(bufferlen-- == 0)
		{
			break;
		}
		
		v = USART_Receive();
		
		if(v == '\r' || v == '\n')
		{
			break;
		}
		
		count++;
		*buffer++ = v;
		*buffer = '\0';
	} while (1);
	return count;
}*/