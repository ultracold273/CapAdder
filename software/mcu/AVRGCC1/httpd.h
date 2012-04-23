/*
 * httpd.h
 *
 * Created: 2012/2/21 15:40:17
 *  Author: ultracold
 */ 


#ifndef HTTPD_H_
#define HTTPD_H_

void server(void);
unsigned char HttpRequestDeal(unsigned char *httpData, unsigned int length);
unsigned char bufferDeal(unsigned char *udpData, unsigned int length);


#endif /* HTTPD_H_ */