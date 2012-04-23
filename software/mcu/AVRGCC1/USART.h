/*
 * USART.h
 *
 * Created: 2011/10/12 9:01:20
 *  Author: ultracold
 */ 


#ifndef USART_H_
#define USART_H_

void USART_init();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void USART_StringPut(char *str);
//unsigned char USART_StringGet(unsigned char *buffer, unsigned char bufferlen);



#endif /* USART_H_ */