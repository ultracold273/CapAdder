/*
 * SAADevice.c
 *
 * Created: 2011/10/10 8:52:35
 *  Author: ultracold
 */ 
#include "I2C.h"
#include "SAADevice.h"
#include "USART.h"

void SAA_W(unsigned char addr, unsigned char subaddr, unsigned char data)
{
	I2C_Send_M_Start();
	I2C_Send_M_Addr(addr);
	I2C_Send_M_Data(subaddr);
	I2C_Send_M_Data(data);
	I2C_Send_M_Stop();
}

unsigned char SAA_R(unsigned char addr, unsigned char subaddr)
{
	unsigned char data;
	I2C_Send_M_Start();
	I2C_Send_M_Addr(addr&0xFE);
	I2C_Send_M_Data(subaddr);
	I2C_Send_M_ReStart();
	I2C_Send_M_Addr(addr);
	data = I2C_Send_M_Read();
	I2C_Send_M_Stop();
	return data;
}

void SAA7113_init(void)
{
	//Horizontal increment delay recommended position
	SAA_W(SAA7113_ADDR_W, 0x01, 0x08);
	//CVBS-AI2, Hysteresis update off, amplifier & anti-alias filter on
	SAA_W(SAA7113_ADDR_W, 0x02, 0xC1);//CHANGED FROM 0xC0
	//AGC on, White peak active, Long vertical blanking, line 24 for 50Hz, normal clamping
	SAA_W(SAA7113_ADDR_W, 0x03, 0x23);//CHANGED FROM 0x33
	// Static Gain Control -3dB
	SAA_W(SAA7113_ADDR_W, 0x04, 0x00);
	SAA_W(SAA7113_ADDR_W, 0x05, 0x00);
	// Horizontal sync begin - Recommended value for raw data type??--ref by older version
	SAA_W(SAA7113_ADDR_W, 0x06, 0xEB);//CHANGED FROM 0xE9
	// Horizontal sync stop - Recommended value for raw data type??--ref by older version
	SAA_W(SAA7113_ADDR_W, 0x07, 0xE0);//CHANGED FROM 0x0D
	// Normal vertical noise reduction, PLL closed, Fast locking mode-->VTR mode, FOET off, 50Hz/625lines
	// Automatic field detection
	SAA_W(SAA7113_ADDR_W, 0x08, 0x88);//CHANGED FROM 0x98
	// Aperture factor 0.25, AGC update per line, VBLB active, Center freq 4.1Mhz,
	// Pre-filter bypassed, BYPS active
	SAA_W(SAA7113_ADDR_W, 0x09, 0x01);//NOT THE SAME WITH THE OLDER VERSION
	// Luminance brightness 128(CCIR level)
	SAA_W(SAA7113_ADDR_W, 0x0A, 0x80);
	// Luminance contrast control 1.109(CCIR level)
	SAA_W(SAA7113_ADDR_W, 0x0B, 0x47);
	// Chrominance saturation control 1.0(CCIR level)
	SAA_W(SAA7113_ADDR_W, 0x0C, 0x40);
	// Chrominance hue control 0 DEG
	SAA_W(SAA7113_ADDR_W, 0x0D, 0x00);
	// Nominal chrominance bandwidth, nominal time constant,
	// Chrominance comb filter on, PAL BGHIN mode, DTO disabled
	SAA_W(SAA7113_ADDR_W, 0x0E, 0x01);
	// Automatic chrominance gain control on(MSB=0), other bits are dummy in this situation
	SAA_W(SAA7113_ADDR_W, 0x0F, 0x2A);//NOT THE SAME -- OLDER: Programmable CG
	// Luminance delay compensation 0, VREF pulse length 286 position refer to data sheet
	// Fine position of HS 0??, Standard ITU 656 output format
	SAA_W(SAA7113_ADDR_W, 0x10, 0x00);
	// Automatic color killer, processed VPO output, RTS0/RTS1/RTCO active
	// Output VPO-bus active, standard horizontal lock indicator, ???
	SAA_W(SAA7113_ADDR_W, 0x11, 0x0C);//NOT THE SAME -- OLDER: Color forced on
	// NOTICE: RTS0/RTS1 settings
	// RTS0-VREF, RTS1-HREF
	SAA_W(SAA7113_ADDR_W, 0x12, 0x7E);//CHANGED FROM 0x01
	// AOUT connected to input AD1(CHANGD FROM intertestp1), AD8-AD1 on VPO7-VPO0
	SAA_W(SAA7113_ADDR_W, 0x13, 0x01);//CHANGED FROM 0x00
	// Start of VGATE pulse, 1st-line2, 2nd-line315, dummy in this situation
	SAA_W(SAA7113_ADDR_W, 0x15, 0x00);
	// Stop of VGATE pulse, 1st-line2, 2nd-line315, dummy in this situation
	SAA_W(SAA7113_ADDR_W, 0x16, 0x00);
	// Provide MSB for 0x15, 0x16, dummy in this situation
	SAA_W(SAA7113_ADDR_W, 0x17, 0x00);
	// Data slicer clock 13.5MHz, amplitude searching active, one framing error allowed
	// Hamming check for 2 bytes after framing code, dependent on data type, 50Hz rate
	SAA_W(SAA7113_ADDR_W, 0x40, 0x02);//NOT THE SAME WITH OLDER VERSION(60Hz)??
	//SAA_W(SAA7113_ADDR_W, 0x40, 0x82);//NOT THE SAME WITH OLDER VERSION(60Hz)??
	// Video component signal, active video region
	for(int i=0x41;i<=0x57;i++)
	{
		SAA_W(SAA7113_ADDR_W, i, 0xFF);
	}
	// Programmable Framing Code
	SAA_W(SAA7113_ADDR_W, 0x58, 0x00);
	// Horizontal offset - Recommended value
	SAA_W(SAA7113_ADDR_W, 0x59, 0x54);
	// Vertical offset - Value for 50Hz 625 lines input
	SAA_W(SAA7113_ADDR_W, 0x5A, 0x07);// NOT THE SAME WITH OLDER VERSION(60Hz)??
	//SAA_W(SAA7113_ADDR_W, 0x5A, 0x0a);// NOT THE SAME WITH OLDER VERSION(60Hz)??

	// invert field indicator(MSB==1),(D4-0x5A,D0~D3-0x59)
	SAA_W(SAA7113_ADDR_W, 0x5B, 0x83);
	// SDID codes - default
	SAA_W(SAA7113_ADDR_W, 0x5E, 0x00);
}

void SAA7121_init(void)
{
	// Disable wide screen signal
	SAA_W(SAA7121_ADDR_W, 0x26, 0x00);
	unsigned char a = SAA_R(SAA7121_ADDR_R, 0x26);
	char temp[3];
	temp[0] = ((a & 0xF0) >> 4) + 0x30;
	temp[1] = (a & 0x0F) + 0x30;
	temp[2] = 0;
	USART_StringPut(&temp);
	SAA_W(SAA7121_ADDR_W, 0x27, 0x00);
	// Start and end point of burst in clock cycles DECCOL on / DECFIS off
	SAA_W(SAA7121_ADDR_W, 0x28, 0xA1);//CHANGED FROM 0x21
	
	SAA_W(SAA7121_ADDR_W, 0x29, 0x1D);
	// Copy guard disabled, 0x2A~0x2D are dummy word
	SAA_W(SAA7121_ADDR_W, 0x2A, 0x80);
	SAA_W(SAA7121_ADDR_W, 0x2B, 0x80);
	SAA_W(SAA7121_ADDR_W, 0x2C, 0x80);
	SAA_W(SAA7121_ADDR_W, 0x2D, 0x80);
	SAA_W(SAA7121_ADDR_W, 0x2E, 0x00); //Disable in D7 & D6
	// Y/Cb/Cr data are straight binary, horizontal & vertical trigger taken from MP
	// input data encoded
	SAA_W(SAA7121_ADDR_W, 0x3A, 0x13);
	a = SAA_R(SAA7121_ADDR_R, 0x3A);
	temp[0] = ((a & 0xF0) >> 4) + 0x30;
	temp[1] = (a & 0x0F) + 0x30;
	temp[2] = 0;
	USART_StringPut(&temp);
	// PAL-B/G mode and data from input ports
	//SAA_W(SAA7121_ADDR_W, 0x5A, 0x3F);// NOT THE SAME WITH OLDER VERSION
	//SAA_W(SAA7121_ADDR_W, 0x5A, 0x2A);// OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x5A, 0x00);// NEWEST VERSION
	// Gain for Cb signal
	SAA_W(SAA7121_ADDR_W, 0x5B, 0x7D);//CHANGED FROM 0xA0
	// Gain for Cr signal
	SAA_W(SAA7121_ADDR_W, 0x5C, 0xAF);//CHANGED FROM 0xD8
	// Black level-Enable odd/even field control from RTCI, BLCKL = 0x2A(w2s = 140 IRE)
	SAA_W(SAA7121_ADDR_W, 0x5D, 0x6A);//CHANGED FROM 0x23
	// Blanking level- Enable subcarrier phase reset from RTCI, BLNNL = 0x2E(w2s = 140 IRE)
	//SAA_W(SAA7121_ADDR_W, 0x5E, 0x6E);//CHANGED FROM 0x35 NOT THE SAME WITH OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x5E, 0x75);// OLDER VERSION
	// Cross reduction at 4.1MHz, blanking level
	//SAA_W(SAA7121_ADDR_W, 0x5F, 0x6E);//NOT THE SAME WITH OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x5F, 0x35);//OLDER VERSION
	// Std control, 864 pixel per line, PAL encoding, std bandwidth for chrominance encoding
	// luminance gain for 100 IRE w-b, PAL switch phase is nominal, DAC in nominal operation
	//SAA_W(SAA7121_ADDR_W, 0x61, 0x06);//NOT THE SAME WITH OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x61, 0x26);//OLDER VERSION
	// Enable RTCE, 100IRE w-b PAL encoding
	SAA_W(SAA7121_ADDR_W, 0x62, 0xAF);//CHANGED from 0x2F
	// Define Subcarrier
	SAA_W(SAA7121_ADDR_W, 0x63, 0xCB);
	SAA_W(SAA7121_ADDR_W, 0x64, 0x8A);
	SAA_W(SAA7121_ADDR_W, 0x65, 0x09);
	SAA_W(SAA7121_ADDR_W, 0x66, 0x2A);
	// No captioning & extended data
	SAA_W(SAA7121_ADDR_W, 0x67, 0x80);//CHANGED FROM 0x00
	SAA_W(SAA7121_ADDR_W, 0x68, 0x80);//MSB carry parity bit
	SAA_W(SAA7121_ADDR_W, 0x69, 0x80);
	SAA_W(SAA7121_ADDR_W, 0x6A, 0x80);
	//
	SAA_W(SAA7121_ADDR_W, 0x6B, 0x20);
	// 0x6C & 0x6D are about RCV1 & RCV2 function, dummy in this situation
	//SAA_W(SAA7121_ADDR_W, 0x6C, 0x01);
	//SAA_W(SAA7121_ADDR_W, 0x6D, 0x30);
	SAA_W(SAA7121_ADDR_W, 0x6C, 0x05);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x6D, 0x20);//OLDER VERSION
	// Vertical blanking in accordance with CCIR 624, phase reset every eight fields
	// interlaced 312.5 lines/field at 50Hz
	//SAA_W(SAA7121_ADDR_W, 0x6E, 0xA0);//NOT THE SAME WITH OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x6E, 0x20);//OLDER VERSION
	// Teletext options
	//SAA_W(SAA7121_ADDR_W, 0x6F, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x6F, 0x14);//OLDER VERSION
	// RCV2 output options, dummy in this situation
	//SAA_W(SAA7121_ADDR_W, 0x70, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x71, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x72, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x70, 0x80);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x71, 0xE8);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x72, 0x10);//OLDER VERSION
	// TTX(teletext) request H start/delay, dummy in this situation(following all)
	//SAA_W(SAA7121_ADDR_W, 0x73, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x74, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x73, 0x42);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x74, 0x03);//OLDER VERSION
	// Vertical sync(RCV1 & RCV2)
	//SAA_W(SAA7121_ADDR_W, 0x75, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x75, 0x03);//OLDER VERSION
	// TTX odd/even request VS/VE
	//SAA_W(SAA7121_ADDR_W, 0x76, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x77, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x78, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x79, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x76, 0x05);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x77, 0x16);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x78, 0x04);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x79, 0x16);//OLDER VERSION
	// TTX active line
	//SAA_W(SAA7121_ADDR_W, 0x7A, 0x00);
	//SAA_W(SAA7121_ADDR_W, 0x7B, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x7A, 0x18);//OLDER VERSION
	SAA_W(SAA7121_ADDR_W, 0x7B, 0x38);//OLDER VERSION
	// TTX hash option
	//SAA_W(SAA7121_ADDR_W, 0x7C, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x7C, 0x40);//OLDER VERSION
	// Disable TTX line
	SAA_W(SAA7121_ADDR_W, 0x7E, 0x00);
	SAA_W(SAA7121_ADDR_W, 0x7F, 0x00);
}