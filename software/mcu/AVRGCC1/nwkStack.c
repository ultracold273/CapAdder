/*
 * nwkStack.c
 * 
 * This file implement a simple TCP/IP stack, in order to provide ARP service, and 
 * basic TCP service without any flow and congestion control, just the necessary 
 * part to implement a simple web-page transmission.
 *
 * Created: 2012/2/17 14:28:22
 *  Author: ultracold
 */ 

#include "nwkStack.h"
#include "SPI_enc28j60.h"

/*
 * Ethernet function
 */

void makeEthernetHeader(unsigned char *buf)
{
	unsigned char i;
	for(i = 0;i<6;i++)
	{
		buf[DST_MAC_ADDR_ST + i] = buf[SRC_MAC_ADDR_ST + i];
	}

	buf[SRC_MAC_ADDR_ST] = ENC28J60_MADDR5;
	buf[SRC_MAC_ADDR_ST + 1] = ENC28J60_MADDR4;
	buf[SRC_MAC_ADDR_ST + 2] = ENC28J60_MADDR3;
	buf[SRC_MAC_ADDR_ST + 3] = ENC28J60_MADDR2;
	buf[SRC_MAC_ADDR_ST + 4] = ENC28J60_MADDR1;
	buf[SRC_MAC_ADDR_ST + 5] = ENC28J60_MADDR0;
}

// Network Layer

/*
 * Address Resolution Protocol(ARP) Service
 * As a server, a configured IP address, so we should only accept ARP request packet
 * and then send back an ARP acknowledge packet contains the MAC address of 
 *
 */

/* 0 for false, 1 for true */
unsigned char isArpAndTargetMyIp(unsigned char *buf)
{
	unsigned int type = 0x00;
	type = (buf[NWK_TYPE_ST] << 8) | buf[NWK_TYPE_ST + 1];
	if(type != 0x0806)	// ARP Character Type
	{
		return 0;
	}
	if(buf[ARP_TPA] != IPADDR0 || buf[ARP_TPA + 1] != IPADDR1 ||
		buf[ARP_TPA + 2] != IPADDR2 || buf[ARP_TPA + 3] != IPADDR3)
		{
			return 0;
		}
	return 1;
}

/* 
 * Before calling this function, you must ensure that the buf stores an arp packet
 * So it is better to call isArpAndTargetMyIp() first
 */
void sendArpAnswers(unsigned char *buf)
{
	// Hardware Type for Ethernet is 0x0001
	buf[ARP_HTYPE_ST] = 0x00;
	buf[ARP_HTYPE_ST + 1] = 0x01;
	// Protocol Type for IP is 0x0800
	buf[ARP_PTYPE_ST] = 0x08;
	buf[ARP_PTYPE_ST + 1] = 0x00;
	// Hardware size is 0x06
	buf[ARP_HLEN] = 0x06;
	// Protocol size is 0x04
	buf[ARP_PLEN] = 0x04;
	// Operational Code for ArpAnswer
	buf[ARP_OPER] = 0x00;
	buf[ARP_OPER + 1] = 0x02;
	// First copy sender Mac & ip addr to dest Mac & ip addr
	unsigned char a = 0;
	while(a < 10)
	{
		buf[ARP_THA + a] = buf[ARP_SHA + a];
		a++;
	}
	// Put own device address to the specific field
	buf[ARP_SHA] = ENC28J60_MADDR5;
	buf[ARP_SHA + 1] = ENC28J60_MADDR4;
	buf[ARP_SHA + 2] = ENC28J60_MADDR3;
	buf[ARP_SHA + 3] = ENC28J60_MADDR2;
	buf[ARP_SHA + 4] = ENC28J60_MADDR1;
	buf[ARP_SHA + 5] = ENC28J60_MADDR0;
	buf[ARP_SPA] = IPADDR0;
	buf[ARP_SPA + 1] = IPADDR1;
	buf[ARP_SPA + 2] = IPADDR2;
	buf[ARP_SPA + 3] = IPADDR3;
	makeEthernetHeader(buf);
	enc28j60_sendPacket(42, buf, HDR_CHECK_ARP);
}

/*
 * Internet Protocol(IP) Service
 * 
 */

/* 0 for false, 1 for true */
unsigned char isIpPacketAndTargetMyIp(unsigned char *buf)
{
	unsigned int type = 0x00;
	type = (buf[NWK_TYPE_ST] << 8) | (buf[NWK_TYPE_ST + 1] & 0xFF);
	if(type != 0x0800)	// IP Character Code
	{
		return 0;
	}
	if (buf[IP_DST_ADDR] != IPADDR0 || buf[IP_DST_ADDR + 1] != IPADDR1
		|| buf[IP_DST_ADDR + 2] != IPADDR2 || buf[IP_DST_ADDR + 3] != IPADDR3)
	{
		return 0;
	}
	return 1;
}

unsigned int getIpPacketLength(unsigned char *buf)
{
	if(isIpPacketAndTargetMyIp(buf))
	{
		return (buf[IP_TOTAL_LENGTH] << 8) | buf[IP_TOTAL_LENGTH + 1];
	}
	return 0;
}

void sendIpPacket(unsigned int dataLen, unsigned char *buf, unsigned char type)
{
	static unsigned int ipID = 0x00;
	// Generally the version is 0x40 and the length is 5 words (20 Bytes and 160 bits)
	buf[IP_VERSION_LENGTH] = 0x45;
	// No Differentiated Service Code Point and Explicit Congestion Notification.
	// These usually used by new coming technology like multimedia or VoIP which requires 
	// real-time transmission.
	buf[IP_DSCP_ECN] = 0x00;
	// Total length (MAY BE MODIFIED)
	buf[IP_TOTAL_LENGTH] = (dataLen + 20) >> 8;
	buf[IP_TOTAL_LENGTH + 1] = (dataLen + 20) & 0xFF;
	// Identification 
	buf[IP_IDENTIFICATION] = ipID >> 8;
	buf[IP_IDENTIFICATION + 1] = ipID & 0xFF;
	ipID++;
	// Flags and Segment Offset. Flag for not fragment and no fragments allowed for convenience.
	buf[IP_FLAG_FRAGOFF] = 0x40;
	buf[IP_FLAG_FRAGOFF + 1] = 0x00;
	// Time to Live in this application should be greater than 1.
	buf[IP_TTL] = 0x40;
	// Protocol may not be modified, for we always return the same protocol as the incoming 
	// packet.
	buf[IP_PROTOCOL] = type;
	// First deposit IP address
	for(int i = 0;i<4;i++)
	{
		buf[IP_DST_ADDR + i] = buf[IP_SRC_ADDR + i];
	}
	buf[IP_SRC_ADDR] = IPADDR0;
	buf[IP_SRC_ADDR + 1] = IPADDR1;
	buf[IP_SRC_ADDR + 2] = IPADDR2;
	buf[IP_SRC_ADDR + 3] = IPADDR3;
	// Clear the Header Checksum
	buf[IP_HEADER_CHECKSUM] = 0x00;
	buf[IP_HEADER_CHECKSUM + 1] = 0x00;
	//unsigned int ck = checkSum(&buf[IP_P], 20, IP);
	//buf[IP_HEADER_CHECKSUM] = ck & 0xFF;
	//buf[IP_HEADER_CHECKSUM + 1] = ck >> 8;
	makeEthernetHeader(buf);
	// LENGTH SHOULD BE MODIFIED
	if(type == 0x06)	// TCP's protocol in IP packet is 6
	{
		enc28j60_sendPacket(dataLen + 20 + 14, buf, HDR_CHECK_TCP);
	}else if(type == 0x11)	// UDP's protocol in IP packet is 17
	{
		enc28j60_sendPacket(dataLen + 20 + 14, buf, HDR_CHECK_UDP);	
	}	
}

// Transmission Layer

/*
 * User Diagram Protocol (UDP) Service
 *
 *
 */

unsigned char isUdpPacket(unsigned char *buf)
{
	if(isIpPacketAndTargetMyIp(buf))
	{
		if(buf[IP_PROTOCOL] == 17)
		{
			return 1;
		}
	}
	return 0;
}

unsigned int getUdpPort(unsigned char *buf)
{
	unsigned int port;
	if(isUdpPacket(buf))
	{
		port = (buf[UDP_DST_PORT] << 8) | buf[UDP_DST_PORT + 1];
		return port;
	}
	return 0;
}

unsigned char *getUdpDataPointer(unsigned char *buf)
{
	return &buf[UDP_DATA];
}

void makeUdpPacket(unsigned int dataLen, unsigned char *buf, unsigned char *dataSrc)
{
	unsigned int srcPort = (buf[UDP_DST_PORT] << 8) | buf[UDP_DST_PORT + 1];
	buf[UDP_DST_PORT] = buf[UDP_SRC_PORT];
	buf[UDP_DST_PORT + 1] = buf[UDP_SRC_PORT + 1];
	buf[UDP_SRC_PORT] = srcPort >> 8;
	buf[UDP_SRC_PORT + 1] = srcPort & 0xFF;
	buf[UDP_LENGTH] = (dataLen + 8) >> 8;
	buf[UDP_LENGTH + 1] = (dataLen + 8) & 0xFF;
	buf[UDP_CHECKSUM] = 0x00;
	buf[UDP_CHECKSUM + 1] = 0x00;
	unsigned int i = 0;
	for(i = 0;i < dataLen;i++)
	{
		buf[UDP_DATA + i] = *dataSrc;
		dataSrc++;
	}
	//unsigned int ck = checkSum(&buf[], ..,UDP);
	//buf[UDP_CHECKSUM] = ck >> 8;
	//buf[UDP_CHECKSUM + 1] = ck & 0xFF;
	sendIpPacket(dataLen + 8, buf, 0x11);
}

/*
 * Transmission Control Protocol (TCP) Service
 *
 *
 */

unsigned char isTcpPacket(unsigned char *buf)
{
	if(isIpPacketAndTargetMyIp(buf))
	{
		if(buf[IP_PROTOCOL] == 6)
		{
			return 1;
		}
	}
	return 0;
}

unsigned char isTcpAndSyn(unsigned char *buf)
{
	if(isTcpPacket(buf))
	{
		if((buf[TCP_FLAGS] & TCP_FLAGS_SYN_MASK) && !(buf[TCP_FLAGS] & TCP_FLAGS_ACK_MASK))
		{
			return 1;
		}
	}
	return 0;
}

unsigned char isTcpAndFinAck(unsigned char *buf)
{
	if(isTcpPacket(buf))
	{
		if((buf[TCP_FLAGS] & TCP_FLAGS_FIN_MASK) && (buf[TCP_FLAGS] & TCP_FLAGS_ACK_MASK))
		{
			return 1;
		}
	}
	return 0;
}

unsigned char isTcpAndRequireData(unsigned char *buf)
{
	if(isTcpPacket(buf))
	{
		if(buf[TCP_FLAGS] & TCP_FLAGS_PSH_MASK)
		{
			return 1;
		}
	}
	return 0;
}

unsigned int getTcpPort(unsigned char *buf)
{
	unsigned char port;
	if(isTcpPacket(buf))
	{
		port = (buf[TCP_DST_PORT] << 8) | buf[TCP_DST_PORT + 1];
		return port;
	}
	return 0;	
}

unsigned char *getTcpDataPointer(unsigned char *buf)
{
	unsigned char dataOffsetIfOptionUsed;
	dataOffsetIfOptionUsed = (buf[TCP_DATA_OFFSET] >> 2) - 20;
	return &buf[TCP_DATA + dataOffsetIfOptionUsed];
}

void makeTcpPacket(unsigned char *buf, unsigned char *dataIn, unsigned int dataLength)
{
	unsigned int i;
	for(i = 0;i<dataLength;i++)
	{
		buf[TCP_DATA + i] = dataIn[i];
	}
}

void makeTcpHeader(unsigned char *buf, unsigned char flags, unsigned int tcpPacketLength, unsigned char mss)
{
	static unsigned int seqNum = 0x000A;
	unsigned int seqDupNumL, seqDupNumH;
	unsigned long ackNum = 0L;
	unsigned char dstPortH = buf[TCP_SRC_PORT];
	unsigned char dstPortL = buf[TCP_SRC_PORT + 1];
	//unsigned int dstPort = (buf[TCP_SRC_PORT] << 8) | buf[TCP_SRC_PORT + 1];
	buf[TCP_SRC_PORT] = buf[TCP_DST_PORT];
	buf[TCP_SRC_PORT + 1] = buf[TCP_DST_PORT + 1];
	buf[TCP_DST_PORT] = dstPortH;
	buf[TCP_DST_PORT + 1] = dstPortL;
	//buf[TCP_DST_PORT] = dstPort >> 8;
	//buf[TCP_DST_PORT + 1] = dstPort & 0xFF;
	seqDupNumL = seqNum;
	seqDupNumH = (dstPortH << 8) | dstPortL;
	ackNum = (buf[TCP_SEQ_NUM] << 8) | buf[TCP_SEQ_NUM + 1];
	ackNum <<= 16;
	ackNum |= (buf[TCP_SEQ_NUM + 2] << 8) | buf[TCP_SEQ_NUM + 3];
	ackNum++;
	//if(ackNumL == 0) ackNumH++;
	seqNum++;
	// Fill the Sequence Number
	if (buf[TCP_FLAGS] & TCP_FLAGS_ACK_MASK)
	{
		buf[TCP_SEQ_NUM] = buf[TCP_ACK_NUM];
		buf[TCP_SEQ_NUM + 1] = buf[TCP_ACK_NUM + 1];
		buf[TCP_SEQ_NUM + 2] = buf[TCP_ACK_NUM + 2];
		buf[TCP_SEQ_NUM + 3] = buf[TCP_ACK_NUM + 3];
	}else
	{
		buf[TCP_SEQ_NUM] = seqDupNumH >> 8;
		buf[TCP_SEQ_NUM + 1] = seqDupNumH & 0xFF;
		buf[TCP_SEQ_NUM + 2] = seqDupNumL >> 8;
		buf[TCP_SEQ_NUM + 3] = seqDupNumL & 0xFF;
	}
	// Fill in the Acknowledge number
	for(int i = 3;i >= 0;i--)
	{
		buf[TCP_ACK_NUM + i] = ackNum & 0xFF;
		ackNum >>= 8;
	}
	/*
	buf[TCP_ACK_NUM] = ackNumH >> 8;
	buf[TCP_ACK_NUM + 1] = ackNumH & 0xFF;
	buf[TCP_ACK_NUM + 2] = ackNumL >> 8;
	buf[TCP_ACK_NUM + 3] = ackNumL & 0xFF;
	*/
	// default is 20 byte TCP Header. But can be changed.
	if(mss)
	{
		buf[TCP_DATA_OFFSET] = 0x60;
		buf[TCP_OPTIONS] = 0x02;
		buf[TCP_OPTIONS + 1] = 0x04;
		buf[TCP_OPTIONS + 2] = 0x05;
		buf[TCP_OPTIONS + 3] = 0xB4;
	}else{
		buf[TCP_DATA_OFFSET] = 0x50;
	}	
	buf[TCP_FLAGS] = flags;
	buf[TCP_URGENT_POINTER] = 0x00;
	// Clear the checksum
	buf[TCP_CHECKSUM] = 0x00;
	buf[TCP_CHECKSUM + 1] = 0x00;
	//unsigned int ck = checkSum();
	//buf[TCP_CHECKSUM] = ck >> 8;
	//buf[TCP_CHECKSUM + 1] = ck & 0xFF;
	// 0x06 is the type definition of TCP in IP
	sendIpPacket(tcpPacketLength, buf, 0x06);
}
