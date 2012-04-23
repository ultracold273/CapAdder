/*
 * I2C.c
 *
 * Created: 2011/10/9 16:25:27
 *  Author: ultracold
 */ 
#include <avr/io.h>
#include "I2C.h"
#include "USART.h"

/*
#define SCL_H(us) { \
	DDRC |= SCL; \
	PORTC |= SCL; \
	DELAYUS(us); \
}

#define SCL_L(us) { \
	DDRC |= SCL; \
	PORTC &= ~SCL; \
	DELAYUS(us); \
}

#define SDA_H(us) { \
	DDRC &= ~SDA; \
	PORTC |= SDA; \
	DELAYUS(us); \
}

#define SDA_L(us) { \
	DDRC |= SDA; \
	PORTC &= ~SDA; \
	DELAYUS(us); \
}

void I2C_init(void)
{
	DDRC |= SCL;
	PORTC |= (SCL + SDA);
	// Testing Code
	//DDRB |= SDA;
	//PORTB |= SDA; 
//	while(1)
//	{
		//PORTB ^= SDA;
//	}
}

void I2C_Send_M_Start(void)
{
	SCL_H(40);
	SDA_H(40);
	SDA_L(40);
}

void I2C_Send_M_ReStart(void)
{
	I2C_Send_M_Start();
}

void I2C_Send_M_Stop(void)
{
	SDA_L(40);
	SCL_L(40);
	SCL_H(40);
	SDA_H(40);
}

void I2C_Send_M_Addr(unsigned char addr)
{
	unsigned char counter;
	SCL_L(16);
	for(counter = 0;counter < 8;counter++)
	{
		if (addr & 0x80)
		{
			SDA_H(7);
		}else{
			SDA_L(7);
		}
		SCL_H(7);
		SCL_L(5);
		addr <<= 1;
	}
	SDA_H(23);
	SCL_H(7);
	if(PINC & SDA)
	{
		ERROR();
	}
}

void I2C_Send_M_Data(unsigned char data)
{
	unsigned char counter;
	SCL_L(16);
	for(counter = 0;counter < 8;counter++)
	{
		if (data & 0x80)
		{
			SDA_H(7);
		}else{
			SDA_L(7);
		}
		SCL_H(7);
		SCL_L(4);
		data <<= 1;
	}
	SDA_H(23);
	SCL_H(7);
	if(PINC & SDA)
	{
		ERROR();
	}
}

unsigned char I2C_Send_M_Read(void)
{
	unsigned char counter;
	unsigned char data = 0;
	SDA_H(0);
	
	SCL_L(12);
	for(counter = 0;counter < 8;counter++)
	{
		SCL_H(16);
		data <<= 1;
		if(PINB & SDA)
		{
			data |= 0x01;
		}
		SCL_L(12);
	}
	return data;
}

void DELAYUS(unsigned int wt)
{
	for (;wt>0;wt--)
	{
		__asm__("NOP");
		__asm__("NOP");
		__asm__("NOP");
		__asm__("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
	}
}

void ERROR(void)
{
	USART_StringPut("I2C ERROR");
}

/*
 * This is used in the inner module TWI to implement the I2C interface
 * But in reality, the SCI & SDA pin did not connect to the functional
 * pins. So this kind of implement is not appropriate in this situation
 *
 */
void I2C_init(void)
{
	// set bit register rate = 7.3728M/(16 + 2*10*1) = 200kbps
	// 16M/(16 + 2* 18 * 4) = 100kbps
	DDRC &= ~((1<<PC0) + (1<<PC1));
	PORTC |= (1<<PC0);
	TWBR = 72;
	// Last 2 bits set to zero pre-dividor is 4
	TWSR = 0x00;
}

void I2C_Send_M_Start(void)
{
	// Send START Signal 
	TWCR = (1<<TWEN)|(1<<TWSTA);
	TWCR |= (1<<TWINT);
	// Wait for the Start Signal has been sent. 
	while(!(TWCR & (1<<TWINT)));
	// Check TWI Status Register 
	if(((TWSR & 0xF8) != START) && ((TWSR & 0xF8) != RESTART)) ERROR(1);
}

void ERROR(unsigned char type) 
{
	USART_StringPut("I2C ERROR");
	switch (type)
	{
		case 1:USART_StringPut("start error");
				for(int i = 0;i<8;i++)
				{
					USART_Transmit(((TWSR >> i) & 0x01) + 0x30);
				}
				break;
		case 2:USART_StringPut("restart error");break;
		case 3:USART_StringPut("write addr error");break;
		case 4:USART_StringPut("read addr error");break;
		case 5:USART_StringPut("transmit data error");break;
		case 6:USART_StringPut("read data error");break;
		default:break;
	}
	//throw std::exception("The method or operation is not implemented.");
}

void I2C_Send_M_ReStart(void)
{
	// Send ReStart Signal 
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	// Wait for the Start Signal has been sent. 
	while(!(TWCR & (1<<TWINT)));
	// Check TWI Status Register 
	if((TWSR & 0xF8) != RESTART) ERROR(2);
}

void I2C_Send_M_Addr(unsigned char addr)
{	
	// Load Address and Start transmission 
	TWDR = addr;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	if((addr & 0x01) == 0)	//write operation
	{
		if((TWSR & 0xF8) != MT_SLAW_ACK) ERROR(3);
	}else{					//read operation
		if((TWSR & 0xF8) != MT_SLAR_ACK) ERROR(4);
	}
}

void I2C_Send_M_Data(unsigned char data)
{	
	// Load Data and Start transmission 
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	// Wait for the Data has been sent
	while(!(TWCR &(1<<TWINT)));
	// Check TWI Status Register 
	if((TWSR & 0xF8) != MT_DATAW_ACK) ERROR(5);
}

void I2C_Send_M_Stop(void)
{	
	// Send Stop Signal
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

unsigned char I2C_Send_M_Read(void)
{
	// Set Receive Bits 
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	// Wait for the data to be received 
	while(!(TWCR & (1<<TWINT)));
	// Check TWI Status Register 
	if((TWSR & 0xF8) != MT_DATAR_ACK) ERROR(6);
	return TWDR;
}
