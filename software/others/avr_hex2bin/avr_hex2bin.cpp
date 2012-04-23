// avr_hex2bin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>

unsigned char string2hexbin(unsigned char* in)
{
	unsigned char temp = 0;
	if(in != NULL)
	{
		for(int i = 0;i<2;i++)
		{
			temp <<= 4;
			unsigned char a = (in[i] > '9')?in[i]-'7':in[i]-'0';
			temp |= a;
		}
	}
	return temp;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char in[100] = {0};
	char out[100] = {0};

	printf("Enter the hex filename: \n");
	scanf("%s", in);
	strcpy(out, in);
	int length = strlen(out);
	out[length - 1] = 'n';
	out[length - 2] = 'i';
	out[length - 3] = 'b';
	
	FILE *inFile = fopen(in,"rb");
	FILE *outFile = fopen(out, "wb");

	unsigned char buffer[40];

	if(inFile == NULL || outFile == NULL)
	{
		printf("Cannot open file\n");
		return 1;
	}

	while(!feof(inFile))
	{
		fread(buffer, 1, 9, inFile);
		unsigned char num = string2hexbin(&buffer[1]);
		if(num == 0) break;
		fread(buffer, 1, num*2, inFile);
		for(int i = 0;i < num;i++)
		{
			unsigned char num = string2hexbin(&buffer[i*2]);
			fwrite(&num, 1, 1, outFile);
		}
		fread(buffer, 1, 4, inFile);
	}

	fclose(inFile);
	fclose(outFile);
	return 0;
}

