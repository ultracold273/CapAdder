/*
 * SramSet.h
 *
 * Created: 2011/10/31 11:01:37
 *  Author: ultracold
 */ 


#ifndef SRAMSET_H_
#define SRAMSET_H_

#define CSRAM 0x04
#define CSCPLD 0x20
#define WRRAM 0x40
#define RAMA0 0x01
#define RAMA1 0x02

void SramAddrSet(unsigned long addr);
void SramInit(unsigned char backColor);
void SramClear(void);
void SramScrollInit(unsigned char backColor);
void SramScrollClear(void);
void SramHZ16Set(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void SramHZ24Set(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void SramHZ16SetWithoutBorder(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void SramHZ24SetWithoutBorder(unsigned char chH, unsigned char chL, 
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void sramASC16Set(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void sramASC16SetWithoutBorder(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void sramASC24Set(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void sramASC24SetWithoutBorder(unsigned ch,
				unsigned char charColor, unsigned char backColor,
				unsigned int col);
void SramSpace16Set(unsigned int col);
void SramSpace24Set(unsigned int col);
void SramSpace16SetWithoutBorder(unsigned char backColor, unsigned int col);
void SramSpace24SetWithoutBorder(unsigned char backColor, unsigned int col);

#endif /* SRAMSET_H_ */