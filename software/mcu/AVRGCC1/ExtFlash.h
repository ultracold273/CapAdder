/*
 * ExtFlash.h
 *
 * Created: 2011/10/20 10:37:23
 *  Author: ultracold
 */ 


#ifndef EXTFLASH_H_
#define EXTFLASH_H_

#define ExtFlash_Read1		0x00
#define ExtFlash_Read2		0x01
#define ExtFlash_ReadID		0x90    //ReadID
#define ExtFlash_Reset		0xFF    //Reset
#define ExtFlash_PageprogSt	0x80    //Start Writing
#define ExtFlash_PageprogEd	0x10    //End Writing
#define ExtFlash_Copyback1	0x00    
#define ExtFlash_Copyback2	0x8A
#define ExtFlash_BlockeraseSt	0x60    //Block Erase Start
#define ExtFlash_BlockeraseEd	0xD0    //Block Erase End
#define ExtFlash_Readstatus	0x70    //Read Status Register



#define CLE_H (PORTC |= (1<<PC4))
#define CLE_L (PORTC &= ~(1<<PC4))
#define ALE_H (PORTC |= (1<<PC3))
#define ALE_L (PORTC &= ~(1<<PC3))
#define WE_H (PORTC |= (1<<PC2))
#define WE_L (PORTC &= ~(1<<PC2))
#define RE_H (PORTC |= (1<<PC6))
#define RE_L (PORTC &= ~(1<<PC6))
#define ACE1_H (PORTC |= (1<<PC5))
#define ACE1_L (PORTC &= ~(1<<PC5))
#define ACE0_H (PORTD |= (1<<PC5))

#define IDLE (PINC & (1<<PINC7))

void PORTC_init(void);
void PORTA_init(void);
void ExtFlashCommandSet(unsigned char cmd);
void ExtFlashAddressSet(unsigned char addr);
void ExtFlashDataSet(unsigned char data);
unsigned char ExtFlashDataRead(void);
void ExtFlashReset(void);
unsigned char ExtFlashStatusRead(void);
unsigned char ExtFlashCheckID(void);
void ExtFlashPageDataRead(unsigned long Page, unsigned char *buffer, unsigned char len);
unsigned char ExtFlashPageProgram(unsigned long Page, unsigned char *buffer, unsigned char len);
unsigned char ExtFlashBlockErase(unsigned int block);
unsigned char ExtFlashWholeErase(void);



#endif /* EXTFLASH_H_ */