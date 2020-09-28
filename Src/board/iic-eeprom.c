#include "eeprom.h"
#include "iic.h"
 
/*
*********************************************************************************************************
*
*********************************************************************************************************
*/
uint8_t FT24C64A_CheckOk(void)
{
	if (I2C_CheckDevice(EE_DEV_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		I2C_Stop();		
		return 0;
	}
}
 
/******************************************************************
*	
********************************************************************/
uint8_t FT24C64A_ReadRandomBytes(uint8_t *buf, uint16_t addr, uint16_t size)
{
	uint16_t i;
	uint8_t ret;
	
	I2C_Start();

	ret |= I2C_SendByte(EE_DEV_ADDR|I2C_WR);	
	ret |= I2C_SendByte((uint8_t)addr/EE_PAGE_NUM);
	ret |= I2C_SendByte((uint8_t)addr%EE_PAGE_NUM);
	
	if( ret != 0 )
	{
		DebugPrintf("Random Read Failed. \r\n");
		return 0;
	}
	
	I2C_Start();
	
	ret |= I2C_SendByte(EE_DEV_ADDR | I2C_RD);	
	if( ret != 0 )
	{
		DebugPrintf("Random Read Failed. \r\n");
		return 0;
	}
	
	for (i = 0; i < size; i++)
	{
		buf[i] = I2C_RecvByte();	
		
		if (i != size - 1)
		{
			I2C_SendACK(0);	
		}
		else
		{
			I2C_SendACK(1);	
		}
	}
	I2C_Stop();
	return 1;	
}
 
 uint8_t FT24C64A_WritePages(uint8_t *buf, uint16_t addr, uint16_t size)
{
	uint16_t i;
	uint16_t usAddr;
	uint16_t ret=0;
	
	for (i = 0; i < size; i++)
	{
		if ((i == 0) || (addr & (EE_PAGE_SIZE - 1)) == 0)
		{
			I2C_Start();
			ret |= I2C_SendByte(0xa0);
			ret |= I2C_SendByte(addr/EE_PAGE_NUM);
			ret |= I2C_SendByte(addr%EE_PAGE_NUM);				
		}	
		ret |= I2C_SendByte(buf[i]); 
		addr++;	
	}
	
	I2C_Stop();
	
	if( ret != 0 )
	{
		printf("Write fild!\r\n");
		return 0;
	}
	return 1;	
}
 
