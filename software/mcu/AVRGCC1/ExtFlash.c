/*
 * ExtFlash.c
 *
 * Created:
 *  Author: ultracold
 */
#include <avr/io.h>
#include "ExtFlash.h"

//CLE often hold low
//ALE often hold low
//WE often hold high
//RE often hold high
//ACE1 often hold high

void PORTC_init(void)
{
	DDRC = 0x7F;
	PORTC = (1<<PC0)|(1<<PC1)|(1<<PC7);
	asm("NOP");
}

void PORTA_init(void)
{
	;
}

void ExtFlashCommandSet(unsigned char cmd)
{
	ACE0_H;
	ALE_L;
	ACE1_L;
	RE_H;
	CLE_H;
	WE_L;
	// SENDA
	DDRA = 0xFF;
	PORTA = cmd;
	
	WE_H;
	//ACE1_H();
	CLE_L;
	//ALE_H();
}

void ExtFlashAddressSet(unsigned char addr)
{
	ACE0_H;
	ACE1_L;
	RE_H;
	CLE_L;
	ALE_H;
	WE_L;
	//SENDA(addr);
	DDRA = 0xFF;
	PORTA = addr;
	
	WE_H;
	ALE_L;
}

void ExtFlashDataSet(unsigned char data)
{
	ACE0_H;
	RE_H;
	CLE_L;
	ACE1_L;
	ALE_L;
	WE_L;
	//SENDA(data);
	DDRA = 0xFF;
	PORTA = data;
	
	WE_H;
}

//This function must be called after address set
unsigned char ExtFlashDataRead(void)
{
	unsigned char data;
	ACE0_H;
	while(!IDLE);
	CLE_L;
	ACE1_L;
	WE_H;
	ALE_L;
	RE_L;
	asm("NOP");
	//GETA(data);
	DDRA = 0x00;
	data = PINA;
	
	RE_H;
	return data;
}

void ExtFlashReset(void)
{
	ExtFlashCommandSet(ExtFlash_Reset);
}

unsigned char ExtFlashStatusRead(void)
{
	unsigned char status;
	ExtFlashCommandSet(ExtFlash_Readstatus);
	status = ExtFlashDataRead();
	ACE1_H;
	return status;
}

unsigned char ExtFlashCheckID(void)
{
	unsigned char id[4];
	ExtFlashCommandSet(ExtFlash_ReadID);
	ExtFlashAddressSet(0x00);
	id[0] = ExtFlashDataRead();
	id[1] = ExtFlashDataRead();
	id[2] = ExtFlashDataRead();
	id[3] = ExtFlashDataRead();
	ACE1_H;
	if(id[0] != 0xEC || id[1] != 0x76 )
	//	||id[2] != 0xA5 || id[3] != 0xC0)
		{
			return 0;
		}
	else
		return 1;
}

void ExtFlashPageDataRead(unsigned long Page, unsigned char *buffer, unsigned char len)
{
	unsigned char i;
	unsigned long blockPage;
	unsigned char *buf = buffer;
	
	blockPage = Page;
	ExtFlashCommandSet(ExtFlash_Read1);
	ExtFlashAddressSet(0x00);
	ExtFlashAddressSet(blockPage & 0xFF);
	ExtFlashAddressSet((blockPage>>8)&0xFF);
	ExtFlashAddressSet((blockPage>>16)&0x01);
	
	while(!IDLE);
	for(i = 0;i<len;i++)
	{
		buf[i] = ExtFlashDataRead();
	}
	ACE1_H;
}

unsigned char ExtFlashPageProgram(unsigned long Page, unsigned char *buffer, unsigned char len)
{
	unsigned int i;
	unsigned long blockPage;
	unsigned char *buf = buffer;
	unsigned char status;
	
	blockPage = Page;
	ExtFlashCommandSet(ExtFlash_PageprogSt);
	ExtFlashAddressSet(0x00);
	ExtFlashAddressSet(blockPage & 0xFF);
	ExtFlashAddressSet((blockPage>>8) & 0xFF);
	ExtFlashAddressSet((blockPage>>16) & 0x01);
	
	for(i = 0;i < len;i++)
	{
		ExtFlashDataSet(buf[i]);
	}
	ExtFlashCommandSet(ExtFlash_PageprogEd);
	while(!IDLE);
	status = ExtFlashStatusRead();
	if(status & 0x01)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

unsigned char ExtFlashBlockErase(unsigned int block)
{
	unsigned long blockPage;
	unsigned char status;
	blockPage = block << 5;
	
	ExtFlashCommandSet(ExtFlash_BlockeraseSt);
	ExtFlashAddressSet(blockPage & 0xFF);
	ExtFlashAddressSet((blockPage>>8) & 0xFF);
	ExtFlashAddressSet((blockPage>>16) & 0x01);
	ExtFlashCommandSet(ExtFlash_BlockeraseEd);
	
	asm("NOP");
	while(!IDLE);
	status = ExtFlashStatusRead();
	if (status & 0x01)
	{
		return 0;
	} 
	else
	{
		return 1;
	}
}

unsigned char ExtFlashWholeErase(void)
{
	unsigned int block;
	for(block = 0; block < 2048;block++)
	{
		if (!ExtFlashBlockErase(block))
		{
			return 0;
		}
	}
	return 1;
}