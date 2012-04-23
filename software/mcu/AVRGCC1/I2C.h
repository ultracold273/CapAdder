/*
 * I2C.h
 *
 * Created: 2011/10/10 10:38:06
 *  Author: ultracold
 */ 


#ifndef I2C_H_
#define I2C_H_

#define SCL 0x01
#define SDA 0x02

#define START 0x08
#define RESTART 0x10
#define MT_SLAW_ACK 0x18
#define MT_SLAR_ACK 0x40
#define MT_DATAW_ACK 0x28
#define MT_DATAR_ACK 0x50

void I2C_init(void);
void I2C_Send_M_Start(void);
void I2C_Send_M_ReStart(void);
void I2C_Send_M_Addr(unsigned char addr);
void I2C_Send_M_Data(unsigned char data);
void I2C_Send_M_Stop(void);
unsigned char I2C_Send_M_Read(void);
void ERROR(unsigned char type);
void DELAYUS(unsigned int wx);

#endif /* I2C_H_ */