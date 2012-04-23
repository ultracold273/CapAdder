/*
 * SPI_en28j60.c
 *
 * Created: 2011/12/8 10:08:30
 *  Author: ultracold
 */ 
#include <avr/io.h>
#include "SPI_enc28j60.h"
#include "nwkStack.h"

/* MCU Specific Code */
void SPI_MasterInit(void)
{
	// SPI Mode Enable, Master Mode, SPI mode(0,0), clk/16 transmission rate.
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	DDRB |= (1<<PB4) | (1<<PB5) | (1<<PB7);
}

void enc28j60_enable(void)
{
	PORTB &= ~(1<<PB4);	// ENC28J60 CS_n connected to PB4 
}

void enc28j60_disable(void)
{
	PORTB |= (1<<PB4);	// ENC28J60 CS_n connected to PB4 
}

void enc28j60_resetDisable(void)
{
	DDRD |= (1<<PD7);
	PORTD |= (1<<PD7);
}

void enc28j60_resetEnable(void)
{
	DDRD |= (1<<PD7);
	PORTD &= ~(1<<PD7);
}

unsigned char SPI_MasterTransmit(unsigned char cData)
{
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

/* enc28j60 Drivers */
unsigned char enc28j60_ReadOperation(unsigned char op, unsigned char addr)
{
	unsigned char data;
	enc28j60_enable();
	SPI_MasterTransmit(op | (addr & ADDR_MASK));
	data = SPI_MasterTransmit(0xFF);
	if(addr & 0x80)
	{
		data = SPI_MasterTransmit(0xFF);
	}
	enc28j60_disable();
	return data;
}

void enc28j60_WriteOperation(unsigned char op, unsigned char addr, unsigned char data)
{
	enc28j60_enable();
	SPI_MasterTransmit(op | (addr & ADDR_MASK));
	SPI_MasterTransmit(data);
	enc28j60_disable();
}

void enc28j60_ReadBufferMemory(unsigned int len, unsigned char *dpt)
{
	enc28j60_enable();
	SPI_MasterTransmit(ENC28J60_READ_BUF_MEM);
	while(len)
	{
		len--;
		*dpt = SPI_MasterTransmit(0xFF);
		dpt++;
	}
	*dpt = '\0';
	enc28j60_disable();
}

void enc28j60_WriteBufferMemory(unsigned int len, unsigned char *dpt)
{
	enc28j60_enable();
	SPI_MasterTransmit(ENC28J60_WRITE_BUF_MEM);
	while(len)
	{
		len--;
		SPI_MasterTransmit(*dpt);
		dpt++;
	}
	enc28j60_disable();
}

void enc28j60_setBank(unsigned char addr)
{
	static unsigned char currentBank;
	if((addr & BANK_MASK) != currentBank)
	{
		enc28j60_WriteOperation(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL0 | ECON1_BSEL1);
		enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON1, (addr & BANK_MASK) >> 5);
		currentBank = addr & BANK_MASK;
	}
}

void enc28j60_SystemReset(void)
{
	enc28j60_enable();
	SPI_MasterTransmit(0xFF);
	enc28j60_disable();
}

unsigned char enc28j60_ReadControlRegister(unsigned char addr)
{
	enc28j60_setBank(addr);
	return enc28j60_ReadOperation(ENC28J60_READ_CTRL_REG, addr);
}

void enc28j60_WriteControlRegister(unsigned char addr, unsigned char command)
{
	enc28j60_setBank(addr);
	enc28j60_WriteOperation(ENC28J60_WRITE_CTRL_REG, addr, command);
}

unsigned int enc28j60_ReadPhyRegister(unsigned char addr)
{
	unsigned int returnData;
	enc28j60_WriteControlRegister(MIREGADR, addr);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, MICMD, MICMD_MIIRD);
	while(enc28j60_ReadControlRegister(MISTAT) & MISTAT_BUSY);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_CLR, MICMD, MICMD_MIIRD);
	returnData = enc28j60_ReadControlRegister(MIRDL);
	returnData |= (enc28j60_ReadControlRegister(MIRDH))<<8;
	return returnData;
}

void enc28j60_WritePhyRegister(unsigned char addr, unsigned int data)
{
	enc28j60_WriteControlRegister(MIREGADR, addr);
	enc28j60_WriteControlRegister(MIWRL, data);
	enc28j60_WriteControlRegister(MIWRH, data>>8);
	while(enc28j60_ReadControlRegister(MISTAT) & MISTAT_BUSY);
}

static unsigned int nextPacketPt;

void enc28j60_init(void)
{
	enc28j60_resetDisable();
	enc28j60_SystemReset();
	nextPacketPt = RECEIVEST;
	// BANK 0 Registers
	// Set the Receive Buffer Start Address
	enc28j60_WriteControlRegister(ERXSTL, RECEIVEST & 0xFF);
	enc28j60_WriteControlRegister(ERXSTH, RECEIVEST >> 8);
	// Set the Receive Buffer End Address
	enc28j60_WriteControlRegister(ERXNDL, RECEIVEED & 0xFF);
	enc28j60_WriteControlRegister(ERXNDH, RECEIVEED >> 8);
	// Set the Receive Read Buffer Pointer ( forbid hardware write )
	enc28j60_WriteControlRegister(ERXRDPTL, RECEIVEST & 0xFF);
	enc28j60_WriteControlRegister(ERXRDPTH, RECEIVEST >> 8);
	// Set the Transmit Buffer Start Address
	enc28j60_WriteControlRegister(ETXSTL, TRANSMITST & 0xFF);
	enc28j60_WriteControlRegister(ETXSTH, TRANSMITST >> 8);
	// Set the Transmit Buffer End Address (need to be redefined in transmitting real packets)
	enc28j60_WriteControlRegister(ETXNDL, TRANSMITED & 0xFF);
	enc28j60_WriteControlRegister(ETXNDH, TRANSMITED >> 8);
	// DMA Controller initialization??
	
	// BANK 1 Registers
	// The Ethernet Controller only accept Unicast packet targeted to its MAC Address
	// And Broadcast packet of ARP type
	// Enable Unicast bit & CRC bit & pattern match bit
	// ATTENTION: THIS HAS BEEN MODIFIED CANCELL THE PMEN SET BCEN
	enc28j60_WriteControlRegister(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
	// In order to use Pattern Match Filter. We must configure EPMOH:EPMOL, EPMM7:EPMM0 and
	// EPMCSH:EPMCSL Registers.
	// As all the bits used to match locate in the first 64 bits, it's OK to use the default
	// value of EPMOH:EPMOL(OFFSET)
	// We only need to match the Dst ADDR(FF FF FF FF FF FF)(First 6 bytes), and Type (08 06)
	// (13th, 14th bytes), the Musk Register should be 0011 0000 0011 1111
	// ATTENTION: BELOW HAS BEEN DEPRECATED.
	enc28j60_WriteControlRegister(EPMM0, 0x3F);
	enc28j60_WriteControlRegister(EPMM1, 0x30);
	// IP checksum of the selected bits are EPMCSH:EPMCSL = F7F9
	enc28j60_WriteControlRegister(EPMCSL, 0xF9);
	enc28j60_WriteControlRegister(EPMCSH, 0xF7);
	
	// BANK 2 Registers
	// Allow MAC Module operate in Full-duplex Mode, thus should enable it to TX & RX pause
	// frame.
	enc28j60_WriteControlRegister(MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
	// Clear MACON2.MARST to make MAC Module quit the rest state.
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_CLR, MACON2, MACON2_MARST);
	// Let Device automatically add paddings to 60 bytes, enable CRC and Frame Length Check
	// Set in the Full-duplex mode
	enc28j60_WriteControlRegister(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	// Set the Frame Maximum Length
	enc28j60_WriteControlRegister(MAMXFLL, MAX_FRAME_LENGTH & 0xFF);
	enc28j60_WriteControlRegister(MAMXFLH, MAX_FRAME_LENGTH >> 8);
	// Set the Back-to-Back Intra-packet Gap Register (Recommended Value for FULLDX is 15H)
	enc28j60_WriteControlRegister(MABBIPG, 0x15);
	// Set the Intra-packet Gap Register (Rcmd Vaue is 0C12H)
	enc28j60_WriteControlRegister(MAIPGL, 0x12);
	enc28j60_WriteControlRegister(MAIPGH, 0x0C);
	
	// BANK 3 Registers
	// Set the Device's MAC address
	enc28j60_WriteControlRegister(MAADR0, ENC28J60_MADDR0);
	enc28j60_WriteControlRegister(MAADR1, ENC28J60_MADDR1);
	enc28j60_WriteControlRegister(MAADR2, ENC28J60_MADDR2);
	enc28j60_WriteControlRegister(MAADR3, ENC28J60_MADDR3);
	enc28j60_WriteControlRegister(MAADR4, ENC28J60_MADDR4);
	enc28j60_WriteControlRegister(MAADR5, ENC28J60_MADDR5);
	
	
	// Physical Register Initialization
	enc28j60_WritePhyRegister(PHCON1, PHCON1_PDPXMD);
	enc28j60_WritePhyRegister(PHCON2, PHCON2_HDLDIS);
	
	// Change Bank to Bank 0
	enc28j60_setBank(ECON1);
	// Enable packet receive interrupt
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
	// Enable Receive Function and DMA Check Sum Calculation
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN | ECON1_CSUMEN);
}

unsigned char enc28j60_getEthernetRevisionID(void)
{
	return enc28j60_ReadControlRegister(EREVID);
}

void enc28j60_sendPacket(unsigned int length, unsigned char *packetOut, unsigned char hdrChecksumFlag)
{
	// Initialize the Write Pointer
	enc28j60_WriteControlRegister(EWRPTL, TRANSMITST & 0xFF);
	enc28j60_WriteControlRegister(EWRPTH, TRANSMITST >> 8);
	// Set the TXND to the actual data end
	unsigned int dataEnd = TRANSMITST + length;
	enc28j60_WriteControlRegister(ETXNDL, dataEnd & 0xFF);
	enc28j60_WriteControlRegister(ETXNDH, dataEnd >> 8);
	// First write the Control byte to the first transmission buffer byte
	unsigned char cmdByte = 0x0E;
	enc28j60_WriteBufferMemory(1, &cmdByte);
	// Put all the data into the Buffer Memory
	enc28j60_WriteBufferMemory(length, packetOut);
	// Using Hardware checkSum calculation
	if(hdrChecksumFlag)
	{
		if (hdrChecksumFlag & HDR_CHECK_IP_MASK)
		{
			unsigned int ipCksum = enc28j60_hardwareChecksum(IP_P, 20);
			enc28j60_WriteControlRegister(EWRPTL, (TRANSMITST + 1 + IP_HEADER_CHECKSUM) & 0xFF);
			enc28j60_WriteControlRegister(EWRPTH, (TRANSMITST + 1 + IP_HEADER_CHECKSUM) >> 8);
			enc28j60_WriteBufferMemory(2, (unsigned char *)(&ipCksum));
		}
		if (hdrChecksumFlag & HDR_CHECK_TRANSLY_MASK)
		{
			unsigned int cksum = enc28j60_hardwareChecksum(TCP_P, length - TCP_P);
			// Pseudo-Header Checksum Calculation
			unsigned int pseHdCksumH = 0;
			unsigned int pseHdCksumL = 0;
			unsigned int pseHdChsumReal;
			for(int i = 0;i < 8;i += 2)
			{
				pseHdCksumH += packetOut[IP_SRC_ADDR + i];
				pseHdCksumL += packetOut[IP_SRC_ADDR + i + 1];
			}
			pseHdCksumL += packetOut[IP_PROTOCOL];
			pseHdCksumH += ((length - TCP_P) >> 8);
			pseHdCksumL += ((length - TCP_P) & 0xFF);
			pseHdCksumH += pseHdCksumL >> 8;
			pseHdChsumReal = ((pseHdCksumH & 0xFF) << 8) | (pseHdCksumL & 0xFF);
			pseHdChsumReal += (pseHdCksumH >> 8);
			pseHdChsumReal = ((pseHdChsumReal & 0xFF) << 8) | (pseHdChsumReal >> 8);

			cksum ^= 0xFFFF;

			cksum += pseHdChsumReal;

			if(cksum < pseHdChsumReal)
			{
				cksum++;
			}
			
			cksum ^= 0xFFFF;

			if(hdrChecksumFlag & HDR_CHECK_TRANSLY_TYPE_MASK) // UDP type
			{
				enc28j60_WriteControlRegister(EWRPTL, (TRANSMITST + 1 + UDP_CHECKSUM) & 0xFF);
				enc28j60_WriteControlRegister(EWRPTH, (TRANSMITST + 1 + UDP_CHECKSUM) >> 8);
				enc28j60_WriteBufferMemory(2, (unsigned char *)(&cksum));
			}else{
				enc28j60_WriteControlRegister(EWRPTL, (TRANSMITST + 1 + TCP_CHECKSUM) & 0xFF);
				enc28j60_WriteControlRegister(EWRPTH, (TRANSMITST + 1 + TCP_CHECKSUM) >> 8);
				enc28j60_WriteBufferMemory(2, (unsigned char *)(&cksum));
			}			
		}
	}
	// Set the ECON1.TXRTS to initiate the transmission
	enc28j60_setBank(ECON1);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXIF);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	// If the transmission is failed, try again
	if(enc28j60_ReadControlRegister(EIR) & EIR_TXERIF)
	{
		enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	}
}

unsigned int enc28j60_receivePacket(unsigned int length, unsigned char *packetIn)
{
	// Check whether there exists unread packets
	if(enc28j60_ReadControlRegister(EPKTCNT) == 0)
	{
		return 0;
	}
	
	// Write next packet address to Read Register
	enc28j60_WriteControlRegister(ERDPTL, nextPacketPt & 0xFF);
	enc28j60_WriteControlRegister(ERDPTH, nextPacketPt >> 8);
	// Get the extra message generated by controller
	unsigned char data[7];
	enc28j60_ReadBufferMemory(6,data);
	// Read the next packet address
	nextPacketPt = data[0];
	nextPacketPt |= (data[1]<<8);
	// Get the actual length of the packet
	unsigned int actualLen = data[2];
	actualLen |= (data[3]<<8);
	// Other status
	unsigned int rsv = data[4];
	rsv |= (data[5] << 8);
	
	// Remove the CRC part
	actualLen -= 4;
	if(actualLen > length)
	{
		actualLen = length;
	}
	// Read the packet
	enc28j60_ReadBufferMemory(actualLen, packetIn);
	// Move Forward the ERXRDPT
	enc28j60_WriteControlRegister(ERXRDPTL, nextPacketPt & 0xFF);
	enc28j60_WriteControlRegister(ERXRDPTH, nextPacketPt >> 8);
	
	// Decrease the packet counter
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return actualLen;
}

unsigned int enc28j60_hardwareChecksum(unsigned int offsetToTransSt, unsigned int length)
{
	unsigned int checksum;
	enc28j60_WriteControlRegister(EDMASTL, (TRANSMITST + 1 + offsetToTransSt) & 0xFF);
	enc28j60_WriteControlRegister(EDMASTH, (TRANSMITST + 1 + offsetToTransSt) >> 8);
	enc28j60_WriteControlRegister(EDMANDL, (TRANSMITST + offsetToTransSt + length) & 0xFF);
	enc28j60_WriteControlRegister(EDMANDH, (TRANSMITST + offsetToTransSt + length) >> 8);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_CLR, EIR, EIR_DMAIF);
	enc28j60_WriteOperation(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_DMAST);
	while(!(enc28j60_ReadControlRegister(EIR) & EIR_DMAIF));
	checksum = enc28j60_ReadControlRegister(EDMACSH);
	checksum |= enc28j60_ReadControlRegister(EDMACSL) << 8;
	return checksum;
}