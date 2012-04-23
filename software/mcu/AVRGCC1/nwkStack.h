/*
 * nwkStack.h
 *
 * Created: 2012/2/17 14:28:43
 *  Author: ultracold
 */ 


#ifndef NWKSTACK_H_
#define NWKSTACK_H_

#define IPADDR3 1
#define IPADDR2 15
#define IPADDR1 16
#define IPADDR0 172

/* MACROS SHOW OFFSET IN AN ETHERNET FRAME*/
/* Ethernet Header */
#define DST_MAC_ADDR_ST 0
#define SRC_MAC_ADDR_ST 6
#define NWK_TYPE_ST 12
/* ARP Header */
#define ARP_P ARP_HTYPE_ST
#define ARP_HTYPE_ST 14
#define ARP_PTYPE_ST 16
#define ARP_HLEN 18
#define ARP_PLEN 19
#define ARP_OPER 20
#define ARP_SHA 22
#define ARP_SPA 28
#define ARP_THA 32
#define ARP_TPA 38
/* IP Header */
#define IP_P IP_VERSION_LENGTH
#define IP_VERSION_LENGTH 14
#define IP_DSCP_ECN 15
#define IP_TOTAL_LENGTH 16
#define IP_IDENTIFICATION 18
#define IP_FLAG_FRAGOFF 20
#define IP_TTL 22
#define IP_PROTOCOL 23
#define IP_HEADER_CHECKSUM 24
#define IP_SRC_ADDR 26
#define IP_DST_ADDR 30
/* TCP Header */
#define TCP_P TCP_SRC_PORT
#define TCP_SRC_PORT 34
#define TCP_DST_PORT 36
#define TCP_SEQ_NUM 38
#define TCP_ACK_NUM 42
#define TCP_DATA_OFFSET 46
#define TCP_FLAGS 47
#define TCP_WIN_SIZE 48
#define TCP_CHECKSUM 50
#define TCP_URGENT_POINTER 52
#define TCP_DATA 54
#define TCP_OPTIONS 54
/* UDP Header */
#define UDP_P UDP_SRC_PORT
#define UDP_SRC_PORT 34
#define UDP_DST_PORT 36
#define UDP_LENGTH 38
#define UDP_CHECKSUM 40
#define UDP_DATA 42

/* TCP Flag Mask */
#define TCP_FLAGS_ACK_MASK 0x10
#define TCP_FLAGS_SYN_MASK 0x02
#define TCP_FLAGS_FIN_MASK 0x01
#define TCP_FLAGS_PSH_MASK 0x08


/* Function Declaration */
void makeEthernetHeader(unsigned char *buf);
unsigned char isArpAndTargetMyIp(unsigned char *buf);
void sendArpAnswers(unsigned char *buf);
unsigned char isIpPacketAndTargetMyIp(unsigned char *buf);
unsigned int getIpPacketLength(unsigned char *buf);
void sendIpPacket(unsigned int dataLen, unsigned char *buf, unsigned char type);
unsigned char isUdpPacket(unsigned char *buf);
unsigned int getUdpPort(unsigned char *buf);
unsigned char *getUdpDataPointer(unsigned char *buf);
void makeUdpPacket(unsigned int dataLen, unsigned char *buf, unsigned char *dataSrc);
unsigned char isTcpPacket(unsigned char *buf);
unsigned char isTcpAndSyn(unsigned char *buf);
unsigned char isTcpAndFinAck(unsigned char *buf);
unsigned char isTcpAndRequireData(unsigned char *buf);
unsigned int getTcpPort(unsigned char *buf);
unsigned char *getTcpDataPointer(unsigned char *buf);
void makeTcpPacket(unsigned char *buf, unsigned char *dataIn, unsigned int dataLength);
void makeTcpHeader(unsigned char *buf, unsigned char flags, unsigned int tcpPacketLength, unsigned char mss);

#endif /* NWKSTACK_H_ */