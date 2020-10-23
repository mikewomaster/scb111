/*
 * STUB
*/
#include <delay.h>
#include <usbd_cdc_if.h>
#include <stdint.h>
#include "function.h"
#include "param_init.h"
#include "modbus_handle.h"
#include "modbus_rtu.h"
#include "modbus_rtu_sensor.h"

extern sensorData g_sensorData;
extern Param_Config_t param_config_tbl[];
extern mod_name_t mod_name_default;
extern software_ver sw_v_default;
extern int nbiot_read_status(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n);
extern int get_value_uint32(char *set_data, uint16_t num,char *ret_data);
extern uint32_t g_restore_flag;
extern uint32_t g_uart_config_change;

uart_config_t uart_config_default = {0, 8, 1, 0, 1};
uart_config_t uart_config;

__align(4) nbiot_config_t nbiot_config_default={"internet", "", "", 0};
nbiot_config_t nbiot_config;
uint8_t b_nbiot_Config_Changed = 0;

//__align(4) mqtt_config_t mqtt_config_default={"afalcolea.duckdns.org", 1883, "mqttchannel", "", "mqttuser", "Mqttuser@1", 900};
__align(4) mqtt_config_t mqtt_config_default={"", 1883, "", "", 60, "", "", 100};
mqtt_config_t mqtt_config;
uint8_t b_mqtt_Config_Changed = 0;

__align(4) userinfo user_info_default = {{'a','d','m','i','n',0},{'a','d','m','i','n',0}};
__align(4) rtuModbus rtuModbus_default;

ssl_result_t ssl_result;

void Modbus_Slave_USBPutData(uint8_t *buf, uint32_t len)
{
	// FIX ME: reason for delay
	DelayMs(500);
  CDC_Transmit_FS(buf, len);
	DelayMs(500);
}

/*
 * All for write process
*/
uint8_t Write_Param(uint16_t *data, uint16_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t tmp[300];
	uint8_t tmp2[32];	
	uint8_t sn[32];	
	uint8_t res, i;
	uint8_t* p = (uint8_t*)data;
	
	if( part_n == PARTITION_UART)
	{
		memcpy( tmp, (uint8_t*)&uart_config, sizeof(uart_config));
		for( i=0;i<num/2;i++ )
		{
			tmp[i*2+offset] = *p++;
			tmp[i*2+1+offset] = *p++;	
		}			
	}			
	else if (part_n == PARTITION_USER_INFO)
	{
		return 0;
	}
	else if( part_n == PARTITION_MODEL_NAME || part_n == PARTITION_SN )
	{
		// model name & sn
		for( i=0;i<num;i++ )
			tmp[i+offset] = *p++;		
	}
	else if( part_n == 6 )
	{
		// response timeout
		for( i=0;i<num;i++ )
			tmp[i+offset] = *p++;		
	}
	else if( part_n == PARTITION_SW_VERSION )
	{
		return 0;
	}
	else if( part_n == PARTITION_NBIOT )
	{
		memcpy( tmp, (uint8_t*)&nbiot_config, sizeof(nbiot_config_t));
		for( i=0;i<num;i++ )
			tmp[i+offset] = *p++;
	}
	else if( part_n == PARTITION_MQTT )
	{
		memcpy( tmp, (uint8_t*)&mqtt_config, sizeof(mqtt_config_t));
		for( i=0;i<num;i++ )
			tmp[i+offset] = *p++;
	}
	else if( part_n == PARTITION_MEM_CHECKUSER )
	{
		for( i=0; i< (num / 2); i++ )
		{
			tmp[i*2+1] = *p++;
			tmp[i*2] = *p++;	
		}				
		
		// FIX ME ?
		return 0;
		
		// login function
		if( read_partition(PARTITION_USER_INFO, tmp2, param_config_tbl[PARTITION_USER_INFO].num) < 0 )
		{

		}
		
		// compare username
		if( memcmp( tmp, tmp2, 16) != 0 )
		 	return 0x8;
		// compare password
		if( memcmp( tmp+16, tmp2+16, 16) == 0 )
		 	return 0;
		// compare with sn
		if( read_partition(PARTITION_SN, sn, param_config_tbl[PARTITION_SN].num) < 0 )
		{
			return 0x8;
		}		
		p = (uint8_t*)data;
		if(memcmp( p+16, sn, 16) == 0)
		 	return 0;
		
		return 0x8;
	}
	else if(part_n == PARTITION_MEM_RESET)
	{
		if( *data != 1 )
			return 0;
		if( 0 != write_partition(PARTITION_UART, (char *)&uart_config_default, param_config_tbl[0].num))
			return -1;
		// if( 0!= write_partition(PARTITION_USER_INFO, (char *)&user_info_default, 	 param_config_tbl[8].num))
			// return -1;
		if( 0 != write_partition(PARTITION_NBIOT, (char *)&nbiot_config_default, sizeof(nbiot_config_t)))
			return -1;
		if( 0 != write_partition(PARTITION_MQTT, (char *)&mqtt_config_default, sizeof(mqtt_config_t)))
			return -1;
		if( 0 != write_partition(PARTITION_MODBUS_RTU, (char *)&rtuModbus_default, sizeof(rtuModbus)))
			return -1;

		g_restore_flag = 1;
		return 0;
	}

	res = write_partition(part_n, tmp, param_config_tbl[part_n].num);
	if( res == 0 )
	{
		if( part_n == PARTITION_UART )
		{
			memcpy( (uint8_t*)&uart_config, tmp, sizeof(uart_config));
			g_uart_config_change = 1;
		}
	}
	return res;
}

int nbiot_write_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	char tmp[100];
	uint8_t res;
	uint8_t* p = (uint8_t*)data;
	int cpy_num = 0;

	if(data == NULL || num <= 0)
		return -1;

	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, (uint8_t*)&nbiot_config, sizeof(nbiot_config_t));
	switch(offset)
	{
		case 0:
		{
			cpy_num = 32;
			memset(&tmp[offset], '\0', cpy_num);
			set_value_str(p, num, &tmp[offset]);
			break;
		}
		case 32:
		case 48:
		{
			cpy_num = 16;
			memset(&tmp[offset], '\0', cpy_num);
			set_value_str(p, num, &tmp[offset]);
			break;
		}
		case 64:
			set_value_uint16(p, num, &tmp[offset]);
			break;
		default:
			break;
		}

	res = write_partition(part_n, tmp, sizeof(nbiot_config_t));
	if( res == 0 )
	{
		if(memcmp(tmp, &nbiot_config, sizeof(nbiot_config_t)) != 0)
		{
				memcpy( (uint8_t*)&nbiot_config, tmp, sizeof(nbiot_config_t));
				b_nbiot_Config_Changed = 1;
		}
	}

	return res;
}

int mqtt_write_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	char tmp[200];
	uint8_t res;
	uint8_t* p = (uint8_t*)data;
	char *p_mqtt = (char *)&mqtt_config;
	if(data == NULL || num <= 0)
		return -1;

	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, (uint8_t*)&mqtt_config, sizeof(mqtt_config_t));
	memset(&tmp[offset], '\0', num);

	switch(offset)
	{
		case 0:
		case 34:
		case 98:
		case 134:
		case 150:
			if(offset == 34)
				memset(&tmp[offset], '\0', 64);
			else
				memset(&tmp[offset], '\0', 32);		
			set_value_str(p, num, &tmp[offset]);
			break;

		case 32:
		case 166:
			set_value_uint16(p, num, &tmp[offset]);	
			break;
		
		case 130:
			set_value_uint32(p, num, &tmp[offset]);
			break;
	}

	res = write_partition(part_n, tmp, sizeof(mqtt_config_t));
	if( res == 0 ) {
		memcpy((uint8_t*)&mqtt_config, tmp, sizeof(mqtt_config_t));
		b_mqtt_Config_Changed = 1;
	}
	return res;
}

int modbus_rtu_write_cfg(uint16_t *data, uint16_t offset, uint16_t num, uint16_t part_n)
{
	char tmp[14];
	uint8_t res;
	uint8_t* p = (uint8_t*)data;
	int modbusRTUSeq = 0;

	if(num <= 0)
		return -1;

	modbusRTUSeq = offset / 14;

	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, p, sizeof(tmp));

	memcpy(&g_sensorData.sensorConf.rtuModbusTable[modbusRTUSeq], p, sizeof(tmp));
	g_sensorData.value[modbusRTUSeq] = 0;
	
	res = write_partition_more_seq(part_n, tmp, sizeof(rtuUnit), modbusRTUSeq);
	if( res == 0 )
		modbusRTUSeq = 0;

	return res;
}

int sslResWrite(uint16_t *data, uint16_t offset, uint16_t num, uint16_t part_n)
{
	char tmp[200];
	uint8_t res;
	uint8_t* p = (uint8_t*)data;
	char *p_ssl = (char *)&ssl_result;
	if(data == NULL || num <= 0)
		return -1;

	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, (uint8_t*)&ssl_result, sizeof(ssl_result));
	memset(&tmp[offset], '\0', num);
	memcpy(&tmp[offset], p, num);
	
	res = write_partition(part_n, tmp, sizeof(ssl_result_t));
	if( res == 0 ) {
		memcpy((uint8_t*)&ssl_result, tmp, sizeof(ssl_result_t));
	}

	return res;
}

uint8_t Do_W_HOLD_REG_Function(uint16_t addr, uint16_t *data, uint16_t num, uint8_t n)	
{

	if ((addr >= MODBUSRTU_VALUE_START) && (addr <= MODBUSRTU_VALUE_END))
	{
		return modbus_rtu_write_cfg(data, (addr-MODBUSRTU_VALUE_START)*2, (uint16_t)num, n);
	}
	else if((addr >= NBIOT_VALUE_START) && (addr <= NBIOT_VALUE_END))
	{
		return nbiot_write_cfg(data, (addr-NBIOT_VALUE_START)*2, (uint16_t)num, n);
	}
	else if((addr >= MQTT_VALUE_START) && (addr <= MQTT_VALUE_END))
	{
		return mqtt_write_cfg(data, (addr-MQTT_VALUE_START)*2, (uint16_t)num, n);
	}
	else if (addr >= SSL_VALUE_START && addr <= SSL_VALUE_END)
	{
		return sslResWrite(data, (addr-SSL_VALUE_START)*2, (uint16_t)num, n);
	}
	else if(addr < 1000)
	{
		return Write_Param(data, (addr-param_config_tbl[n].base_addr)*2, num, n);
	}
	else
	{
		return 1;
	}
}

/*
 * All for read process
*/
void Read_Param(uint16_t *data, uint16_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t tmp[100];
	uint8_t i;
	uint8_t* p = (uint8_t*)data;

	if( part_n == PARTITION_MODEL_NAME)
	{
		for(i=0;i<16;i++)
		{
			*(p+i*2+1) = mod_name_default.name[i*2];
			*(p+i*2) = mod_name_default.name[i*2+1];
		}
		return;
	}

	if( part_n == PARTITION_SW_VERSION )
	{
		for(i=0;i<16;i++)
		{
			*(p+i*2+1) = sw_v_default.data[i*2];
			*(p+i*2) = sw_v_default.data[i*2+1];
		}		
		return;		
	}
	
	if( read_partition(part_n, (char *)tmp, param_config_tbl[part_n].num) < 0 )
	{
		memset(p, 0xff, num);
	}
	else
	{
		memcpy(p, tmp+offset, num);
		if( part_n == PARTITION_USER_INFO )
		{
			for(i=0;i<num/2;i++)
			{
				*(p+i*2+1) = (tmp+offset)[i*2];
				*(p+i*2) = (tmp+offset)[i*2+1];
			}
		}
	}
}

int nbiot_read_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t *pcfg;
	uint8_t *p = (uint8_t *)data;
	uint16_t i;
	char tmp[100]={'\0'};
	int ret = -1;

	pcfg = (uint8_t *)&nbiot_config;

	if (read_partition(part_n, tmp, sizeof(nbiot_config_t)) < 0) 
		{
		switch(offset)
			{
			case 64:
				memcpy(p, (pcfg+offset), num);
				ret = 1;
				break;
			default:
				ret = get_value_str((char *)pcfg+offset, num, p);
				break;
			}
		}
	else
		{
		switch(offset)
			{
			case 64:
				memcpy(p, (tmp+offset), num);
				ret = 1;
				break;
			default:
				ret = get_value_str(tmp+offset, num, p);
				break;
			}
		}
	
	return ret;
}

int mqtt_read_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t *pcfg;
	uint8_t *p = (uint8_t *)data;
	char tmp[200]={'\0'};
	int ret = -1;

	pcfg = (uint8_t *)&mqtt_config;
	if (read_partition(part_n, tmp, sizeof(mqtt_config_t)) < 0) 
	{
		switch(offset)
		{
			case 32:
			case 166:
				memcpy(p, (pcfg+offset), num);
				ret = 1;
				break;
			case 130:			
				ret = get_value_uint32(pcfg+offset, num, p);
				break;
			default:
				ret = get_value_str((char *)pcfg+offset, num, p);
				break;
		}
	}
	else
	{
		switch(offset)
		{
			case 32:
			case 166:
				memcpy(p, (tmp+offset), num);
				ret = 1;
				break;
			case 130:
				ret = get_value_uint32(tmp+offset, num, p);
				break;
			default:
				ret = get_value_str(tmp+offset, num, p);
				break;
		 }
	}

	return ret;
}

extern sensorData g_sensorData;

int modbus_rtu_read_cfg(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t *p = (uint8_t *)data;
	rtuUnit tmp[10];
	
	int ret = 0;
	if (read_partition_modbus_rtu(part_n, (char *)tmp, sizeof(rtuModbus)/2, offset) != -1) {
		for (int i = 0; i < 10; i++) {
			 if (offset > 0)
				tmp[i].num = g_sensorData.value[i + 10];
			 else
				tmp[i].num = g_sensorData.value[i];
		}

		memcpy(p, tmp, sizeof(rtuModbus)/2);
	}

	return ret;
}

int sslResRead(uint16_t *data, uint8_t offset, uint16_t num, uint16_t part_n)
{
	uint8_t *pcfg;
	uint8_t *p = (uint8_t *)data;
	char tmp[200]={'\0'};
	int ret = 0;
	pcfg = (uint8_t *)&ssl_result;

	if (read_partition(part_n, tmp, sizeof(ssl_result)) < 0)
		memcpy(p, pcfg + offset, num);
	else
		memcpy(p, tmp + offset, num);

	return ret;
}

void Do_R_HOLD_REG_Function(unsigned short addr, unsigned short* data, uint16_t num, uint8_t n)
{
	if ((addr >= MODBUSRTU_VALUE_START) && (addr <= MODBUSRTU_VALUE_END))
	{
		modbus_rtu_read_cfg(data, (addr-MODBUSRTU_VALUE_START)*2, (uint16_t)num, n);
	}
	else if (addr >= SSL_VALUE_START && addr <= SSL_VALUE_END)
	{
		sslResRead(data, (addr-SSL_VALUE_START)*2, (uint16_t)num, n);
	}
	else if(addr < 1000)
	{
		Read_Param(data, (addr-param_config_tbl[n].base_addr)*2, (uint16_t)num, n);
	}
	else if((addr >= NBIOT_VALUE_START) && (addr <= NBIOT_VALUE_END))
	{
		nbiot_read_cfg(data, (addr-NBIOT_VALUE_START)*2, (uint16_t)num, n);
	}
	else if((addr >= NBIOT_STATUS_VALUE_START) && (addr <= NBIOT_STATUS_VALUE_END))
	{
		nbiot_read_status(data, (addr-NBIOT_STATUS_VALUE_START)*2, (uint16_t)num, n);
	}
	else if((addr >= MQTT_VALUE_START) && (addr <= MQTT_VALUE_END))
	{
		mqtt_read_cfg(data, (addr-MQTT_VALUE_START)*2, (uint16_t)num, n);
	}
	else if((addr >= MQTT_STATUS_VALUE_START) && (addr <= MQTT_STATUS_VALUE_END))
	{
		mqtt_read_status(data, (addr-MQTT_STATUS_VALUE_START)*2, (uint16_t)num, n);
	}
}
