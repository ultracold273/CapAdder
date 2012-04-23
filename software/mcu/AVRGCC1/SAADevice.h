/*
 * SAADevice.h
 *
 * Created: 2011/10/10 16:28:00
 *  Author: ultracold
 */ 


#ifndef SAADEVICE_H_
#define SAADEVICE_H_


#define SAA7113_ADDR_R 0x4B
#define SAA7113_ADDR_W 0x4A

#define SAA7121_ADDR_R 0x89
#define SAA7121_ADDR_W 0x88

void SAA_W(unsigned char addr, unsigned char subaddr, unsigned char data);
unsigned char SAA_R(unsigned char addr, unsigned char subaddr);
void SAA7113_init(void);
void SAA7121_init(void);

#endif /* SAADEVICE_H_ */