#include <stdint.h>

#include "gsModbus.h"
#include "string.h"
#include "modbus_handle.h"
#include "param_init.h"

unsigned int gs_baudrate[10] = {9600, 300,1200,2400,4800,9600,19200,38400,57600,115200};

extern Param_Config_t param_config_tbl[];
	
uint8_t Modbus_Frame_Check(uint8_t func, uint16_t addr, uint16_t *data, uint16_t num)
{
	uint8_t i;
	
	if( func != 0x03 && func != 0x06 && func != 0x10 ) 
	{
		return 0xff;
	}
	else if ((addr >= MODBUSRTU_VALUE_START) && (addr <= MODBUSRTU_VALUE_END))
	{
		return PARTITION_MODBUS_RTU;
	}
	else if(addr < 1000)
	{
		for(i = 0; i < 10; i++)
		{
			if( addr>=param_config_tbl[i].base_addr && (addr+num/2-1)<(param_config_tbl[i].base_addr+param_config_tbl[i].num/2) )
				return param_config_tbl[i].part_tbl_n;		
		}
	}
	else if((addr >= NBIOT_VALUE_START) && (addr <= NBIOT_VALUE_END))
	{
		return PARTITION_NBIOT;
	}
	else if((addr >= NBIOT_STATUS_VALUE_START) && (addr <= NBIOT_STATUS_VALUE_END))
	{
		return PARTITION_NBIOT;
	}

	else if((addr >= MQTT_VALUE_START) && (addr <= MQTT_VALUE_END))
	{
		return PARTITION_MQTT;
	}
	else if((addr >= MQTT_STATUS_VALUE_START) && (addr <= MQTT_STATUS_VALUE_END))
	{
		return PARTITION_MQTT;
	}
	else
	{
		return 0xff;
	}
	
	if( addr == 2001)
	{
		return 'T';
	}
		
	return 0xfe;
}

void exchangeBytes(unsigned char* byte)
{
    unsigned char	tmp;
	
		tmp = *(byte+1);
		*(byte+1) = *byte;
    *byte = tmp;
    return;
}

static unsigned short crc_Compute(unsigned char *PBuf,unsigned char len)
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

void gsModbus_Set_SendFunction(gsModbus *pModbus,void (*fun)(unsigned char *PBuf, unsigned int length))
{
	if(fun == 0){return;}
	pModbus->gsModbus_PutData = fun;
}

void gsModbus_Set_ADDR(gsModbus *pModbus,unsigned char addr)
{
	pModbus->Address = addr;
}

void gsModbus_Dev_Coil_Function(gsModbus *pModbus, void (*fun)(unsigned short addr))
{

}

void gsModbus_Write_REG_Function(gsModbus *pModbus, unsigned char (*fun)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n))
{
	pModbus->Set_HOLD_Function = fun;
}

void gsModbus_Read_REG_Function(gsModbus *pModbus, void (*fun)(unsigned short addr, unsigned short* data, unsigned short num, unsigned char n))
{
	pModbus->Read_HOLD_Function = fun;
}

void gsModbus_Beforehand_Function(gsModbus *pModbus, void (*fun)(unsigned char *pBuf, unsigned int size))
{
	pModbus->gsModbus_forword = fun;
}

void gsModbus_Processor(gsModbus *pModbus,unsigned char *pBuf,unsigned int len)
{
	unsigned short Check_Crc = 0x00;																			
	
	// if(pModbus->gsModbus_PutData == 0x00){return;}													
	if(pBuf == 0x00 || len < 2){return;}																																
	Check_Crc = pBuf[len-1]*256+pBuf[len-2];
	if(Check_Crc == crc_Compute(pBuf,len-2))
	{
		// handle frame by LM self 
		if( pBuf[0x00] == pModbus->Address )
		{
			pModbus->Function_Code = pBuf[0x01];																	
			pModbus->Regist_Addr 	= pBuf[0x02]*256+pBuf[0x03];										
			pModbus->Regist_Size 	= pBuf[0x04]*256+pBuf[0x05];										
			
			switch(pModbus->Function_Code)
			{
				case READ_COIL_STA:
				case READ_INPUT_STA:
					gsModbus_01_02_Response(pModbus);	
					break;			
				case READ_HOLD_REG:
				case READ_INPUT_REG:
					gsModbus_03_04_Response(pModbus);										
					break;			
				case WRITE_COIL_REG:			
				case WRITE_HOLD_REG:
					gsModbus_05_06_Response(pModbus);	
					break;
				case WRITE_HOLD_W_REG:
					gsModbus_16_Response(pModbus, pBuf);	
					break;
				default:
					gsModbus_Exception_Response(MB_EX_ILLEGAL_FUNCTION, pModbus);
					break;				
			}
		}
		else
			pModbus->gsModbus_forword(pBuf, len);
	}
	// handle timeout
	
}

void gsModbus_Exception_Response(unsigned char err_code, gsModbus *pModbus)
{
	unsigned char tx_buf[100]	=	{0};
	unsigned char *Tx_pBuff		= tx_buf;
	unsigned short reply_crc 	= 0x00;
	
	*(Tx_pBuff++) = pModbus->Address;																													
	*(Tx_pBuff++) = pModbus->Function_Code+0x80;
	*(Tx_pBuff++) = err_code;
		reply_crc = crc_Compute(tx_buf,3);																	
	*(Tx_pBuff++) =(unsigned char)reply_crc;																									
	*(Tx_pBuff++) =(unsigned char)(reply_crc>>8);	
	pModbus->gsModbus_PutData(tx_buf,5);	
}

void gsModbus_01_02_Response(gsModbus *pModbus)
{
	unsigned char res;
	res = Modbus_Frame_Check(pModbus->Function_Code, pModbus->Regist_Addr+1, 0, 2);
	if( res >= 0xf0 )
	{
		gsModbus_Exception_Response((~res)+1, pModbus);
		return;
	}
}
		
void gsModbus_03_04_Response(gsModbus *pModbus)
{
	unsigned char tx_buf[253]	=	{0};
	unsigned char *Tx_pBuff		= tx_buf;
	unsigned short reply_crc 	= 0x00;
	unsigned short funcVal;
	unsigned char i, res;

	*(Tx_pBuff++) = pModbus->Address;
	*(Tx_pBuff++) = pModbus->Function_Code;
	*(Tx_pBuff++) =(unsigned char)(pModbus->Regist_Size*2);

	res = Modbus_Frame_Check(pModbus->Function_Code, pModbus->Regist_Addr+1, 0, pModbus->Regist_Size*2);
	if( res >= 0xf0 )
	{
		gsModbus_Exception_Response((~res)+1, pModbus);
		return;
	}

	pModbus->Read_HOLD_Function(pModbus->Regist_Addr+1, (unsigned short*)Tx_pBuff, pModbus->Regist_Size*2, res);
	for( i=0;i<pModbus->Regist_Size;i++ )
	{
		exchangeBytes(Tx_pBuff);
		Tx_pBuff += 2;
	}

	reply_crc = crc_Compute(tx_buf,3+pModbus->Regist_Size*2);
	*(Tx_pBuff++) =(unsigned char)reply_crc;
	*(Tx_pBuff++) =(unsigned char)(reply_crc>>8);
	pModbus->gsModbus_PutData(tx_buf,5+pModbus->Regist_Size*2);
}
 
void gsModbus_05_06_Response(gsModbus *pModbus)
{
	unsigned char tx_buf[100]={0};																				
	unsigned char *Tx_pBuff			= tx_buf;																	
	unsigned short reply_crc 		= 0x00;																		

	unsigned char res, res2;

	// register addr valid?
	res = Modbus_Frame_Check(pModbus->Function_Code, pModbus->Regist_Addr+1, &pModbus->Regist_Size, 2);
	if( res >= 0xf0 )
	{
		gsModbus_Exception_Response((~res)+1, pModbus);
		return;
	}
	
	if( pModbus->Function_Code == WRITE_COIL_REG )
	{													
		
	}
	else if( pModbus->Function_Code == WRITE_HOLD_REG )
	{																			 
		res2 = pModbus->Set_HOLD_Function( pModbus->Regist_Addr+1, &pModbus->Regist_Size, 2, res );
		if( res2 != 0)
		{
			// device error
			gsModbus_Exception_Response(4, pModbus);
			return;
		}
	}

	*(Tx_pBuff++) = pModbus->Address;																			
	*(Tx_pBuff++) = pModbus->Function_Code;																
	*(Tx_pBuff++) = (unsigned char)(pModbus->Regist_Addr>>8);							
	*(Tx_pBuff++) = (unsigned char)pModbus->Regist_Addr;									
	*(Tx_pBuff++) =	(unsigned char)(pModbus->Regist_Size>>8);							
	*(Tx_pBuff++) =	(unsigned char)(pModbus->Regist_Size);									
	reply_crc = crc_Compute(tx_buf,6);																		
	*(Tx_pBuff++) =(unsigned char)reply_crc;															
	*(Tx_pBuff++) =(unsigned char)(reply_crc>>8);													
	pModbus->gsModbus_PutData(tx_buf,8);																	
}

void gsModbus_16_Response(gsModbus *pModbus, unsigned char *pBuf)
{
	unsigned char tx_buf[100]={0};																				
	unsigned char *Tx_pBuff	= tx_buf;																	
	unsigned short i, reply_crc = 0x00;		

	unsigned char funcode = 0xff;
	unsigned char res, res2;
	unsigned char *startbyte = pBuf+7;

	// register addr valid?
	for( i=0;i<pModbus->Regist_Size;i++ )
	{
		exchangeBytes(startbyte);
		startbyte += 2;
	}		
	
	res = Modbus_Frame_Check(pModbus->Function_Code, pModbus->Regist_Addr+1, 0, pModbus->Regist_Size*2);
	if( res >= 0xf0 )
	{
		gsModbus_Exception_Response((~res)+1, pModbus);
		return;
	}
	
	// The parameters were checked above
	res2 = pModbus->Set_HOLD_Function(pModbus->Regist_Addr+1, (unsigned short*)(pBuf+7), pModbus->Regist_Size*2, res);
	if( res2 != 0)
	{
		// device error
		gsModbus_Exception_Response(4, pModbus);
		return;
	}
	*(Tx_pBuff++) = pModbus->Address;																			
	*(Tx_pBuff++) = pModbus->Function_Code;																
	*(Tx_pBuff++) = (unsigned char)(pModbus->Regist_Addr>>8);							
	*(Tx_pBuff++) = (unsigned char)pModbus->Regist_Addr;									
	*(Tx_pBuff++) =	(unsigned char)(pModbus->Regist_Size>>8);							
	*(Tx_pBuff++) =	(unsigned char)pModbus->Regist_Size;									
	reply_crc = crc_Compute(tx_buf,6);																		
	*(Tx_pBuff++) =(unsigned char)reply_crc;															
	*(Tx_pBuff++) =(unsigned char)(reply_crc>>8);													
	pModbus->gsModbus_PutData(tx_buf,8);									
}
