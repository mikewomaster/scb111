#include "function.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

// extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart2;

#define __DEBUG__
#define UART2DEBUG 1

void  DebugPrintf(char  *p_fmt, ...)
{
	#ifndef __DEBUG__
		return;
	#else
		char str[100];
		uint8_t  len;
		va_list vArgs;
		va_start(vArgs, p_fmt);
		vsnprintf((char*)str, 100, (char const *)p_fmt, vArgs);
		va_end(vArgs);
		len = strlen(str);

	#if UART2DEBUG
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_UART_Transmit(&huart2, (uint8_t *)str, len, 2000);
		DelayMs(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	#else
		CDC_Transmit_FS((uint8_t *)str, len);
		DelayMs(100);
	#endif

	#endif
}

#if 0
	void exchangeBytes(unsigned char* byte)
	{
			unsigned char	tmp;
		
			tmp = *(byte+1);
			*(byte+1) = *byte;
			*byte = tmp;
			return;
	}
#endif

unsigned short crc_Compute(unsigned char *PBuf,unsigned char len)
{
      char i;
      unsigned int c_dat=0xffff;//有的CRC校验这个值是0
      for (;len>0;len--)//
      {
        c_dat ^= *PBuf;
       for(i=0;i<8;i++)
        {
          if((c_dat&0x1)==0x1)
          {
            c_dat >>= 1;
            c_dat ^= 0xa001;
          }
          else
          {
            c_dat >>= 1;
          }
        }
        PBuf++;
      }
      return c_dat;
}

int set_value_str(char *set_data, uint16_t num, char *ret_data)
{
	int i = 0;
	char tmp[100]={'\0'};

	if(set_data == NULL || num <= 0 || ret_data == NULL)
		return -1;

	for( i = 0; i < (num/2); i++)
	{
		tmp[i*2+1] = *set_data++;
		tmp[i*2] = *set_data++;
	}

	if((num%2) != 0)
	{
		tmp[i*2+1] = *set_data;
		tmp[i*2] = 0x00;
	}

	memcpy(ret_data, tmp, num);
	return 1;
}

int set_value_uint16(char *set_data, uint16_t num,char *ret_data)
{
	int i = 0;
	char tmp[2]={'\0'};
	
	if(set_data == NULL || num <= 0 || ret_data == NULL)
		return -1;

	for( i = 0; i < num; i++)
		{
		tmp[i] = *set_data++;
		}

	memcpy(ret_data, tmp, num);
	return 1;
}

int set_value_uint32(char *set_data, uint16_t num,char *ret_data)
{
	int i = 0;
	char tmp[4]={'\0'};
	
	if(set_data == NULL || num <= 0 || ret_data == NULL)
		return -1;

	for( i = 0; i < (num/4); i++)
		{
		tmp[i*4+2] = *set_data++;
		tmp[i*4+3] = *set_data++;
		tmp[i*4] = *set_data++;
		tmp[i*4+1] = *set_data++;
		}

	memcpy(ret_data, tmp, num);
	return 1;
}

int get_value_uint32(char *set_data, uint16_t num,char *ret_data)
{
	int i = 0;
	char tmp[4]={'\0'};
	
	if(set_data == NULL || num <= 0 || ret_data == NULL)
		return -1;

	for( i = 0; i < (num/4); i++)
	{
		tmp[i*4+2] = *set_data++;
		tmp[i*4+3] = *set_data++;
		tmp[i*4] = *set_data++;
		tmp[i*4+1] = *set_data++;
	}

	memcpy(ret_data, tmp, num);
	return 1;
}

int get_value_str(char *data, uint16_t num,char *ret_data)
{
	int i = 0;
	char tmp[100]={'\0'};

	if(data == NULL || num <= 0 || ret_data == NULL)
		return -1;

	for( i = 0; i < (num/2); i++)
	{
		tmp[i*2+1] = *data++;
		tmp[i*2] = *data++;
	}

	if((num%2) != 0)
	{
		tmp[i*2+1] = *data;
		tmp[i*2] = 0x00;
	}

	memcpy(ret_data, tmp, num);
	return 1;
}
