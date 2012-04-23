/*
 * httpd.c
 *
 * Created: 2012/2/21 15:40:01
 *  Author: ultracold
 */ 

#include "nwkStack.h"
#include "SPI_enc28j60.h"
#include "generic.h"
#include "httpd.h"
#include "ExtFlash.h"
#include <avr/io.h>

#define WEB_PAGE_LENGTH 250

void server(void)
{
	static unsigned char* ok = "OK";
	static unsigned char* error = "ERROR";
	static unsigned char* notfound = "HTTP";
	//static unsigned char* notfound = "HTTP/1.1 404 Not Found\r\nServer: nginx/0.8.54\r\nContent-Type: text/html; charset=gb2312\r\nContent-Length: 110\r\nConnection: keep-alive\r\nAccept-Ranges: bytes\r\n\r\n";
	static unsigned char* webPage = "HTTP/1.1 200 OK\r\nServer: nginx/0.8.54\r\nContent-Type: text/html; charset=gb2312\r\nContent-Length: 110\r\nConnection: keep-alive\r\nAccept-Ranges: bytes\r\n\r\n<html>\n<head>\n<title>字符叠加器</title>\n</head>\n<body>\n<h1>你好，这是ATmega32.</h1>\n</body>\n</html>\n";
	
	while(enc28j60_receivePacket(NETWORKBUF_MAX_SIZE, networkBuffer))
	{
		if (isArpAndTargetMyIp(networkBuffer))
		{
			sendArpAnswers(networkBuffer);
		} 
		else if(isUdpPacket(networkBuffer))
		{
			if(50123 == getUdpPort(networkBuffer))
			{				
				if(bufferDeal(getUdpDataPointer(networkBuffer), getIpPacketLength(networkBuffer) - 28))
				{
					makeUdpPacket(2, networkBuffer, ok);
				}else{
					makeUdpPacket(5, networkBuffer, error);
				}
			}else{
				makeUdpPacket(5, networkBuffer, error);
			}
			
		}else if(isTcpAndSyn(networkBuffer))		// TCP 3-handshake
		{
			if (80 == getTcpPort(networkBuffer))
			{
				makeTcpHeader(networkBuffer, TCP_FLAGS_ACK_MASK | TCP_FLAGS_SYN_MASK, 24, 1);
			}
		}else if(isTcpAndRequireData(networkBuffer))
		{
			if (80 == getTcpPort(networkBuffer))
			{
				switch (HttpRequestDeal(getTcpDataPointer(networkBuffer), getIpPacketLength(networkBuffer) - 40))
				{
					case 1:{makeTcpPacket(networkBuffer, webPage, WEB_PAGE_LENGTH);
							makeTcpHeader(networkBuffer, TCP_FLAGS_ACK_MASK | TCP_FLAGS_PSH_MASK, 
											WEB_PAGE_LENGTH + 20, 0);
							break;}
					case 2:{makeTcpPacket(networkBuffer, notfound, 156);
							makeTcpHeader(networkBuffer, TCP_FLAGS_PSH_MASK | TCP_FLAGS_ACK_MASK,
											156 + 20, 0);
							break;}
					case 3:{break;} // This part should be modified.
					default:{break;}
				}
			}
		}else if(isTcpAndFinAck(networkBuffer))
		{
			if(80 == getTcpPort(networkBuffer))
			{
				makeTcpHeader(networkBuffer, TCP_FLAGS_ACK_MASK | TCP_FLAGS_FIN_MASK, 20, 0);
			}
		}
	}
}

/* 
 * This function is used to recognize the HTTP request method.
 * @param  httpData points to the start byte of an HTTP request.
 *		   length indicate the whole HTTP packet's length.
 * @return 1 stands for a GET method, and the URL requested is valid.
 *		   2 stands for a GET method, but the URL requested cannot be found.
 *         3 stands for a POST method.
 *		   0 stands for errors.
 */
unsigned char HttpRequestDeal(unsigned char *httpData, unsigned int length)
{
	if (length < 3)
	{
		return 0;
	}else if (httpData[0] == 'G' && httpData[1] == 'E' && httpData[2] == 'T')
	{
		if (httpData[3] == 0x20 && httpData[4] == 0x2F && httpData[5] == 0x20)
		{
			return 1;
		}else
		{
			return 2;
		}
	}else if (httpData[0] == 'P' && httpData[1] == 'O' && httpData[2] == 'S'
				&& httpData[3] == 'T')
	{
		return 3;	
	}else
	{
		return 0;
	}
}

/*
 * bufferDeal() deal with the udp Data packet, mainly contains character database or the 
 * display command and data. udpData points to the storage space contains those data, and
 * length is the valid storage space length.
 * Data space frame format
 * 1   2   3       4  5     ...   last
 * [ + P + Type + Offset + DATA + ]
 * Character Database frame format
 */
unsigned char bufferDeal(unsigned char *udpData, unsigned int length)
{
	if('[' == udpData[0])
	{
		// Erase all the character database
		if('E' == udpData[1])
		{
			
		}
		// Flash-programming command. transferring character database.
		if('P' == udpData[1]) 
		{
			unsigned char type = udpData[2];
			unsigned long pageOffset = udpData[3] | (udpData[4] << 8);
			switch(type)
			{
				case 1:pageOffset += ASC16_OFFSET;break;
				case 2:pageOffset += ASC24_OFFSET;break;
				case 3:pageOffset += HZK16_OFFSET;break;
				case 4:pageOffset += HZK24_OFFSET;break;
			}
			ExtFlashPageProgram(pageOffset, &udpData[5],length - 6);
			//ExtFlashPageDataRead(pageOffset, buffer, length - 6);
			//makeUdpPacket(length - 6, networkBuffer, buffer);
		}else if('D' == udpData[1])
		{
			if(udpData[2])
			{
				info.DisplayMode = SCROLL;
			}else{
				info.DisplayMode = FULLSCR;
			}
			
			if (udpData[3])
			{
				info.FontSize = DOT24;
			}else{
				info.FontSize = DOT16;
			}
			
			if (udpData[4])
			{
				info.isBorder = NOBORDER;
			}else{
				info.isBorder = BORDER;
			}
			// Set the color
			info.CharacColor = udpData[5];
			info.BackgdColor = udpData[6];
			
			// Set the StartRow and StartLow
			info.StartRow = udpData[7];
			info.StartCol = udpData[8];
			
			// Put all the data into the data buffer memory
			for(int i = 0;i<length - 10;i++)
			{
				characBuffer[i] = udpData[i + 9];
			}
			
			info.CharacLength = length - 10;
			info.isChanged = 1;
		}		
	}
	return 1;
}